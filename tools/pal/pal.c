/*
 * Copyright (c) 2006-2010 Andreas Sandberg
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
#include <stdio.h>
#include <argp.h>

#include "symbols.h"
#include "pdp8.h"
#include "asl.h"

#define VERSION "0.01"
void setup_pass(FILE *input, int p);
int yyparse();

int verbose = 0;

const char *argp_program_version =
    "PAL III Compatible assembler " VERSION ".\n"
    "Copyright (C) 2006-2010 Andreas Sandberg <andreas@sandberg.pp.se>";

const char *argp_program_bug_address = "<andreas@sandberg.pp.se>";

typedef struct {
    char *input;
    char *output;
} args;

const struct argp_option options[] = {
    { "verbose", 'v', NULL, 0, "Produce verbose output"},
    { "output", 'o', "FILE", 0, "Output file."},
    { NULL, 0, NULL, 0, NULL}
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    args *a = state->input;
  
    switch (key) {
    case 'v':
        verbose = 1;
        break;
    
    case 'o':
        a->output = arg;
        break;
    
    case ARGP_KEY_ARG:
        if(state->arg_num >= 1)
            argp_usage(state);

        a->input = arg;
        break;
    
    case ARGP_KEY_END:
        if(state->arg_num != 1)
            argp_usage(state);
        break;
    
    default:
        return ARGP_ERR_UNKNOWN;
    }
  
    return 0;
}


static const struct argp argp = {
    options,
    &parse_opt,
    "INPUT",
    "Compiles a PDP-8 or PDP-12 program.",
    NULL,
    NULL,
    NULL
};

int
main(int argc, char **argv)
{
    args a;
    int i;
    FILE *input;
    FILE *output;
    int ret;
  
    a.input = NULL;
    a.output = NULL;
  
    argp_parse(&argp, argc, argv, 0, NULL, &a);

    for(i = 0; pdp8_symbols[i].name; i++)
        add_symbol(pdp8_symbols[i].name,
                   pdp8_symbols[i].value);
  
    input = fopen(a.input, "r");
  
    if(!input) {
        fprintf(stderr, "Can't open input file '%s'.\n", a.input);
        return 1;
    }
  
    setup_pass(input, 0);
    if ((ret = yyparse()) == 0) {
        /* Run pass 2 */
        setup_pass(input, 1);
        if((ret = yyparse()) == 0) {
            if(a.output) {
                output = fopen(a.output, "w");
                if(output) {
                    asl_output_rim(output);
                    fclose(output);
                } else
                    fprintf(stderr, "Failed to open output file (%s).\n",
                            a.output);
	
            }
            fseek(input, 0, SEEK_SET);
            asl_output_p3(input, stdout);
        }
    }

    return ret;
}

/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
