
#ifndef _IOB_H
#define _IOB_H

typedef struct io_device_s{
  /* Data to be passed to all IO instructions */
  void* data;

  /* Called on IO, returns 0 if the IO-request
     was not handled. */
  int (*io)(struct cpu_instance_s* cpu, void* data);
  /* Called on an IO-reset. */
  void (*reset)(struct cpu_instance_s* cpu, void* data);
  /* Called to deallocate data and close the device. */
  void (*destroy)(void* data);
} io_device;

void iob_io(struct cpu_instance_s* cpu);
void iob_reset(struct cpu_instance_s* cpu);

#endif
