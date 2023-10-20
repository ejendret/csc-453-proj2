#ifndef RRH
#define RRH
#include "lwp.h"

// Declare the RR-specific functions
void init(void);
void shutdown(void);
void admit(thread new);
void remove(thread victim);
thread next(void);
int qlen(void);

// void print_scheduler(void);

#endif
