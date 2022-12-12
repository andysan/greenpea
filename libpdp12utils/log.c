/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "log.h"

void (*log_sink)(int, const char *, void *) = &log_sink_default;
log_level_t global_log_level = LOG_NORMAL;
void *log_data = NULL;


void
lprintf(log_level_t level, const char *fmt, ...) {
    if (level < global_log_level)
        return;

    va_list ap1;
    va_list ap2;
    char *msg;
    int len;

    va_start(ap1, fmt);
    va_copy(ap2, ap1);

    len = vsnprintf(NULL, 0, fmt, ap1);

    msg = malloc(len + 1);
    if (msg) {
        vsnprintf(msg, len + 1, fmt, ap2);
        log_sink(level, msg, log_data);
        free(msg);
    }

    va_end(ap2);
    va_end(ap1);
}

void
log_sink_default(int level, const char *msg, void *data) {
    if (level >= LOG_WARNING)
        fprintf(stderr, "%s", msg);
    else
        fprintf(stdout, "%s", msg);
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
