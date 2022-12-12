/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _PARSER_H
#define _PARSER_H

typedef struct parser_command {
    /* Defines the command name */
    char *cmd_name;
    /* Number of parameters, -1 disables
       parameter number check. */
    int num_param;
    /* Function to call on command
       invokation. */
    void (*do_cmd)(int argc, char **argp, void *data);
    /* Short help string, shown when
       listing commands. */
    char *help_short;
    /* Detailed help string, shown
       when issuing "help CMD" */
    char *help_long;
} parser_command;

void parser_exec(const parser_command *cmds, void *data, const char *c);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
