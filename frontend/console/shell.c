
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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

void cmd_exit(int argc, char** argp, void* data) {
  ((cmd_data*)data)->end = 1;
}

void cmd_state(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;
  
  printf("IR: %.4o\tMQ: %.4o\n"
	 "PC: %.4o\tAC: %.4o\n"
	 "IF: %.2o\t\tDF: %.2o\n",
	 cpu->ir, cpu->mq,
	 cpu->pc, cpu->ac,
	 cpu->ifr, cpu->dfr);
  
  printf("FLO: %i 8MODE: %i\n",
 	 cpu->flags & CPU_FLAGS_FLO ? 1 : 0,
 	 cpu->flags & CPU_FLAGS_8MODE ? 1 : 0);
	 
}

void cmd_load(int argc, char** argp, void* data) {
  cpu_instance* cpu = ((cmd_data*)data)->cpu;
  int words;
  FILE* f = fopen(argp[1], "r");
  
  if(f == NULL) {
    lprintf(LOG_ERROR, "Failed to input open file: %s\n",
	    strerror(errno));
    return;
  }
  
  words = load_rim(f, cpu->core, CPU_CORE_SIZE);
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

void cmd_step(int argc, char** argp, void* data) {
  cpu_step(((cmd_data*)data)->cpu);
}

void cmd_set(int argc, char** argp, void* data) {
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
    else if(!strcmp(reg, "if"))
      cpu->ifr = value;
    else if(!strcmp(reg, "df"))
      cpu->dfr = value;
    else
      lprintf(LOG_ERROR, "Invalid register specified!\n");
  }
}

void cmd_dump(int argc, char** argp, void* data) {
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

const parser_command cmds[] = {
  {"exit", 0, &cmd_exit, "Exits the emulator", NULL},
  {"state", 0, &cmd_state, "Prints the CPU state.", NULL},
  {"load", 1, &cmd_load, "Loads a paper tape into memory.",
   "load FILENAME\nLoads FILENAME into memory."},
  {"set", 2, &cmd_set, "Sets a processor register or memory address.",
   "set REGISTER VALUE\nSets REGISTER to the value VALUE."},
  {"step", 0, &cmd_step, "Executes the next instruction.", NULL},
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
