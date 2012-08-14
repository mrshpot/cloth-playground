/*
 * Portions Copyright (c) 2012, Taras Shpot
 * All rights reserved. Email: mrshpot@gmail.com
 * 
 * This demo is free software; you can redistribute it and/or modify
 * it under the terms of the BSD-style license that is included in the
 * file LICENSE.
 */

#include "platform.hpp"
#ifdef PLATFORM_WINDOWS

#include <windows.h>


// Origin: http://stackoverflow.com/questions/2494356/how-to-use-gettimeofday-or-something-equivalent-with-visual-studio-c-2008

struct w32_timeval {
    __int32    tv_sec;         /* seconds */
    __int32    tv_usec;        /* microseconds */
};

static const __int64 DELTA_EPOCH_IN_MICROSECS = 11644473600000000;

void w32_gettimeofday(struct w32_timeval *tv)
{
    FILETIME ft;
    __int64 tmpres = 0;

    ZeroMemory(&ft,sizeof(ft));

    GetSystemTimeAsFileTime(&ft);

    tmpres = ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tv->tv_sec = (__int32)(tmpres*0.000001);
    tv->tv_usec =(tmpres%1000000);
}

double ptime()
{
    struct w32_timeval tv;
    w32_gettimeofday(&tv);
    return (double)tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

#endif // PLATFORM_WINDOWS
