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
#include "symbols.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pal.h"

typedef struct symbol_list_s {
    symbol_t sym;
    struct symbol_list_s *next;
} symbol_list_t;

static symbol_list_t *sb = NULL;
static symbol_list_t *se = NULL;

symbol_t *
get_symbol(char *name) {
    symbol_list_t *s = sb;
  
    while(s && strcmp(s->sym.name, name))
        s = s->next;
  
    return &s->sym;
}

void
add_symbol(char *name, int val) {
    symbol_list_t *s;
    symbol_t *sym;
  
    if((sym = get_symbol(name))) {
        free(sym->name);
    } else {
        s = malloc(sizeof(symbol_list_t));
        sym = &s->sym;
    
        s->next = NULL;
    
        if(!sb)
            sb = s;
  
        if(se)
            se->next=s;
  
        se = s;
    }
  
    sym->name = strdup(name);
    sym->value = val;
  
    if(verbose)
        printf("Add symbol: %s = %i\n", name, val);
}

/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
