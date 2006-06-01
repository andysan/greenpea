#ifndef _TELETYPE_H
#define _TELETYPE_H

#define TELETYPE_KSF (06031)
#define TELETYPE_KCC (06032)
#define TELETYPE_KRS (06034)


#define TELETYPE_TSF (06041)
#define TELETYPE_TCF (06042)
#define TELETYPE_TPC (06044)

typedef enum {
  TELETYPE_FLAGS_KEYBOARD = 1,
  TELETYPE_FLAGS_PRINTER = 2
} teletype_flags;

typedef struct {
  int flags;
} teletype_state;

io_device* teletype_create();

#endif
