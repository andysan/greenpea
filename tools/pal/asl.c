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
#include <stdlib.h>

#include "asl.h"

typedef enum {
    ASL_INSTRUCTION = 0,
    ASL_SET_BASE
} asl_type_t;

typedef struct asl_node_s {
    asl_type_t type;
    int line;

    union {
        int opcode;
        int base_addr;
    };
  
    struct asl_node_s* next;
} asl_node_t;

static asl_node_t *ab = NULL;
static asl_node_t *ae = NULL;

#define RIM_HIGH(n) ((n & 07700) >> 6)
#define RIM_LOW(n) (n & 077)
#define RIM_CHECKSUM(n) (                       \
        (unsigned int)RIM_HIGH(n) +             \
        (unsigned int)RIM_LOW(n)                \
        )

static void
asl_add(asl_node_t *n)
{
    if(!ab)
        ab = n;
    if(ae)
        ae->next = n;

    n->next = NULL;
    ae = n;
}

void
asl_add_instr(int line, int opcode)
{
    asl_node_t *a = malloc(sizeof(asl_node_t));

    a->type = ASL_INSTRUCTION;
    a->line = line;
    a->opcode = opcode;

    asl_add(a);
}

void
asl_set_base(int line, int addr)
{
    asl_node_t* a = malloc(sizeof(asl_node_t));

    a->type = ASL_SET_BASE;
    a->line = line;
    a->base_addr = addr;

    asl_add(a);
}

void
asl_output_p3(FILE *in, FILE *out)
{
    int line = 0;
    asl_node_t* n = ab;
    int addr = 0;
    char *cin = NULL;
    size_t cin_len;

    while(n && getline(&cin, &cin_len, in) != -1) {
        line++;

        if(n->line == line) {
            switch(n->type) {
            case ASL_INSTRUCTION:
                fprintf(out, "%.4o\t%.4o\t%s", addr, n->opcode, cin);
                addr++;
                break;

            case ASL_SET_BASE:
                addr = n->base_addr;
                fprintf(out, "\t\t%s", cin);
                break;

            default:
                fprintf(stderr,
                        "Unknown instruction type (%i) in asl_output_p3.",
                        n->type);
                return;
            }

            n = n->next;
        } else {
            fprintf(out, "\t\t%s", cin);
        }
    }
}

void
asl_output_rim(FILE *out)
{
    asl_node_t *n;
    unsigned int addr = 0;
    unsigned int checksum = 0;

    fprintf(out, "\x80");
  
    for(n = ab; n; n = n->next) {
        switch(n->type) {
        case ASL_INSTRUCTION:
            fprintf(out,
                    "%c%c",
                    RIM_HIGH(n->opcode),
                    RIM_LOW(n->opcode));
            checksum += RIM_CHECKSUM(n->opcode);
            addr++;
            break;
	
        case ASL_SET_BASE:
            addr = n->base_addr;
            fprintf(out,
                    "%c%c",
                    RIM_HIGH(addr) | 0100,
                    RIM_LOW(addr));
            checksum += RIM_CHECKSUM(addr) + 0100;
            break;
      
        default:
            fprintf(stderr,
                    "Unknown instruction type (%i) in asl_output_rim.",
                    n->type);
            return;
        }
    }

    fprintf(out,
            "%c%c",
            RIM_HIGH(checksum),
            RIM_LOW(checksum));
  
    fprintf(out, "\x80");
}

/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
