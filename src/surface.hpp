/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#ifndef SURFACE_HPP__INCLUDED
#define SURFACE_HPP__INCLUDED

#include <glm/glm.hpp>


class Surface
{
public:
    Surface(size_t rows, size_t cols);
    ~Surface();

    void lock();
    void unlock();

    void draw();

    glm::vec3& pos_at(int i, int j) { return m_points[i * m_cols + j].pos; }
    void make_plane(const glm::vec3 &origin,
                    const glm::vec3 &dir1, const glm::vec3 &dir2,
                    float len1, float len2);

    size_t rows() { return m_rows; }
    size_t cols() { return m_cols; }

private:
    struct Point
    {
        glm::vec3 pos;
    };

    Point *m_points;
    unsigned int m_vertex_buffer, m_index_buffer;
    bool m_locked;
    size_t m_rows, m_cols;
    size_t m_num_points, m_num_indices;

    void upload();
    void gen_indices();

};

#endif // SURFACE_HPP__INCLUDED
