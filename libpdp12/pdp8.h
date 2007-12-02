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
#ifndef _PDP8_H
#define _PDP8_H

#define PDP8_OP_AND   (00000)
#define PDP8_OP_TAD   (01000)
#define PDP8_OP_ISZ   (02000)
#define PDP8_OP_DCA   (03000)
#define PDP8_OP_JMS   (04000)
#define PDP8_OP_JMP   (05000)
#define PDP8_OP_IO    (06000)
#define PDP8_OP_OTHER (07000)

#define PDP8_G1_CLA   (00200)
#define PDP8_G1_CLL   (00100)
#define PDP8_G1_CMA   (00040)
#define PDP8_G1_CML   (00020)
#define PDP8_G1_RAR   (00010)
#define PDP8_G1_RAL   (00004)
#define PDP8_G1_ROTX2 (00002)
#define PDP8_G1_IAC   (00001)


#define PDP8_G2       (00400)
#define PDP8_G2_CLA   (00200)
#define PDP8_G2_SMA   (00100)
#define PDP8_G2_SZA   (00040)
#define PDP8_G2_SNL   (00020)
#define PDP8_G2_SENSE (00010)
#define PDP8_G2_OSR   (00004)
#define PDP8_G2_HLT   (00002)
#define PDP8_G2_EAE   (00001)

#define PDP8_EAE_CLA  (00200)
#define PDP8_EAE_MQA  (00100)
#define PDP8_EAE_SCA  (00040)
#define PDP8_EAE_MQL  (00020)

#define PDP8_EAE_SCL  (01 << 1)
#define PDP8_EAE_MUY  (02 << 1)
#define PDP8_EAE_DVI  (03 << 1)
#define PDP8_EAE_NMI  (04 << 1)
#define PDP8_EAE_SHL  (05 << 1)
#define PDP8_EAE_ASR  (06 << 1)
#define PDP8_EAE_LSR  (07 << 1)

void pdp8_do(cpu_instance *cpu);
void pdp8_step(cpu_instance *cpu);
void pdp8_inc_pc(cpu_instance *cpu);

int pdp8_read_d(cpu_instance *cpu, int addr);
void pdp8_write_d(cpu_instance *cpu, int addr, int data);
int pdp8_read_i(cpu_instance *cpu, int addr);
void pdp8_write_i(cpu_instance *cpu, int addr, int data);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
