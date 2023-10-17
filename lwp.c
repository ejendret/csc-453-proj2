#include "lwp.h"
#include "schedulers.h"

scheduler current_scheduler  = NULL;
tid_t thread_counter = NO_THREAD;
tid_t current_thread = NO_THREAD;
void* initial_stack = NULL;

// 8MB stack size, assuming MB is 1024^2
#define DEFAULT_STACK_SIZE 8388608

/*
Creates a new thread and admits it to the current scheduler. The thread’s resources will consist of a
context and stack, both initialized so that when the scheduler chooses this thread and its context is
loaded via swap_rfiles() it will run the given function. This may be called by any thread.
*/
extern tid_t lwp_create(lwpfun fun, void * arg)
{
    // Check scheduler, should default to rr if null
    // CHANGE?
    if (current_scheduler == NULL) 
    {
        return NO_THREAD;
    }

    //Initialize new thread struct for LWP
    thread new_thread = (thread)malloc(sizeof(context));
    if (new_thread == NULL) 
    {
        perror("malloc");
        return NO_THREAD;
    }

    //get memory page size 
    long mempagesize = sysconf(_SC_PAGE_SIZE);
    if (mempagesize == -1) 
    {
        perror("sysconf");
        // CHANGE?
        free(new_thread);
        return NO_THREAD;
    }

    // Determine stacksize
    size_t howbig = DEFAULT_STACK_SIZE; // Default stack size
   
    // If rlimit for stack size exists and is not infinity, use instead of default
    struct rlimit stacksize_rl;
    if (getrlimit(RLIMIT_STACK, &stacksize_rl) == 0) 
    {
        if (stacksize_rl.rlim_cur != RLIM_INFINITY)
        {
            howbig = stacksize_rl.rlim_cur;
        }
    }

    // If the resource limit is not a multiple of the page size
    if (mempagesize % howbig != 0)
    {
        // Round up to the nearest multiple of the page size
        howbig = ((howbig + mempagesize - 1) / mempagesize) * mempagesize;
    }

    // Allocate and initialize the stack for the new LWP using mmap
    // mmap(2) returns a pointer to the memory region on success or MAP_FAILED on failure.
    new_thread->stack = mmap(NULL, howbig, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (new_thread->stack == MAP_FAILED) {
        perror("mmap");
        free(new_thread);
        return NO_THREAD;
    }

    // Setup stack and thread context
    thread_counter++;
    new_thread->tid = thread_counter;
    new_thread->stacksize = howbig;
    new_thread->state.rdi = (unsigned long)fun; // arg 1 of lwp_wrap
    new_thread->state.rsi = (unsigned long)arg; // arg2 of lwp_wrap
    new_thread->state.fxsave = FPU_INIT; 
    new_thread->status = LWP_LIVE;

    // Get stack bottom, set the base pointer to this, this is our "old" base pointer
    unsigned long * stack_bottom = new_thread->stack + new_thread->stacksize;
    new_thread->state.rbp = (unsigned long)stack_bottom;

    // Put the pointer to wrap and base pointer on stack
    *(stack_bottom - 8) = (unsigned long)lwp_wrap;
    *(stack_bottom - 16) = new_thread->state.rbp;

    // Admit the new LWP to the scheduler
    current_scheduler->admit(new_thread);

    return new_thread->tid;
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
    //Initialize new thread struct for LWP
    thread new_thread = (thread)malloc(sizeof(context));
    if (new_thread == NULL) 
    {
        perror("malloc");
        return NO_THREAD;
    }

    // Set the context for thread, already have a stack so new_thread->stack should be NULL
    thread_counter++;
    new_thread->tid = thread_counter;
    new_thread->stack = NULL;
    new_thread->status = LWP_LIVE;

    // Admit to scheduler
    current_scheduler.admit(new_thread);

    // Get next thread to run
    thread* next = current_scheduler.next();
    
    // Yield to thread
    lwp_yield(next);
}
extern tid_t lwp_wait(int * tid)
{
}
void lwp_wrap(lwpfun fun, void *arg)
{
    int rval;
    rval = fun(arg);
    lwp_exit(rval);
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