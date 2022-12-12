/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LOG_H
#define _LOG_H

typedef enum {
    LOG_DEBUG = -20,
    LOG_VERBOSE = -10,
    LOG_NORMAL = 0,
    LOG_WARNING = 10,
    LOG_ERROR = 20
} log_level_t;

extern void (*log_sink)(int, const char *, void *);
extern log_level_t global_log_level;
extern void *log_data;

void lprintf(log_level_t level, const char *fmt, ...);
void log_sink_default(int level, const char *msg, void *data);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
