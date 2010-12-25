%{
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

#include "symbols.h"
#include "pal.h"
#include "asl.h"

extern int yylex();
extern int yyerror(const char *str);
extern void yyrestart(FILE *input_file);

static int pass = 0;
static int addr = 0;
extern int lexxer_line;

void
setup_pass(FILE* input, int p)
{
    pass = p;
    lexxer_line = 0;
    addr = 0;
    fseek(input, 0, SEEK_SET);
    yyrestart(input);
}

static void
add_label(char* l)
{
    if(verbose)
	printf("Label: %s\n", l);
    add_symbol(l, addr);  
}

static void
emit_instr(int op)
{
    if(pass == 1)
	asl_add_instr(lexxer_line, op);
    addr++;
}

static void
set_base(int a)
{
    addr = a;
    if(pass == 1)
	asl_set_base(lexxer_line, a);
}

%}

%union {
    int ival;
    char* cval;
}

/* %name-prefix="paly_" */
%output="pal_y.c"
%defines

%token <ival>INTEGER
%token <cval>ID
%token EOL

%left '+' '-'

%%

lines: line EOL
       | EOL lines
       | line EOL lines
;

line:	'*' INTEGER {
          set_base($<ival>2);
	}
        | sym_def
	| ID ',' {
	  add_label($<cval>1);
	}
	| ID ',' exprs {
	  add_label($<cval>1);
	  emit_instr($<ival>3);
	}
        | exprs {
	  emit_instr($<ival>1);
	}
;

sym_def: ID '=' sym_def {add_symbol($<cval>1, $<ival>3);$<ival>$ = $<ival>3;};
        | ID '=' exprs  {add_symbol($<cval>1, $<ival>3);$<ival>$ = $<ival>3;};
;

exprs:expr exprs { $<ival>$ = $<ival>1 | $<ival>2; }
      | expr

expr: '(' exprs ')' {
        $<ival>$ = $<ival>2;
      }
      | ID { 
	symbol_t *s = get_symbol($<cval>1);
	if(s)
	  $<ival>$ = s->value;
	else if(pass == 0)
	  $<ival>$ = 0;
	else {
	  yyerror("Unknown symbol");
	  YYERROR;
	}
      }
      | expr '-' integer { $<ival>$ = $<ival>1 - $<ival>3; }
      | expr '+' integer { $<ival>$ = $<ival>1 + $<ival>3; }
      | integer
;

integer: INTEGER { $<ival>$  = $<ival>1; }
/*         | '-' INTEGER { $<ival>$  = -$<ival>1; } */
         | '.' { $<ival>$ = addr; }

%%
