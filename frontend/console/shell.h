/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _SHELL_H
#define _SHELL_H

#define MAX_SCRIPT_LINE_LENGTH (1024)

void shell_script(cpu_instance_t *cpu, const char *name);
void shell_start(cpu_instance_t *cpu);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
