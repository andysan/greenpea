
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <libpdp12/cpu.h>
#include <libpdp12/iob.h>
#include <liblog/log.h>

#include "teletype.h"

#define HAS_INSTR(mn) ((cpu->ir & TELETYPE_ ## mn) == TELETYPE_ ## mn)
#define CLEAR_FLAG(f) ts->flags = ts->flags & ~TELETYPE_FLAGS_ ## f;
#define HAS_FLAG(f) (ts->flags & TELETYPE_FLAGS_ ## f)
#define SET_FLAG(f) ts->flags |= TELETYPE_FLAGS_ ## f;

static int teletype_io(cpu_instance* cpu, void* data) {
  teletype_state* ts = (teletype_state*)data;
  int ret = 0;
  
  lprintf(LOG_VERBOSE, "teletype_io: %.4o\n", cpu->ir);
  
  /* Skip on Keyboard flag */
  if(HAS_INSTR(KSF)) {
    if(HAS_FLAG(KEYBOARD))
      cpu_inc_pc(cpu);
    
    ret = 1;
  }
  
  /* Clear Keyboard flag */
  if(HAS_INSTR(KCC)) {
    CLEAR_FLAG(KEYBOARD);
    ret = 1;
  }
  
  /* Read Keyboard Buffer Static */
  if(HAS_INSTR(KRS)) {
    /* TODO: Implement KRS. */
    lprintf(LOG_ERROR, "KRS not implemented and shouldn't happen, flag not set!\n");
    ret = 1;
  }
  
  /* Skip on Teleprinter Flag */
  if(HAS_INSTR(TSF)) {
    if(HAS_FLAG(PRINTER))
       cpu_inc_pc(cpu);
    ret = 1;
  }
  
  /* Clear Teleprinter Flag */
  if(HAS_INSTR(TCF)) {
    CLEAR_FLAG(PRINTER);
    ret = 1;
  }
  
  /* Load Teleprinter and Print */
  if(HAS_INSTR(TPC)) {
    lprintf(LOG_NORMAL, "0%o", cpu->ac & 0377);
    SET_FLAG(PRINTER);
    ret = 1;
  }
  
  return ret;
}

static void teletype_reset(cpu_instance* cpu, void* data) {
  teletype_state* ts = (teletype_state*)data;
  
  ts->flags = 0;
}

static void teletype_destroy(void* data) {
  teletype_state* ts = (teletype_state*)data;
  
  free(ts);
}

io_device* teletype_create() {
  io_device* dev;
  
  dev = malloc(sizeof(io_device));
  dev->data = malloc(sizeof(teletype_state));
  
  dev->io = &teletype_io;
  dev->reset = &teletype_reset;
  dev->destroy = &teletype_destroy;
  
  teletype_reset(NULL, dev->data);
  
  return dev;
}
