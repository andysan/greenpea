/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _ASR33_H
#define _ASR33_H

/*
 * Interface for the ASR33 device.
 */
typedef struct {
    /* Called when the instruction to print a character on the ASR33 is called.
     * Is responsible for setting the printer_flag when printing is done.
     */
    void (*print)(unsigned char c, void *data);
    char (*read)(void *data);

    int keyboard_flag : 1;
    int printer_flag : 1;

    void* data;
} asr33_t;

int asr33_instr(cpu_instance_t *cpu);
void asr33_reset(cpu_instance_t *cpu);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
