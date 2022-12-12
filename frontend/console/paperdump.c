/*
 * SPDX-FileCopyrightText: Copyright 2006-2007, 2010-2011 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <errno.h>
#include <libpdp12utils/rim.h>
#include <libpdp12utils/bin.h>
#include <libpdp12utils/log.h>

const char *argp_program_version =
    "PDP12 paper tape dumper version " VERSION ".\n"
    "Copyright (C) 2006, 2007 Andreas Sandberg <sandberg@update.uu.se>";

const char *argp_program_bug_address = "<sandberg@update.uu.se>";

typedef struct {
    char *tape;
    int offset;
    int mode_rim;
} args;

const struct argp_option options[] = {
    { "debug", 'd', "level", 0, "Modifies the debug output threshold"},
    { "rim", 'r', NULL, 0, "Load a RIM image instead of a BIN image"},
    { "verbose", 'v', NULL, 0, "Produce verbose output"},
    { "offset", 'o', "offset", 0, "Use offset for input"},
    { NULL, 0, NULL, 0, NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    args* a = state->input;

    switch (key) {
    case 'd':
        global_log_level = atoi(arg);
        break;

    case 'r':
        a->mode_rim = 1;
        break;

    case 'v':
        if(global_log_level > LOG_VERBOSE)
            global_log_level = LOG_VERBOSE;
        break;

    case 'o':
        a->offset = atoi(arg);
        break;

    case ARGP_KEY_ARG:
        if(state->arg_num >= 1)
            argp_usage(state);

        a->tape = arg;
        break;

    case ARGP_KEY_END:
        if(state->arg_num < 1)
            argp_usage(state);
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}


const struct argp a_argp = {
    options,
    &parse_opt,
    "[tape]",
    "Dumps a PDP-12 tape image to the screen.",
    NULL,
    NULL,
    NULL
};

void
dump(char* tape, int offset, int mode_rim) {
    FILE *f = fopen(tape, "r");
    int *core = malloc(sizeof(int) * 4096);
    int i;

    memset(core, 0xff, sizeof(int) * 4096);

    if (f == NULL) {
        lprintf(LOG_ERROR, "Failed to input open file: %s\n", strerror(errno));
        exit(1);
    }

    if (mode_rim) {
        if (load_rim(f, offset, core, 4096) == -1) {
            lprintf(LOG_ERROR, "Failed to load RIM-file...\n");
            exit(1);
        }
    } else {
        if (load_bin(f, offset, core, 4096) == -1) {
            lprintf(LOG_ERROR, "Failed to load BIN-file...\n");
            exit(1);
        }
    }

    for (i = 0; i < 4096; i++) {
        if (core[i] > 0 && core[i] < 07777)
            printf("%.4o: %.4o\n", i, core[i]);
    }

    fclose(f);
    free(core);
}

int
main(int argc, char **argp) {
    args a;

    a.mode_rim = 0;
    a.offset = 0;
    a.tape = NULL;

    argp_parse(&a_argp, argc, argp, 0, NULL, &a);

    printf("Log level: %i\n", global_log_level);
    dump(a.tape, a.offset, a.mode_rim);

    return 0;
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
