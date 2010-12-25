/*
 * Copyright (c) 2006-2010 Andreas Sandberg
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
#include "pdp8.h"
#include "symbols.h"

#include "stdio.h"


symbol_t pdp8_symbols[] = {
    /* Pseudo instructions and other junk */
    {"I", 0400},
  
    /* Memory Reference Instructions */
    {"AND", 00000},
    {"TAD", 01000},
    {"ISZ", 02000},
    {"DCA", 03000},
    {"JMS", 04000},
    {"JMP", 05000},
  
    /* Program Interrupt */
    {"ION", 06001},
    {"IOF", 06002},

    /* High-Speed Reader */
    {"RSF", 06011},
    {"RRB", 06012},
    {"RFC", 06014},

  
    /* High-Speed Punch */
    {"PSF", 06021},
    {"PCF", 06022},
    {"PPC", 06024},
    {"PLS", 06026},

    /* Keyboard/Reader */
    {"KSF", 06031},
    {"KCC", 06032},
    {"KRS", 06034},
    {"KRB", 06036},

    /* Group 2 Operators */
    {"HLT", 07402},
    {"OSR", 07404},

    /* Floating-Point Instructions */
    {"FEXT", 00000},
    {"FADD", 01000},
    {"FSUB", 02000},
    {"FMPY", 03000},
    {"FDIV", 04000},
    {"FGET", 05000},
    {"FPUT", 06000},
    {"FNOR", 07000},

    /* Teleprinter/Punch */
    {"TSF", 06041},
    {"TCF", 06042},
    {"TLS", 06046},
    {"TPC", 06044},

    /* Group 1 Operators */
    {"NOP", 07000},
    {"IAC", 07001},
    {"RAL", 07004},
    {"RTL", 07006},
    {"RAR", 07010},
    {"RTR", 07012},
    {"CML", 07020},
    {"CMA", 07040},
    {"CLL", 07100},
    {"CLA", 07200},

    /* Combined Operators */
    {"CIA", 07041},
    {"LAS", 07604},
    {"SKP", 07410},
    {"SNL", 07420},
    {"SZL", 07430},
    {"SZA", 07440},
    {"SNA", 07450},
    {"SMA", 07500},
    {"SPA", 07510},
    {"STL", 07120},
    {"GLK", 07204},
    {"STA", 07240},

    /* DECtape Dual Transport Type 555 */
    {"MMMM", 06757},
    {"MMMF", 06756},
    {"MMML", 06766},
    {"MMLS", 06751},
    {"MMLM", 06752},
    {"MMLF", 06754},
    {"MMSF", 06761},
    {"MMCF", 06772},
    {"MMSC", 06771},
    {"MMRS", 06774},
    {"MMCC", 06762},
    {"MMLC", 06764},

    /* DECtape Transport Type TU55 and Control Type TC01 */
    {"DTRA", 06761},
    {"DTCA", 06762},
    {"DTXA", 06764},
    {"DTSF", 06771},
    {"DTRN", 06772},
    {"DTLB", 06774},

    /* Memory Parity Type 188 */
    {"SMP", 06101},
    {"CMP", 06104},
  
  
    /* ****** EXTENDED SYMBOLS ******* */

    /* PDP-5 EAE Symbols 153* */
  
    /* PDP-8 EAE Symbols 182 */

    /* Memory Extension Control Type 183 */

    /* Auto Restart Type KR01 */

    /* AD Converter Type 189 */

    /* AD Converter/Multiplexer 138E/139E */

    /* Oscilloscope Display Type 34D */

    /* Scope Type 30N */

    /* Light Pen Type 370 */

    /* Plotter and Control Type 350B */

    /* Card Reader and Control Type CR01C */

    /* Card Reader Type 451 */

    /* Card Punch and Control Type 450 */

    /* Line Printer Type 645 */

    /* Serial Drum 250 and 251 */

       /* Magnetic Tape Type 57A */

    /* Magnetic Tape Type 580 */


    /* ****** 8-CHANNEL SAMPLE AND HOLD TYPE AC01A ******* */
    /* Option to Type 139E Multiplexor */

    /* Data Communication System Type 630 */

  
    {NULL, 0}
};

/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
