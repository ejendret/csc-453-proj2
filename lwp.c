#include "lwp.h"
#include "schedulers.h"

extern tid_t lwp_create(lwpfun fun, void * arg)
{
}
extern void lwp_exit(int status)
{
}
extern tid_t lwp_gettid(void)
{
}
extern void lwp_yield(void)
{
}
extern void lwp_start(void)
{
}
extern tid_t lwp_wait(int * tid)
{
}
extern void lwp_set_scheduler(scheduler fun)
{
}
extern scheduler lwp_get_scheduler(void)
{
}
extern thread tid2thread(tid_t tid)
{
}