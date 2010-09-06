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
#include "asr33.h"

#define ASR33_KSF (06031)
#define ASR33_KCC (06032)
#define ASR33_KRS (06034)


#define ASR33_TSF (06041)
#define ASR33_TCF (06042)
#define ASR33_TPC (06044)

#define HAS_INSTR(mn) ((cpu->ir & ASR33_ ## mn) == ASR33_ ## mn)

int
asr33_instr(cpu_instance_t *cpu) {
     asr33_t *asr33 = cpu->asr33;
     int ret = 0;

     if (!asr33)
          return 0;

     /* Skip on Keyboard flag */
     if (HAS_INSTR(KSF)) {
          if (asr33->keyboard_flag)
               cpu_inc_pc(cpu);

          ret = 1;
     }

     /* Clear Keyboard flag */
     if (HAS_INSTR(KCC)) {
          asr33->keyboard_flag = 0;
          ret = 1;
     }
     /* Read Keyboard Buffer Static */
     if (HAS_INSTR(KRS)) {
          if (asr33->read)
               cpu_set_ac(cpu, (cpu->ac & 07400) | (asr33->read(asr33->data) & 0377));

          ret = 1;
     }

     /* Skip on Teleprinter Flag */
     if (HAS_INSTR(TSF)) {
          if (asr33->printer_flag)
               cpu_inc_pc(cpu);

          ret = 1;
     }

     /* Clear Teleprinter Flag */
     if (HAS_INSTR(TCF)) {
          asr33->printer_flag = 0;
          ret = 1;
     }

     /* Load Teleprinter and Print */
     if (HAS_INSTR(TPC)) {
          if (asr33->print)
               asr33->print(cpu->ac & 0377, asr33->data);
          else
               /* Pretend that the character was actually printed. */
               asr33->printer_flag = 1;

          ret = 1;
     }

     return ret;
}

void
asr33_reset(cpu_instance_t *cpu) {
     asr33_t *asr33 = cpu->asr33;

     if (!asr33)
          return;

     asr33->printer_flag = 0;
     asr33->keyboard_flag = 0;
}
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
