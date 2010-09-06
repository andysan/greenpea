/*
 * Copyright (c) 2006, 2007 Andreas Sandberg
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
