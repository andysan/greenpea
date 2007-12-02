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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <libpdp12utils/log.h>
#include "cpu.h"
#include "vr12.h"


static void
vr12_dsce(int full, vr12 *vr12, int x, int y, int pattern) {
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
vr12_dis(vr12 *vr12, int x, int y, int c) {
     if (!vr12 || !vr12->dis)
          return;

     vr12->dis(x, -y + 0377, c, vr12->data);
}

void
vr12_dsc(vr12 *vr12, int full, int x, int y, int pattern) {
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
