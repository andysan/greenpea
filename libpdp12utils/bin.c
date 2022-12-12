/*
 * SPDX-FileCopyrightText: Copyright 2011 Andreas Sandberg <andreas@sandberg.uk>
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


#define BIN_TYPE_MASK 0xC0
#define BIN_DATA_MASK 0x3F

#define BIN_TYPE_INSTR 0x00
#define BIN_TYPE_ORIGIN 0x40
#define BIN_TYPE_FIELD 0xC0
#define BIN_TYPE_LEAD_TRAIL 0x80

static int
skip_all(FILE *file, int skipmask, int skipchar) {
    int c;

    lprintf(LOG_DEBUG, "Skipping leader/trailer bytes (0x%.2hhx)...\n",
	skipchar);

    while (((c = fgetc(file)) & skipmask) == skipchar)
	;

    if (c == EOF)
	return 0;
    else {
	ungetc(c, file);
	return 1;
    }
}


int
load_bin(FILE *file, int offset, int *core, int core_size) {
    int c0, c1;
    unsigned int checksum = 0;
    int count = 0;

    int addr = -1;

    if (fseek(file, offset, SEEK_SET) == -1) {
        lprintf(LOG_ERROR,
                "Can't set offset 0%.o: %s",
                offset, strerror(errno));
        return -1;
    }

    /* Seems like some of the dumps from Bitsavers are 0x00 padded,
     * skip that padding first and then skip the proper BIN format
     * padding. */
    if (!skip_all(file, 0xFF, 0x00) ||
	!skip_all(file, BIN_TYPE_MASK, BIN_TYPE_LEAD_TRAIL)) {
	lprintf(LOG_VERBOSE,
		"Reached EOF in file before finding useful data.\n");
	return 0;
    }

    while ((c0 = fgetc(file)) != EOF &&
	   (c1 = fgetc(file)) != EOF) {
        int word = ((c0 << 6) | c1) & 07777;
	count++;

	switch (c0 & BIN_TYPE_MASK) {
	case BIN_TYPE_ORIGIN:
	    if ((c1 & BIN_TYPE_MASK) != 0)
		lprintf(LOG_WARNING, "Potential error while decoding origin\n");
	    lprintf(LOG_DEBUG, "origin := %.4o\n", word);
	    addr = word;
	    break;

	case BIN_TYPE_INSTR: {
	    int c2 = fgetc(file);
	    if (c2 != EOF)
		ungetc(c2, file);

	    if ((c1 & BIN_TYPE_MASK) != 0)
		lprintf(LOG_WARNING, "Potential error while decoding instruction\n");

	    /* Now, this is either an instruction or a checksum, it
	     * all depends on the proceeding bytes */

	    /* If we reach EOF without trailer bytes, we assume
	     * it's still a checksum. */
	    if (c2 == EOF ||
		(c2 & BIN_TYPE_MASK) == BIN_TYPE_LEAD_TRAIL) {
		/* Checksum */
		checksum &= 07777;
		if (word != checksum) {
		    lprintf(LOG_ERROR,
			    "BIN loader cheksum error.\n"
			    "Calculated checksum: %.4o\n"
			    "Expected cheksum: %.4o\n",
			    checksum, word);
		    return -1;
		} else {
		    lprintf(LOG_DEBUG,
			    "BIN loader loaded %.4o words.\n",
			    count);
		    return count;
		}
	    } else {
		/* Actual instruction */
		if (addr == -1) {
		    lprintf(LOG_ERROR, "Got instruction, but origin not set.\n");
		    return -1;
		}

		lprintf(LOG_DEBUG, "%.4o: %.4o\n", addr, word);

		if (addr >= 0 && addr < core_size) {
		    core[addr++] = word;
		} else {
		    lprintf(LOG_ERROR,
			    "Address (%.4o) outside allocated core! Failing.\n",
			    addr);
		    return -1;
		}
	    }
	} break;

	case BIN_TYPE_LEAD_TRAIL:
	    lprintf(LOG_ERROR, "Unexpected trailer character.\n");
	    return -1;
	case BIN_TYPE_FIELD:
	    lprintf(LOG_WARNING, "Skipping unhandled field processing directive (%.4o)\n", c0);
            ungetc(c1, file);
	    break;
	}

        switch (c0 & BIN_TYPE_MASK) {
        case BIN_TYPE_ORIGIN:
        case BIN_TYPE_INSTR:
            checksum += (unsigned int)c0 + (unsigned int)c1;
            break;
        default:
            break;
        }
            
    };

    lprintf(LOG_ERROR, "Premature end of file.\n");
    return -1;
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
