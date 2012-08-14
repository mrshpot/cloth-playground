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

    Sphere(const glm::vec3 &origin, float r)
        : origin(origin)
        , r(r)
    {
    }

    Sphere(const Sphere &other)
        : origin(other.origin)
        , r(other.r)
    {
    }
    
    float equ(const glm::vec3 &x) const
    {
        // Equation: (x - Ox)^2 + (y - Oy) ^2 + (z - Oz) ^2 = R^2;
        // (Ox, Oy, Oz) are stored in origin; R is stored in r.
        glm::vec3 v = x - origin;
        return glm::dot(v, v) - (r * r);
    }
};

struct Plane
{
    glm::vec3 n;
    float d;

    Plane(const glm::vec3 &n, float d)
        : n(glm::normalize(n))
        , d(d)
    {
    }

    /// Compute the equation coefficients from three points in CCW order
    Plane(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
    {
        glm::vec3 v1 = p2 - p1;
        glm::vec3 v2 = p3 - p1;
        
        n = glm::normalize(glm::cross(v1, v2));
        d = 0;
        d = -equ(p1); // should work with any point on the plane
    }
    
    Plane(const Plane &other)
        : n(other.n)
        , d(other.d)
    {
    }
    
    float equ(const glm::vec3 &x) const
    {
        // Equation: Ax + By + Cz + D = 0;
        // (A, B, C) (normalized) are stored in n; D is stored in d.
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

#endif // WORLD_HPP__INCLUDED
