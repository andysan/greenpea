/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "readln.h"

#define MAX_CMD_LENGTH (1024)

char *readln_basic(const char *prompt) {
    char *cmd;

    if (prompt && *prompt)
        printf("%s", prompt);

    /*
     * BUG: Long commands are truncated without telling the user.
     */
    cmd = malloc(MAX_CMD_LENGTH);
    if (!cmd)
        return NULL;

    fgets(cmd, MAX_CMD_LENGTH, stdin);
    return cmd;
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
