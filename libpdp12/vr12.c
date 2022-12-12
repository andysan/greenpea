/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <libpdp12utils/log.h>
#include "cpu.h"
#include "vr12.h"


static void
vr12_dsce(int full, vr12_t *vr12, int x, int y, int pattern) {
    int i;
    int spacing = full ? 04 : 02;

    if (!vr12->dis)
        return;

    for(i = 0; i < 12; i++) {
        vr12->dis(i <= 5 ? x + spacing * 2 : x + spacing,
                  y + ((i % 6) - 5) * spacing,
                  2,
                  vr12->data);
    }
}

void
vr12_dis(vr12_t *vr12, int x, int y, int c) {
    if (!vr12 || !vr12->dis)
        return;

    vr12->dis(x, -y + 0377, c, vr12->data);
}

void
vr12_dsc(vr12_t *vr12, int full, int x, int y, int pattern) {
    if (!vr12)
        return;

    if (full) {
        if (vr12->dsc_full)
            vr12->dsc_full(x, y, pattern, vr12->data);
        else
            vr12_dsce(full, vr12, x, y, pattern);
    } else {
        if (vr12->dsc_half)
            vr12->dsc_half(x, y, pattern, vr12->data);
        else
            vr12_dsce(full, vr12, x, y, pattern);
    }
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
