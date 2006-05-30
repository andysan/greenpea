
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
