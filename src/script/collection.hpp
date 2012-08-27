/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#ifndef SCRIPT_COLLECTION_HPP__INCLUDED
#define SCRIPT_COLLECTION_HPP__INCLUDED

#include <vector>


struct lua_State;

void collection_register(lua_State *L);

template <typename T>
void collection_new(lua_State *L, std::vector<T*> &collection);

#endif // SCRIPT_COLLECTION_HPP__INCLUDED
