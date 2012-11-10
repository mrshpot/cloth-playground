/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#ifndef CLOTH_HPP__INCLUDED
#define CLOTH_HPP__INCLUDED

#include "surface.hpp"
#include "world.hpp"


class Cloth
{
public:
    struct Point
    {
        glm::vec3 pos;
    };

    Cloth(float width, float height, size_t rows, size_t cols, const World &world);
    ~Cloth();

    void lock();
    void unlock();

    void reset_velocity();
    
    Point& at(int i, int j) { return m_points[i * m_cols + j]; }
    void draw();

    void step(float timestep);

    size_t rows() { return m_rows; }
    size_t cols() { return m_cols; }
    float width() { return m_width; }
    float height() { return m_height; }

private:
    Point *m_points, *m_prev_points;
    bool m_locked;
    float m_prev_dt;
    glm::vec3 m_gravity;
    const World &m_world;
    Surface m_surface;
    
    float m_width, m_height;
    size_t m_rows, m_cols;
    float m_dist_to_left, m_dist_to_bottom;
    
    size_t m_num_points, m_num_indices;
    
    void upload();

    void copy_current_to_prev();

    void apply_plane_constraints();
    void apply_sphere_constraints();
    void apply_spring_constraints();
};

#endif // CLOTH_HPP__INCLUDED
