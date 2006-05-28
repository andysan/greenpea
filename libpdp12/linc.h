
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
/*  #define LINC_OP_    (070 >> 2) */
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
#define LINC_OP_EXT2 (040 >> 2)
#define LINC_OP_EXT3 (044 >> 2)
#define LINC_OP_EXT4 (050 >> 2)
/* #define LINC_OP_    (054 >> 2) */
#define LINC_OP_LIF  (060 >> 2)
#define LINC_OP_LDF  (064 >> 2)
#define LINC_OP_EXT5 (070 >> 2)
/* #define LINC_OP_    (074 >> 2) */


#define LINC_OPX1_HLT (00000)
#define LINC_OPX1_ESF (00004)
#define LINC_OPX1_QAC (00005)
#define LINC_OPX1_DJR (00006)
#define LINC_OPX1_CLR (00011)
#define LINC_OPX1_ATR (00014)
#define LINC_OPX1_RTA (00015)
#define LINC_OPX1_NOP (00016)
#define LINC_OPX1_COM (00017)
#define LINC_OPX1_SFA (00024)

void linc_step(cpu_instance* cpu);
void linc_inc_pc(cpu_instance* cpu);
int linc_read(cpu_instance* cpu, int addr);
void linc_write(cpu_instance* cpu, int addr, int data);

#endif
