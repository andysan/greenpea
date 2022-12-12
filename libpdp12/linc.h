/*
 * SPDX-FileCopyrightText: Copyright 2006-2007 Andreas Sandberg <andreas@sandberg.uk>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _LINC_H
#define _LINC_H

/* Instruction classes */
/* #define LINC_CLASS_DIRECT */
#define LINC_CLASS_ALPHA (00000)
#define LINC_CLASS_BETA  (01000)

/* Direct addressing instructions */
#define LINC_OP_ADD (02 >> 1)
#define LINC_OP_STC (04 >> 1)
#define LINC_OP_JMP (06 >> 1)

/* Indirect beta addressing instructions */
#define LINC_OP_LDA (000 >> 2)
#define LINC_OP_STA (004 >> 2)
#define LINC_OP_ADA (010 >> 2)
#define LINC_OP_ADM (014 >> 2)
#define LINC_OP_LAM (020 >> 2)
#define LINC_OP_MUL (024 >> 2)
#define LINC_OP_LDH (030 >> 2)
#define LINC_OP_STH (034 >> 2)
#define LINC_OP_SHD (040 >> 2)
#define LINC_OP_SAE (044 >> 2)
#define LINC_OP_SRO (050 >> 2)
#define LINC_OP_BCL (054 >> 2)
#define LINC_OP_BSE (060 >> 2)
#define LINC_OP_BCO (064 >> 2)
/* Completely reserved for traps. */
#define LINC_OP_TRAP1 (070 >> 2)
#define LINC_OP_DSC (074 >> 2)

/* Misc and indirect alpha addressing instuctions */
#define LINC_OP_EXT1 (000 >> 2)
#define LINC_OP_SET  (004 >> 2)
#define LINC_OP_SAM  (010 >> 2)
#define LINC_OP_DIS  (014 >> 2)
#define LINC_OP_XSK  (020 >> 2)
#define LINC_OP_ROL  (024 >> 2)
#define LINC_OP_ROR  (030 >> 2)
#define LINC_OP_SCR  (034 >> 2)
/* Predefined mnemonics:
   KST: SXL 15
   STD: SXL 16
   TWC: SXL 17
*/
#define LINC_OP_SXL (040 >> 2)
/* Skip instructions */
#define LINC_OP_SKIP (044 >> 2)
/* 0500 is IOB, 501-515 & 521-535 are traps.
   TODO: Check the rest.
*/
#define LINC_OP_EXT2  (050 >> 2)
/* Completely reserved for TRAPS */
#define LINC_OP_TRAP2 (054 >> 2)
#define LINC_OP_LIF  (060 >> 2)
#define LINC_OP_LDF  (064 >> 2)
/* Tape instructions */
#define LINC_OP_TAPE (070 >> 2)
/* According to the manual, 0740-0747 are reserved
   for instruction traps.
   TODO: Check what the other 0740-0770 does.
*/
#define LINC_OP_TRAP3 (074 >> 2)


#define LINC_OPX1_HLT (00000)
#define LINC_OPX1_PDP (00002)
#define LINC_OPX1_ESF (00004)
#define LINC_OPX1_QAC (00005)
#define LINC_OPX1_DJR (00006)
#define LINC_OPX1_CLR (00011)
#define LINC_OPX1_ATR (00014)
#define LINC_OPX1_RTA (00015)
#define LINC_OPX1_NOP (00016)
#define LINC_OPX1_COM (00017)
#define LINC_OPX1_SFA (00024)

#define LINC_OPXS_SNS (00000)
#define LINC_OPXS_AZE (00010)
#define LINC_OPXS_APO (00011)
#define LINC_OPXS_LZE (00012)
#define LINC_OPXS_IBZ (00013)
#define LINC_OPXS_FLO (00014)
#define LINC_OPXS_QLZ (00015)
#define LINC_OPXS_SKP (00016)

#define LINC_OPX2_IOB (00000)
#define LINC_OPX2_RSW (00016)
#define LINC_OPX2_LSW (00017)

#define LINC_ADDR_FRACTION (04000)
#define LINC_ADDR_RIGHT  (04000)
#define LINC_OPERAND_NEGATIVE (04000)

#define LINC_TRAP_SAVE_PC (0140)
#define LINC_TRAP_PC (0141)

#define LINC_DSC_ADDR_X (01)

void linc_do(cpu_instance_t *cpu);
void linc_step(cpu_instance_t *cpu);
void linc_inc_pc(cpu_instance_t *cpu);

int linc_read(cpu_instance_t *cpu, int addr);
void linc_write(cpu_instance_t *cpu, int addr, int data);

#endif
/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
