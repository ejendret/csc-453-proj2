#ifndef TLH
#define TLH

#include "lwp.h"

typedef struct ThreadList(
    thread head;
    int size;
) ThreadList;

ThreadList * initThreadList();

thread addThread(ThreadList * list, thread new_thread);


#endif