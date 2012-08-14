/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#ifndef WORLD_HPP__INCLUDED
#define WORLD_HPP__INCLUDED

#include <vector>

#include <glm/glm.hpp>


struct Sphere
{
    glm::vec3 origin;
    float r;

    float equ(const glm::vec3 &x) const
    {
        glm::vec3 v = x - origin;
        return glm::dot(v, v) - (r * r);
    }
};

struct Plane
{
    glm::vec3 n;
    float d;

    float equ(const glm::vec3 &x) const
    {
        return glm::dot(n, x) + d;
    }
};

struct World
{
    typedef std::vector<Sphere> sphere_array_t;
    typedef std::vector<Plane> plane_array_t;
    
    sphere_array_t spheres;
    plane_array_t planes;
};

/// For a CCW triangle, compute plane equation coefficients
Plane mk_plane(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3);

#endif // WORLD_HPP__INCLUDED
