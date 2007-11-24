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
