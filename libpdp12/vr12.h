/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _VR12_H
#define _VR12_H

/*
 * Interface for the VR12 device.
 * The original display had the following properties:
 * Dimensions: 512x512 pixelse
 * Pixel distance: 0.0176" horizontal
 *                 0.0127" vertical
 * Upper left corner: (0, 0377)
 * Lower right corner: (0777, -377)
 *
 *
 * The emulator automatically translates the coordinates
 * so the upper left corner has the coordinate (0,0).
 *
 * The emulator only requires the frontend to implement the
 * dis operation, in that case, one or both, of the dsc operations
 * will be emulated using the dis instruction. Channel number 2
 * will be used incase of emulation.
 *
 * The pattern is as follows (bit 0 i lsb):
 *  5 11
 *  4 10
 *  3 9
 *  2 8
 *  1 7
 *  0 6
 *
 */
typedef struct {
    void (*dis)(int x, int y, int channel, void *data);
    void (*dsc_half)(int x, int y, int pattern, void *data);
    void (*dsc_full)(int x, int y, int pattern, void *data);
    void *data;
} vr12_t;

void vr12_dis(vr12_t *vr12, int x, int y, int c);
void vr12_dsc(vr12_t *vr12, int full, int x, int y, int pattern);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
