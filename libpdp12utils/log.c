/*
 * Copyright (c) 2006, 2007 Andreas Sandberg
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, arepermitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *        * Neither the name of the Greenpea project nor the names of its
 *          contributors may be used to endorse or promote products derived
 *          from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
