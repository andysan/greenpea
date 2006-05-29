#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "readln.h"

#define MAX_CMD_LENGTH (1024)

char* readln_basic(const char* prompt) {
  char* cmd;
  
  if(prompt && *prompt)
    printf(prompt);
  
  /* BUG: Long commands are truncated without telling
     the user. */
  cmd = malloc(MAX_CMD_LENGTH);
  if(!cmd)
    return NULL;
  
  fgets(cmd, MAX_CMD_LENGTH, stdin);
  return cmd;
}
