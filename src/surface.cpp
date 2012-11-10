/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <GL/glew.h>
#include <GL/gl.h>

#include "surface.hpp"


Surface::Surface(size_t rows, size_t cols)
    : m_locked(false)
    , m_rows(rows)
    , m_cols(cols)
{
    assert(rows >= 2);
    assert(cols >= 2);

    glGenBuffersARB(1, &m_vertex_buffer);
    glGenBuffersARB(1, &m_index_buffer);

    m_num_points = rows * cols;
    m_points = new Point[m_num_points];
    memset(m_points, m_num_points * sizeof(m_points[0]), 0);

    gen_indices();
    upload();
}

Surface::~Surface()
{
    glDeleteBuffersARB(1, &m_vertex_buffer);
    glDeleteBuffersARB(1, &m_index_buffer);
}

void Surface::lock()
{
    assert(!m_locked);
    m_locked = true;
}

void Surface::unlock()
{
    assert(m_locked);
    m_locked = false;
    upload();
}

void Surface::draw()
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertex_buffer);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_index_buffer);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, sizeof(m_points[0]),
                    (GLvoid*)offsetof(Point, pos));
    glIndexPointer(GL_UNSIGNED_INT, sizeof(GLuint), (GLvoid*)NULL);

    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, NULL);
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
}

void Surface::upload()
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertex_buffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB,
                    sizeof(m_points[0]) * m_num_points,
                    m_points,
                    GL_DYNAMIC_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void Surface::gen_indices()
{
    // 2 triangles per each quad cell
    size_t cell_rows = m_rows - 1;
    size_t cell_cols = m_cols - 1;
    
    m_num_indices = cell_rows * cell_cols * 2 * 3;
    GLuint *indices = new GLuint[m_num_indices];

    // iterate over each cell (we have rows-1 x cols-1 quad cells)
    for (size_t i = 0; i < cell_rows; ++i)
    {
        for (size_t j = 0; j < cell_cols; ++j)
        {
            // add two CCW triangles for each
            size_t point_idx = i * m_cols + j;
            size_t idx_idx = (i * cell_cols + j) * 6;
            
            indices[idx_idx + 0] = point_idx;
            indices[idx_idx + 1] = point_idx + 1;
            indices[idx_idx + 2] = point_idx + m_cols;

            indices[idx_idx + 3] = point_idx + m_cols;
            indices[idx_idx + 4] = point_idx + 1;
            indices[idx_idx + 5] = point_idx + m_cols + 1;
        }
    }
    
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_index_buffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,
                    sizeof(indices[0]) * m_num_indices,
                    indices,
                    GL_STATIC_DRAW_ARB);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

    delete[] indices;
}
