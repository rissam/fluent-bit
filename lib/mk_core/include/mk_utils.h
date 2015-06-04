/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Monkey HTTP Server
 *  ==================
 *  Copyright 2001-2015 Monkey Software LLC <eduardo@monkey.io>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef MK_CORE_UTILS_H
#define MK_CORE_UTILS_H

#include "mk_macros.h"

#include <pthread.h>
#include <string.h>
#include <errno.h>

time_t mk_core_init_time;
char *env_trace_filter;
pthread_mutex_t mutex_trace;

/* Trace definitions */
#ifdef TRACE

#define MK_TRACE_CORE 0
#define MK_TRACE_PLUGIN 1
#define MK_TRACE_COMP_CORE "core"

#define MK_TRACE(...) mk_utils_trace(MK_TRACE_COMP_CORE, MK_TRACE_CORE, \
                                     __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

#else
#define MK_TRACE(...) do {} while (0)
#endif

void mk_print(int type, const char *format, ...) PRINTF_WARNINGS(2,3);

#ifdef TRACE
void mk_utils_trace(const char *component, int color, const char *function,
                    char *file, int line, const char* format, ...);
int mk_utils_print_errno(int n);
#endif


/* Thread key to hold a re-entrant buffer for strerror formatting */
#define MK_UTILS_ERROR_SIZE          128
pthread_key_t mk_utils_error_key;

/*
 * Helpers to format and print out common errno errors, we use thread
 * keys to hold a buffer per thread so strerror_r(2) can be used without
 * a memory allocation.
 */
#define MK_UTILS_LIBC_ERRNO_BUFFER()                                  \
    int _err  = errno;                                                \
    char *buf = pthread_getspecific(mk_utils_error_key);              \
    if (strerror_r(_err, buf, MK_UTILS_ERROR_SIZE) != 0) {            \
        mk_err("strerror_r() failed");                                \
    }

static inline void mk_utils_libc_error(char *caller, char *file, int line)
{
    MK_UTILS_LIBC_ERRNO_BUFFER();
    mk_err("%s: %s, errno=%i at %s:%i", caller, buf, _err, file, line);
}

static inline void mk_utils_libc_warning(char *caller, char *file, int line)
{
    MK_UTILS_LIBC_ERRNO_BUFFER();
    mk_warn("%s: %s, errno=%i at %s:%i", caller, buf, _err, file, line);
}

int mk_core_init();

#endif