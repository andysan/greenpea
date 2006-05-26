
#ifndef _RIM_H
#define _RIM_H

#include <stdio.h>

/* Loads a rim-file.
 *
 * IN:
 * file - Pointer to a file to be read, mustn't be NULL.
 * core - Array of int.
 * core_size - Size of the core array.
 *
 * RET:
 * Returns number of words read, -1 on failure.
 */
int load_rim(FILE* file, int* core, int core_size);


#endif
