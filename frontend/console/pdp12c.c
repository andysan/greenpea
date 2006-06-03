/*
 * $Id$
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
#include <signal.h>
#include <libpdp12/cpu.h>
#include <libpdp12/iob.h>
#include <libdecutils/rim.h>
#include <liblog/log.h>
#include <argp.h>

#include "shell.h"

static cpu_instance* cpu = NULL;

const char* argp_program_version =
  "PDP12 simulator version " VERSION ".\n"
  "Copyright (C) 2006 Andreas Sandberg <andreas@sandberg.pp.se>";

const char* argp_program_bug_address = "<andreas@sandberg.pp.se>";

typedef struct {
  char* core;
} args;

const struct argp_option options[] = {
  { "debug", 'd', "level", 0, "Modifies the debug output threshold"},
  { "verbose", 'v', NULL, 0, "Produce verbose output"},
  { NULL, 0, NULL, 0, NULL}
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  args* a = state->input;
  
  switch (key) {
  case 'd':
    log_level = atoi(arg);
    break;
    
  case 'v':
    if(log_level > LOG_VERBOSE)
      log_level = LOG_VERBOSE;
    break;
    
  case ARGP_KEY_ARG:
    if(state->arg_num >= 1)
      argp_usage(state);

    a->core = arg;
    break;
    
  case ARGP_KEY_END:
    if(state->arg_num > 1)
      argp_usage(state);
    break;
    
  default:
    return ARGP_ERR_UNKNOWN;
  }
  
  return 0;
}


static const struct argp a_argp = {
  options,
  &parse_opt,
  "[core]",
  "Starts a PDP-12 simulation on the console.",
  NULL,
  NULL,
  NULL
};

static void sig_int(int signum) {
  if(cpu && cpu->flags & CPU_FLAGS_RUN) {
    cpu_clear_flag(cpu, CPU_FLAGS_RUN);
    lprintf(LOG_NORMAL, "User requested HALT.\n");
  }
}

static void asr33_print(char c, void* data) {
  lprintf(LOG_NORMAL, "asr33_print: %c (0%o)\n", c, c);
  cpu->asr33->printer_flag = 0;
}

void vr12_dis(int x, int y, int channel, void* data) {
  lprintf(LOG_NORMAL, "vr12_dis: (%i, %i) C: %i\n", x, y, channel);
}

static void start_emulator(args* a) {
  io_device* devices[] = { NULL };
  asr33 asr33;
  vr12 vr12;
  /* int i; */
  
  asr33.print = &asr33_print;
  asr33.read = NULL;
  asr33.keyboard_flag = 0;
  asr33.printer_flag = 0;
  asr33.data = NULL;
  
  vr12.dis = &vr12_dis;
  vr12.dsc_half = NULL;
  vr12.dsc_full = NULL;
  vr12.data = NULL;
  
  signal(SIGINT, &sig_int);
  
  cpu = cpu_create();
  cpu->devices = devices;
  cpu->asr33 = &asr33;
  cpu->vr12 = &vr12;
  
  start_shell(cpu);
  
  /*
  for(i = 0; devices[i]; i++)
    devices[i]->destroy(devices[i]->data);
  */  
  
  cpu_destroy(cpu);
}

int main(int argc, char** argp) {
  args a;
  
  a.core = NULL;
  
  argp_parse(&a_argp, argc, argp, 0, NULL, &a);
  start_emulator(&a);
  
  return 0;
}
