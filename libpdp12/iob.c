
#include <liblog/log.h>
#include "cpu.h"
#include "iob.h"

void iob_io(cpu_instance* cpu) {
  io_device* dev;
  int i;
  
  if(!cpu->devices)
    return;
  
  for(i = 0; cpu->devices[i]; i++) {
    dev = cpu->devices[i];
    if(dev->io(cpu, dev->data))
      return;
  }
  
  lprintf(LOG_ERROR, "IO-instruction (IR=%.4o PC=%.4o) not handled.\n", cpu->ir, cpu->pc);
  cpu_clear_flag(cpu, CPU_FLAGS_RUN);
}

void iob_reset(cpu_instance* cpu) {
  io_device* dev;
  int i;
  
  if(!cpu->devices)
    return;
  
  for(i = 0; cpu->devices[i]; i++) {
    dev = cpu->devices[i];
    dev->reset(cpu, dev->data);
  }
}
