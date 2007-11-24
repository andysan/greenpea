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

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <libpdp12utils/log.h>
#include "parser.h"

typedef enum {
     PARSER_NORMAL = 0,
     PARSER_QUOTE = 1,
     PARSER_ESCAPE = 2
} parser_mode;

typedef struct token_list_s {
     const char *t;
     int l;
     struct token_list_s *n;
} token_list;

/*
 * Function that parses a text string and returns a NULL separated
 * string with the translated result.
 */
static char *
parse(const char* text) {
     int len = strlen(text);
     char* out = malloc(len+1);
     int i;

     parser_mode mode = PARSER_NORMAL;

     out[len] = '\0';

     for (i = 0; i < len; i++) {
          if (mode & PARSER_ESCAPE) {
               if (text[i] == ' ' ||
                  text[i] == '\"') {
                    out[i] = text[i];
               } else {
                    lprintf(LOG_ERROR,
                            "Unknown escape code received in parser.\n");
               }
          } else {
               if (text[i] == '\\')
                    mode |= PARSER_ESCAPE;
               else if (text[i] == '\"')
                    mode ^= PARSER_QUOTE;
               else if (!(mode & PARSER_QUOTE) &&
                       isspace(text[i])) {
                    out[i] = '\0';
               } else {
                    out[i] = text[i];
               }
          }
     }

     return out;
}

/*
 * Function that parses and splits text and creates an
 * array with with string pointers in *out. Returns
 * the number of tokens.
 *
 * The caller is responsible for freeing up the memory
 * in (*out)[n].
 */
static int
split(const char *text, char ***out) {
     int i;
     int num_tokens = 0;
     token_list* tokens = NULL;
     token_list* t = NULL;
     char* parsed = parse(text);
     int len = strlen(text);
     int t_len;

     for (i = 0; i < len; i++) {
          t_len = strlen(&parsed[i]);
          if (t_len == 0)
               continue;

          if (t == NULL) {
               tokens = malloc(sizeof(token_list));
               t = tokens;
          } else {
               t->n = malloc(sizeof(token_list));
               t = t->n;
          }

          t->n = NULL;
          t->l = t_len;
          t->t = &parsed[i];
          i += t->l;
          num_tokens++;
     }

     *out = malloc(sizeof(char*) * num_tokens);
     t = tokens;
     for (i = 0; t; i++) {
          (*out)[i] = malloc(t->l + 1);
          memcpy((*out)[i], t->t, t->l + 1);
          t = t->n;
     }

     free(parsed);

     return num_tokens;
}

static void
free_args(int argc, char **argp) {
     int i;
     for (i = 0; i < argc; i++)
          free(argp[i]);

     free(argp);
}

static void
print_help(const parser_command *cmds, const char *cmd) {
     int i;
     if (!cmd) {
          for (i = 0; cmds[i].cmd_name; i++)
               lprintf(LOG_NORMAL, "%s\t\t%s\n",
                       cmds[i].cmd_name, cmds[i].help_short);
     } else {
          for (i = 0; cmds[i].cmd_name; i++) {
               if (!strcmp(cmds[i].cmd_name, cmd)) {
                    if (cmds[i].help_long)
                         lprintf(LOG_NORMAL, "%s\n", cmds[i].help_long);
                    else if (cmds[i].help_short)
                         lprintf(LOG_NORMAL, "%s\n%s\n",
                                 cmds[i].cmd_name, cmds[i].help_short);
                    else
                         lprintf(LOG_NORMAL,
                                 "No help availible for '%s'.\n", cmd);

                    return;
               }
          }
          lprintf(LOG_ERROR, "Unknown command '%s'.\n", cmd);
     }
}

void
parser_exec(const parser_command *cmds, void *data, const char *c) {
     int argc;
     char **argp;
     int i;

     argc = split(c, &argp);
     if(argc == 0)
          return;

     if(!strcmp(argp[0], "help")) {
          if (argc >= 2)
               print_help(cmds, argp[1]);
          else
               print_help(cmds, NULL);

          free_args(argc, argp);
          return;
     }

     for (i = 0; cmds[i].cmd_name; i++) {
          if (!strcmp(cmds[i].cmd_name, argp[0])) {
               if (cmds[i].num_param == -1 ||
                  cmds[i].num_param == argc - 1)
                    cmds[i].do_cmd(argc, argp, data);
               else
                    lprintf(LOG_ERROR,
                            "Invalid number of parameters for '%s'\n",
                            cmds[i].cmd_name);

               free_args(argc, argp);
               return;
          }
     }

     lprintf(LOG_ERROR, "Unknown command '%s'.\n", argp[0]);

     free_args(argc, argp);
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
