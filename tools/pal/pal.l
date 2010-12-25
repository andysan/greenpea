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
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pal_y.h"

int lexxer_line = 0;

extern void
yyerror(const char *str)
{
    fprintf(stderr, "Error: %s (%s) at line %i.\n",
            str,
            yytext,
            lexxer_line);
}

%}

%option outfile="pal_l.c"
%option header-file="pal_l.h"
%option noyywrap nounput noinput
/* %option reentrant */
/* %option prefix="pall_" */

ID       [[:alpha:]][[:alnum:]]*

EOL      [\n]
INTEGER  [0-9]+
OP       [+-]

%%

{INTEGER}       { sscanf(yytext, "%o", &yylval.ival); return INTEGER; }
["].           { yylval.ival = yytext[1]; return INTEGER; }
{ID}            { yylval.cval = strdup(yytext); return ID; }
{EOL}           { lexxer_line++; return EOL; }

[/].*
[ \t]+		/* Eat whitespace */

.               { return yytext[0]; }



%%

/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
