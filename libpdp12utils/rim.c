/*
 * SPDX-FileCopyrightText: Copyright 2006-2007, 2010-2011 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <libpdp12utils/log.h>
#include "rim.h"

#define RIM_LEADER_TRAILER 0x80
#define RIM_ADDR 0x40


int
load_rim(FILE *file, int offset, int *core, int core_size) {
    int c ;
    int addr = 0;
    int word = 0;
    int count = 0;

    if (fseek(file, offset, SEEK_SET) == -1) {
        lprintf(LOG_ERROR,
                "Can't set offset 0%.o: %s",
                offset, strerror(errno));
        return -1;
    }

    lprintf(LOG_DEBUG, "Scanning input file...\n");
    do {
        c = fgetc(file);
        if (c == EOF) {
            lprintf(LOG_VERBOSE,
                    "Reached EOF in file before finding useful data.\n");
            return 0;
        }
    } while (c == 0);
    ungetc(c, file);

    while (1) {
        c = fgetc(file);
        if (c == EOF)
            break;

        if (c & RIM_LEADER_TRAILER)
            continue;

        word = c << 6;

        c = fgetc(file);
        if (c == EOF)
            break;
        word |= c;

        if (word & (RIM_ADDR << 6))
            addr = (word & 07777);
        else {
            if (addr >= 0 && addr < core_size) {
                core[addr] = word & 07777;
                count++;
            } else {
                lprintf(LOG_ERROR,
                        "Address (%.4o) outside allocated core! Failing.\n",
                        addr);
                return -1;
            }

        }
    }

    lprintf(LOG_DEBUG,
            "RIM loader loaded %.4o words.\n",
            count);
    return count;
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
