
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libpdp12/cpu.h>
#include <libdecutils/rim.h>
#include <liblog/log.h>
#include <argp.h>

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

static void start_emulator(args* a) {
  cpu_instance* cpu = cpu_create();
  
  start_shell(cpu);
  
  cpu_destroy(cpu);
}

int main(int argc, char** argp) {
  args a;
  
  a.core = NULL;
  
  argp_parse(&a_argp, argc, argp, 0, NULL, &a);
  start_emulator(&a);
  
  return 0;
}
