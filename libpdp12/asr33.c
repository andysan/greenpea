/*
 * $Id: asr33.c 24 2006-06-02 16:24:53Z sandberg $
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <liblog/log.h>
#include "cpu.h"
#include "asr33.h"

#define ASR33_KSF (06031)
#define ASR33_KCC (06032)
#define ASR33_KRS (06034)


#define ASR33_TSF (06041)
#define ASR33_TCF (06042)
#define ASR33_TPC (06044)

#define HAS_INSTR(mn) ((cpu->ir & ASR33_ ## mn) == ASR33_ ## mn)

void asr33_instr(cpu_instance* cpu) {
  asr33* asr33 = cpu->asr33;
  
  if(!asr33)
    return;
  
  /* Skip on Keyboard flag */
  if(HAS_INSTR(KSF)) {
    if(asr33->keyboard_flag)
      cpu_inc_pc(cpu);
  }
  
  /* Clear Keyboard flag */
  if(HAS_INSTR(KCC))
    asr33->keyboard_flag = 0;
  
  /* Read Keyboard Buffer Static */
  if(HAS_INSTR(KRS)) {
    if(asr33->read)
      cpu_set_ac(cpu, (cpu->ac & 07400) | (asr33->read(asr33->data) & 0377));
  }
  
  /* Skip on Teleprinter Flag */
  if(HAS_INSTR(TSF)) {
    if(asr33->printer_flag)
      cpu_inc_pc(cpu);
  }
  
  /* Clear Teleprinter Flag */
  if(HAS_INSTR(TCF))
    asr33->printer_flag = 0;
  
  /* Load Teleprinter and Print */
  if(HAS_INSTR(TPC)) {
    if(asr33->print)
      asr33->print((char)cpu->ac & 0377, asr33->data);
    else
      /* Pretend that the character was actually printed. */
      asr33->printer_flag = 1;
  }
}

void asr33_reset(cpu_instance* cpu) {
  asr33* asr33 = cpu->asr33;
  
  if(!asr33)
    return;
  
  asr33->printer_flag = 0;
  asr33->keyboard_flag = 0;
}
