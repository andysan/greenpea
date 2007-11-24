/*
 * $Id$
 * Copyright (C) 2006 Andreas Sandberg <andreas@sandberg.pp.se>
 *
 * This file is a part of PDP12-emu, a free PDP12 emulator.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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
