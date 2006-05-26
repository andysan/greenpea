#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <argp.h>
#include <errno.h>
#include <libdecutils/rim.h>
#include <liblog/log.h>

const char* argp_program_version =
  "PDP12 paper tape dumper version " VERSION ".\n"
  "Copyright (C) 2006 Andreas Sandberg <andreas@sandberg.pp.se>";

const char* argp_program_bug_address = "<andreas@sandberg.pp.se>";

typedef struct {
  char* tape;
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
    
    a->tape = arg;
    break;
    
  case ARGP_KEY_END:
    if(state->arg_num < 1)
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
  "[tape]",
  "Dumps a PDP-12 tape image to the screen.",
  NULL,
  NULL,
  NULL
};

void dump(char* tape) {
  FILE* f = fopen(tape, "r");
  int* core = malloc(sizeof(int) * 4096);
  int i;
  
  memset(core, 0xff, sizeof(int) * 4096);
  
  if(f == NULL) {
    lprintf(LOG_ERROR, "Failed to input open file: %s\n", strerror(errno));
    exit(1);
  }
  
  if(load_rim(f, core, 4096) == -1) {
    lprintf(LOG_ERROR, "Failed to load RIM-file...\n");
    fclose(f);
    free(core);
    exit(1);
  }
  
  for(i = 0; i < 4096; i++) {
    if(core[i] > 0 && core[i] < 07777)
      printf("%.4o: %.4o\n", i, core[i]);
  }
  
  fclose(f);
  free(core);
}

int main(int argc, char** argp) {
  args a;
  
  a.tape = NULL;
  
  argp_parse(&a_argp, argc, argp, 0, NULL, &a);
  
  printf("Log level: %i\n", log_level);
  dump(a.tape);
  
  return 0;
}
