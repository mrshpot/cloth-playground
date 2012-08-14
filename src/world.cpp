/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include "world.hpp"


Plane mk_plane(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    Plane res;
    res.d = 0;
    glm::vec3 v1 = p2 - p1;
    glm::vec3 v2 = p3 - p1;
    res.n = glm::normalize(glm::cross(v1, v2));
    res.d = -res.equ(p1); // should work with any point on the plane
    return res;
}