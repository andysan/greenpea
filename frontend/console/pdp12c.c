
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libpdp12/cpu.h>
#include <libdecutils/rim.h>
#include <liblog/log.h>
#include <argp.h>

#include "pdp12c.h"

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


const struct argp a_argp = {
  options,
  &parse_opt,
  "[core]",
  "Starts a PDP-12 simulation on the console.",
  NULL,
  NULL,
  NULL
};

void simulate(char* core) {
  cpu_instance* cpu = cpu_create();
  char* cmd;
  int end = 0;
  
  /* cpu->core[0] = 00516;
  cpu->core[1] = 00014;
  cpu->core[2] = 06000; */
  
  do {
#ifdef HAVE_LIBREADLINE
    cmd = readline("PDP12>");
#  ifdef HAVE_READLINE_HISTORY
    if(cmd && *cmd)
      add_history(cmd);
#  endif
#else
    /* BUG: Long commands are truncated without telling
       the user. */
    cmd = malloc(MAX_CMD_LENGTH);
    if(cmd == NULL) {
      lprintf(LOG_ERROR, "Failed to allocate command buffer!\n");
      exit(1);
    }
    fgets(cmd, MAX_CMD_LENGTH, stdin);
#endif

    if(!cmd) {
      printf("\n");
      end = 1;
    } else if(!strcmp(cmd, "exit")) {
      end = 1;
    } else if(!strcmp(cmd, "help")) {
      printf("Help, no help!!!!\n");
    } else if(!strcmp(cmd, "load")) {
      
    } else if(!strcmp(cmd, "state")) {
      printf("PC: %.4o AC: %.4o\n",
	     cpu->pc, cpu->ac);
    } else if(!*cmd || !strcmp(cmd, "step")) {
      cpu_exec(cpu);
    } else {
      printf("Unknown command\n");
    }
    
    if(!cmd || !*cmd)
      free(cmd);
    
  } while(!end);
  
  cpu_destroy(cpu);
}

int main(int argc, char** argp) {
  args a;

  a.core = NULL;
  
  argp_parse(&a_argp, argc, argp, 0, NULL, &a);
  simulate(a.core);
  
  return 0;
}
