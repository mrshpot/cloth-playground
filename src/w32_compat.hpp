/*
 * Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

 #ifndef W32_COMPAT_HPP__INCLUDED
 #define W32_COMPAT_HPP__INCLUDED
 
 
 #ifdef _MSC_VER
 #define snprintf _snprintf
 #endif // _MSC_VER
 
 #endif //  W32_COMPAT_HPP__INCLUDED