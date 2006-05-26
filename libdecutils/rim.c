#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <liblog/log.h>
#include "rim.h"

int load_rim(FILE* file, int* core, int core_size) {
  int b1, b2, b3;
  int word;
  unsigned int checksum = 0;
  int count = 0;
  
  int addr = -1;
  
  lprintf(LOG_DEBUG, "Scanning input file...\n");
  while(1) {
    b1 = fgetc(file);
    if(b1 == EOF) {
      lprintf(LOG_VERBOSE, "Reached EOF in file before finding useful data.\n");
      return 0;
    }
    
    
    if(b1 != 0 && (b1 & 0200) == 0) {
      ungetc(b1, file);
      break;
    }
  }
  
  b3 = fgetc(file);
  while(1) {
    b1 = b3;
    b2 = fgetc(file);
    b3 = fgetc(file);
    
    if(b1 == EOF ||
       b2 == EOF) {
      lprintf(LOG_ERROR, "Unexpected EOF when scanning RIM-file.\n");
      return -1;
    }
    
    word = ((b1 << 6) | b2) & 07777;
    
    if(b3 & 0200)
      break;
    
    checksum += (unsigned int) b1 + (unsigned int) b2;
    if(b1 & 0100)
      addr = word;
    else {
      core[addr++] = word;
      count++;
    }
  }
  
  checksum &= 07777;
  if(word != checksum) {
    lprintf(LOG_ERROR, "RIM loader cheksum error in file.\n"
	    "Calculated checksum: %.4o\n"
	    "Expected cheksum: %.4o\n",
	    word, checksum);
    return -1;
  } else {
    lprintf(LOG_DEBUG, "RIM loader loaded %.4o words.\n", count);
    return count;
  }
}
