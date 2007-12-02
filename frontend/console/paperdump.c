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
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <errno.h>
#include <libpdp12utils/rim.h>
#include <libpdp12utils/log.h>

const char *argp_program_version =
"PDP12 paper tape dumper version " VERSION ".\n"
"Copyright (C) 2006, 2007 Andreas Sandberg <sandberg@update.uu.se>";

const char *argp_program_bug_address = "<sandberg@update.uu.se>";

typedef struct {
     char *tape;
     int offset;
} args;

const struct argp_option options[] = {
     { "debug", 'd', "level", 0, "Modifies the debug output threshold"},
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
          log_level = atoi(arg);
          break;

     case 'v':
          if(log_level > LOG_VERBOSE)
               log_level = LOG_VERBOSE;
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
dump(char* tape, int offset) {
     FILE *f = fopen(tape, "r");
     int *core = malloc(sizeof(int) * 4096);
     int i;

     memset(core, 0xff, sizeof(int) * 4096);

     if (f == NULL) {
          lprintf(LOG_ERROR, "Failed to input open file: %s\n", strerror(errno));
          exit(1);
     }

     if (load_rim(f, offset, core, 4096) == -1) {
          lprintf(LOG_ERROR, "Failed to load RIM-file...\n");
          fclose(f);
          free(core);
          exit(1);
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

     a.offset = 0;
     a.tape = NULL;

     argp_parse(&a_argp, argc, argp, 0, NULL, &a);

     printf("Log level: %i\n", log_level);
     dump(a.tape, a.offset);

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
