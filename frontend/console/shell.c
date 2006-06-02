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
#include <errno.h>

#include <libpdp12/cpu.h>
#include <libdecutils/rim.h>
#include <liblog/log.h>

#include "shell.h"
#include "readln.h"
#include "parser.h"

typedef struct {
  cpu_instance* cpu;
  int end;
} cmd_data;

static void cmd_exit(int argc, char** argp, void* data) {
  ((cmd_data*)data)->end = 1;
}

static void cmd_state(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;
  
  printf("IR: %.4o\tMQ: %.4o\n"
	 "PC: %.4o\tAC: %.4o\n LINK: %o"
	 "IF: %.2o\t\tDF: %.2o\n"
	 "FLO: %i 8MODE: %i\n",
	 cpu->ir, cpu->mq,
	 cpu->pc, cpu->ac, cpu->l,
	 cpu->ifr, cpu->dfr,
 	 cpu->flags & CPU_FLAGS_FLO ? 1 : 0,
 	 cpu->flags & CPU_FLAGS_8MODE ? 1 : 0);
	 
}

static void cmd_load(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;
  int words;
  FILE* f;
  int offset = 0;
  
  if(argc <= 1 || argc > 3) {
    lprintf(LOG_ERROR, "Invalid number of parameters for 'load'.\n");
    return;
  }
  
  f = fopen(argp[1], "r");
  if(f == NULL) {
    lprintf(LOG_ERROR, "Failed to input open file: %s\n",
	    strerror(errno));
    return;
  }

  if(argc >= 3)
    offset = atoi(argp[2]);
  
  words = load_rim(f, offset, cpu->core, CPU_CORE_SIZE);
  if(words == -1) {
    lprintf(LOG_ERROR, "Failed to load RIM-file...\n"
	    "Core might be dirty.\n");
    fclose(f);
    return;
  }
  
  lprintf(LOG_NORMAL, "Loaded %i words from '%s'.\n",
	  words, argp[1]);
  
  fclose(f);
}

static void cmd_step(int argc, char** argp, void* data) {
  cpu_step(((cmd_data*)data)->cpu);
}

static void cmd_run(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;

  cpu_set_flag(cpu, CPU_FLAGS_RUN);
  lprintf(LOG_VERBOSE, "Running...\n");
  while(cpu->flags & CPU_FLAGS_RUN)
    cpu_step(((cmd_data*)data)->cpu);
  lprintf(LOG_VERBOSE, "Halted at %.4o.\n", cpu->pc);
}

static void cmd_set(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;
  char* reg = argp[1];
  int value;
  int addr;
  
  if(sscanf(argp[2], "%i", &value) != 1) {
    lprintf(LOG_ERROR, "Invalid value specified!\n");
    return;
  }
  
  if(sscanf(argp[1], "%i", &addr) == 1) {
    if(addr >= 0 && addr < CPU_CORE_SIZE)
      cpu->core[addr] = value;
    else
      lprintf(LOG_ERROR, "Invalid address.\n");
  } else {
    if(!strcmp(reg, "pc"))
      cpu->pc = value;
    else if(!strcmp(reg, "ac"))
      cpu->ac = value;
    else if(!strcmp(reg, "mq"))
      cpu->mq = value;
    else if(!strcmp(reg, "if"))
      cpu->ifr = value;
    else if(!strcmp(reg, "df"))
      cpu->dfr = value;
    else
      lprintf(LOG_ERROR, "Invalid register specified!\n");
  }
}

static void cmd_dump(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;
  int i;
  int start, end;
  
  if(sscanf(argp[1], "%i", &start) != 1 ||
     start >= CPU_CORE_SIZE) {
    lprintf(LOG_ERROR, "Illegal start address received.\n");
    return;
  }

  if(sscanf(argp[2], "%i", &end) != 1 || 
     end >= CPU_CORE_SIZE || end < start) {
    lprintf(LOG_ERROR, "Illegal end address received.\n");
    return;
  }
  
  for(i = start; i <= end; i++) {
    if(i % 010 == 0)
      printf("\n%.4o: ", i);
    
    printf(" %.4o", cpu->core[i]);
  }
  printf("\n");
}

static const parser_command cmds[] = {
  {"exit", 0, &cmd_exit, "Exits the emulator", NULL},
  {"state", 0, &cmd_state, "Prints the CPU state.", NULL},
  {"load", -1, &cmd_load, "Loads a paper tape into memory.",
   "load FILENAME [LOAD OFFSET]\nLoads FILENAME into memory."},
  {"set", 2, &cmd_set, "Sets a processor register or memory address.",
   "set REGISTER VALUE\nSets REGISTER to the value VALUE."},
  {"step", 0, &cmd_step, "Executes the next instruction.", NULL},
  {"run", 0, &cmd_run, "Runs the loaded program from the loaded PC.", NULL},
  {"dump", 2, &cmd_dump, "Dumps the core to the console.", NULL},
  {NULL, 0, NULL, NULL}
};

void start_shell(cpu_instance* cpu) {
  char* c;
  cmd_data cd;
  
  cd.cpu = cpu;
  cd.end = 0;
  
  do {
    /* If readline functionallity is emulated by
       in readln.c and if the "real thing" isn't
       installed. */
    c = readline("PDP12>");
    if(c && *c) {
#ifdef HAVE_READLINE_HISTORY
      add_history(c);
#endif
      parser_exec(cmds, &cd, c);
      free(c);
    }
  } while(!cd.end && c);
  if(!c)
    printf("\n");
  
}
