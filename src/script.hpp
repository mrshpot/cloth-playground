/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#ifndef SCRIPT_HPP__INCLUDED
#define SCRIPT_HPP__INCLUDED

#include <string>


class ScriptImpl;
struct World;

class Script
{
    ScriptImpl *m;
    
public:
    Script(World &world);
    ~Script();

    bool load(const char *fname, std::string *error_msg);
    
    void init();
    void reset();
    void update(double dt);
};

#endif // SCRIPT_HPP__INCLUDED
