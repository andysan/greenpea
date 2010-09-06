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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <libpdp12utils/log.h>
#include "rim.h"

int
load_rim(FILE *file, int offset, int *core, int core_size) {
    int b1, b2, b3;
    int word;
    unsigned int checksum = 0;
    int count = 0;

    int addr = -1;

    if (fseek(file, offset, SEEK_SET) == -1) {
        lprintf(LOG_ERROR,
                "Can't set offset 0%.o: %s",
                offset, strerror(errno));
        return -1;
    }

    lprintf(LOG_DEBUG, "Scanning input file...\n");
    while (1) {
        b1 = fgetc(file);
        if (b1 == EOF) {
            lprintf(LOG_VERBOSE,
                    "Reached EOF in file before finding useful data.\n");
            return 0;
        }


        if (b1 != 0 && (b1 & 0200) == 0) {
            ungetc(b1, file);
            break;
        }
    }

    b3 = fgetc(file);
    while (1) {
        b1 = b3;
        b2 = fgetc(file);
        b3 = fgetc(file);

        if (b1 == EOF ||
            b2 == EOF) {
            lprintf(LOG_ERROR,
                    "Unexpected EOF when scanning RIM-file.\n");
            return -1;
        }

        word = ((b1 << 6) | b2) & 07777;

        if (b3 & 0200)
            break;

        checksum += (unsigned int) b1 + (unsigned int) b2;
        if (b1 & 0100)
            addr = word;
        else {
            if (addr >= 0 && addr < core_size) {
                core[addr++] = word;
                count++;
            } else {
                lprintf(LOG_ERROR,
                        "Address (%.4o) outside allocated core! Failing.\n",
                        addr);
            }

        }
    }

    checksum &= 07777;
    if (word != checksum) {
        lprintf(LOG_ERROR,
                "RIM loader cheksum error in file.\n"
                "Calculated checksum: %.4o\n"
                "Expected cheksum: %.4o\n",
                word, checksum);
        return -1;
    } else {
        lprintf(LOG_DEBUG,
                "RIM loader loaded %.4o words.\n",
                count);
        return count;
    }
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
