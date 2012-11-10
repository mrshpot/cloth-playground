/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <algorithm>

#include <GL/glew.h>
#include <GL/gl.h>

#include "cloth.hpp"


const char* gl_error_str(GLenum error)
{
    switch (error)
    {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Enum argument out of range";
        case GL_INVALID_VALUE:
            return "Numeric argument out of range";
        case GL_INVALID_OPERATION:
            return "Operation illegal in current state";
        case GL_STACK_OVERFLOW:
            return "Command would cause a stack overflow";
        case GL_STACK_UNDERFLOW:
            return "Command would cause a stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Not enough memory left to execute the command";
        case GL_TABLE_TOO_LARGE:
            return "The specified table is too large";
        default:
            return "Unknown error";
    }
}

#define CHECK_GL_ERROR()                                                \
    do {                                                                \
        GLenum tmp__error = glGetError();                               \
        if (tmp__error != GL_NO_ERROR)                                  \
        {                                                               \
            fprintf(stderr, "OpenGL error %d: %s", gl_error_str(tmp__error)); \
            assert(tmp__error == GL_NO_ERROR);                          \
        }                                                               \
    } while(false)


Cloth::Cloth(float width, float height, size_t rows, size_t cols, const World &world)
    : m_prev_dt(-1.0f)
    , m_gravity(glm::vec3(0.0f, -0.9f, 0.0f))
    , m_world(world)
    , m_surface(rows, cols)
    , m_width(width)
    , m_height(height)
    , m_rows(rows)
    , m_cols(cols)
{
    assert(rows >= 2);
    assert(cols >= 2);
    
    m_dist_to_left = m_width / (m_cols - 1.0f);
    m_dist_to_bottom = m_height / (m_rows - 1.0f);
    
    m_num_points = rows * cols;
    m_points = new Point[m_num_points];
    m_prev_points = new Point[m_num_points];
    m_spring_phase_buf = new Point[m_num_points];

    memset(m_points, m_num_points * sizeof(m_points[0]), 0);
    memset(m_prev_points, m_num_points * sizeof(m_points[0]), 0);
}

Cloth::~Cloth()
{
    delete[] m_points;
    delete[] m_prev_points;
    delete[] m_spring_phase_buf;
}

void Cloth::lock()
{
    m_surface.lock();
}

void Cloth::unlock()
{
    upload();
    m_surface.unlock();
}

void Cloth::reset_velocity()
{
    copy_current_to_prev();
}

void Cloth::draw()
{
    m_surface.draw();
}

void Cloth::step(float dt)
{
    // Time-corrected Verlet integration as described in:
    // http://lonesock.net/article/verlet.html
    
    if (m_prev_dt < 0)
        m_prev_dt = dt;
    
    float dt2 = dt * dt;
    float dt_coeff = dt / m_prev_dt;

    // apply force
    for (size_t i = 0; i < m_rows; ++i)
    {
        for (size_t j = 0; j < m_cols; ++j)
        {
            size_t idx = i * m_cols + j;
            Point tmp = m_points[idx];
            Point &x = m_points[idx];
            Point &x_prev = m_prev_points[idx];
            m_points[idx].pos = x.pos +
                ((x.pos - x_prev.pos) * 0.99f * dt_coeff) +
                m_gravity * dt2;
            m_prev_points[idx] = tmp;
        }
    }

    apply_spring_constraints();

    apply_plane_constraints();
    apply_sphere_constraints();

    m_prev_dt = dt;
}

void Cloth::upload()
{
    for (size_t i = 0; i < m_rows; ++i)
    {
        for (size_t j = 0; j < m_cols; ++j)
        {
            size_t idx = i * m_cols + j;
            m_surface.pos_at(i, j) = m_points[idx].pos;
        }
    }
}

void Cloth::copy_current_to_prev()
{
    std::copy(m_points, m_points + m_num_points, m_prev_points);
}

void Cloth::apply_plane_constraints()
{
    for (World::plane_array_t::const_iterator it = m_world.planes.begin();
         it != m_world.planes.end(); ++it)
    {
        for (size_t idx = 0; idx < m_num_points; ++idx)
        {
            Point &p = m_points[idx];
            Plane *pl = *it;
            float d = pl->equ(p.pos);
            if (d < 0)
            {
                p.pos -= pl->n * d;
            }
        }
    }
}

void Cloth::apply_sphere_constraints()
{
    for (World::sphere_array_t::const_iterator it = m_world.spheres.begin();
         it != m_world.spheres.end(); ++it)
    {
        Sphere *sp = *it;
        float r = sp->r;
        float r2 = r * r;
        
        for (size_t idx = 0; idx < m_num_points; ++idx)
        {
            Point &p = m_points[idx];
            float d = sp->equ(p.pos);
            if (d < 0)
            {
                glm::vec3 v = p.pos - sp->origin;
                p.pos = (r / sqrtf(r2 + d)) * v + sp->origin;
            }
        }
    }
    
}

static glm::vec3 solve_spring(const glm::vec3 &a, const glm::vec3 &b, float distance)
{
    glm::vec3 ab = b - a;
    float l = sqrt(glm::dot(ab, ab));
    float dl = (l - distance);
    // allow infinite compression
    if (dl < 0.0f) dl = 0.0f;
    return (0.5f * dl / l) * ab;
}

void Cloth::apply_spring_constraints()
{
    static const int num_iterations = 4;
    
    for (int iter = 0; iter < num_iterations; ++iter)
    {
        for (size_t i = 0; i < m_rows; ++i)
        {
            for (size_t j = 0; j < m_cols; ++j)
            {
                size_t idx = i * m_cols + j;
                Point &p = m_points[idx];
                glm::vec3 dx = glm::vec3(0.0f);
                if (i > 0)
                    dx += solve_spring(p.pos, m_points[idx - m_cols].pos,
                                       m_dist_to_bottom);
                if (j > 0) 
                    dx += solve_spring(p.pos, m_points[idx - 1].pos,
                                       m_dist_to_left);
                if (i < (m_rows - 1))
                    dx += solve_spring(p.pos, m_points[idx + m_cols].pos,
                                       m_dist_to_bottom);
                if (j < (m_cols - 1))
                    dx += solve_spring(p.pos, m_points[idx + 1].pos,
                                       m_dist_to_left);
                
                m_spring_phase_buf[idx].pos = p.pos + dx;
            }
        }
        std::swap(m_points, m_spring_phase_buf);
    }
}
