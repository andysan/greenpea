
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
  
  printf("PC: %.4o AC: %.4o\n",
	 cpu->pc, cpu->ac);
}

void cmd_load(int argc, char** argp, void* data) {
}

void cmd_step(int argc, char** argp, void* data) {
  cpu_exec(((cmd_data*)data)->cpu);
}

const parser_command cmds[] = {
  {"exit", 0, &cmd_exit, "Exits the emulator", NULL},
  {"state", 0, &cmd_state, "Prints the CPU state.", NULL},
  {"load", 1, &cmd_load, "Loads a paper tape into memory.",
   "load FILENAME\nLoads FILENAME into memory."},
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
