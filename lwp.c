#include "lwp.h"
#include "rr.h"
#include "schedulers.h"
#define MAX_THREADS 100 // Define a maximum number of threads

static thread terminated_threads[MAX_THREADS];
static thread waiting_threads[MAX_THREADS];
static thread all_threads[MAX_THREADS];
static int terminated_count = 0;
static int waiting_count = 0;
static int all_count = 0;

static tid_t thread_counter = NO_THREAD;
static thread current_thread = NULL;
static void* initial_stack = NULL;

scheduler current_scheduler = NULL;
scheduler RoundRobin = NULL;


tid_t lwp_create(lwpfun fun, void * arg)
{
    if(RoundRobin == NULL)
    {
        struct scheduler publish = {NULL, NULL, admit, remove, next, qlen};
        RoundRobin = &publish;
    }
    // // Check scheduler, should default to rr if null
    // // CHANGE?
    if (current_scheduler == NULL) 
    {
        // struct scheduler publish = {NULL, NULL, admit, remove, next, qlen};
        // current_scheduler = &publish;
        lwp_set_scheduler(NULL);
    }
    // perror("after scheduler");

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
    new_thread->stacksize = howbig/sizeof(unsigned long);
    new_thread->state.rdi = (unsigned long)fun; // arg 1 of lwp_wrap
    new_thread->state.rsi = (unsigned long)arg; // arg2 of lwp_wrap
    new_thread->state.fxsave = FPU_INIT; 
    new_thread->status = LWP_LIVE;

    // Set base and stack pointers what will be top of stack
    new_thread->state.rbp = (unsigned long)&new_thread->stack[new_thread->stacksize - 2];
    new_thread->state.rsp = (unsigned long)&new_thread->stack[new_thread->stacksize - 2];
    
    // Put the pointer to wrap and base pointer on stack
    new_thread->stack[new_thread->stacksize - 1] = (unsigned long)lwp_wrap;
    new_thread->stack[new_thread->stacksize - 2] = new_thread->state.rbp;

    // printf("before admit call\n");
    // Admit the new LWP to the scheduler
    all_threads[((new_thread->tid)-1)] = new_thread;
    all_count++;
    current_scheduler->admit(new_thread);

    // printf("after admit call\n");

    return new_thread->tid;
}

void lwp_start(void)
{
    //perror("start");

    //Initialize new thread struct for LWP
    thread new_thread = (thread)malloc(sizeof(context));
    if (new_thread == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Set the context for thread, already have a stack so new_thread->stack should be NULL
    thread_counter++;
    new_thread->tid = thread_counter;
    new_thread->stack = NULL;
    // new_thread->state = NULL;
    new_thread->status = LWP_LIVE;
    current_thread = new_thread;

    all_threads[((current_thread->tid)-1)] = current_thread;
    all_count++;
    // Admit to scheduler
    current_scheduler->admit(current_thread);
    // swap_rfiles(&current_thread->state, NULL);

    // Yield to next thread
    lwp_yield();
}
// extern tid_t lwp_wait(int * tid)
// {
//     perror("wait");
//     return -1;
// }


void lwp_yield(void)
{
    //perror("yield");
    // Get new thread to run
    thread next_thread = current_scheduler->next();
    //perror("yielded");
    
    // if (current_thread->tid == -1) {
    //     write(1, "-1", 2);
    // } else if (current_thread->tid == 0) {
    //     write(1, "0", 1);
    // } else if (current_thread->tid == 1) {
    //     write(1, "1", 1);
    // } else if (current_thread->tid == 2) {
    //     write(1, "2", 1);
    // } else if (current_thread->tid == 3) {
    //     write(1, "3", 1);
    // } else if (current_thread->tid == 4) {
    //     write(1, "4", 1);
    // } else if (current_thread->tid == 5) {
    //     write(1, "5", 1);
    // } else if (current_thread->tid == 6) {
    //     write(1, "6", 1);
    // } else if (current_thread->tid == 7) {
    //     write(1, "7", 1);
    // } else if (current_thread->tid == 8) {
    //     write(1, "8", 1);
    // }
    // if (next_thread != NULL)
    // {
    //     if (next_thread->tid == -1) {
    //         write(1, "-1", 2);
    //     } else if (next_thread->tid == 0) {
    //         write(1, "0", 1);
    //     } else if (next_thread->tid == 1) {
    //         write(1, "1", 1);
    //     } else if (next_thread->tid == 2) {
    //         write(1, "2", 1);
    //     } else if (next_thread->tid == 3) {
    //         write(1, "3", 1);
    //     } else if (next_thread->tid == 4) {
    //         write(1, "4", 1);
    //     } else if (next_thread->tid == 5) {
    //         write(1, "5", 1);
    //     } else if (next_thread->tid == 6) {
    //         write(1, "6", 1);
    //     } else if (next_thread->tid == 7) {
    //         write(1, "7", 1);
    //     } else if (next_thread->tid == 8) {
    //         write(1, "8", 1);
    //     }
    // }


    // If there is no new thread to run, exit with exit status
    if (next_thread == NULL)
    {
        // perror("done");
        // Get exit status and clean up
        unsigned int exit_status = current_thread->status;
        
        // Don't need to free stack since we didn't allocate for original, may need to free other fields if they require allocated memory that is not freed elsewhere
        free(current_thread);

        // Clean up for scheduler? If defined use shutdown
        if (current_scheduler->shutdown != NULL)
        {
            current_scheduler->shutdown();
        }

        // Exit with retrieved exit status
        exit(exit_status);
    }

    // Context switch
    //perror("swap");

    rfile *temp_thread = &current_thread->state;
    current_thread = next_thread;

    swap_rfiles(temp_thread, &current_thread->state);
    
    //perror("swapped");
}

tid_t lwp_wait(int *status)
{
    //perror("wait");
    // If there are terminated threads, return the oldest one
    if (terminated_count > 0) 
    {
        // There are terminated threads, grab one
        thread terminated_thread = NULL;
        for (int i = 0; i < terminated_count; i++) {
            if (terminated_threads[i] != NULL) {
                terminated_thread = terminated_threads[i];
                terminated_threads[i] = NULL; // Remove the thread from the queue
                break; // Exit the loop after finding the first non-NULL thread
            }
        }

        tid_t term_tid = NO_THREAD; // Declare term_tid outside the if block

        if (terminated_thread != NULL) 
        {
            if (status != NULL) {
                // If a status pointer is provided, set its value
                *status = (terminated_thread->status);
            }

            term_tid = terminated_thread->tid; // Set the value of term_tid

            // // Deallocate resources of the terminated thread
            // free(terminated_thread->stack);
            // free(terminated_thread);

            // Deallocate the stack memory for the exited_thread
            if (terminated_thread->stack != NULL) {
                munmap(terminated_thread->stack, terminated_thread->stacksize * sizeof(unsigned long));
            }
            free(terminated_thread);

            return term_tid; // Return the tid of the terminated thread
        }
    } 
    else
    {
        // No terminated threads, block the current thread by removing it from the scheduler
        current_scheduler->remove(current_thread);

        // Add the current thread to the waiting queue
        waiting_threads[waiting_count] = current_thread;
        waiting_count++;

        // Yield to the next thread
        lwp_yield();

        // Check if the current thread has an associated exited thread after yielding
        if (current_thread->exited != NULL) 
        {
            // The current thread was previously in the waiting queue and has an associated exited thread
            thread exited_thread = current_thread->exited;
            tid_t exited_tid = exited_thread->tid;

            if (status != NULL) {
                // If a status pointer is provided, set its value
                *status = (exited_thread->status);
            }

            // // Remove the current thread from the scheduler
            // current_scheduler->remove(current_thread);

            // Set the current thread's exited field to NULL
            current_thread->exited = NULL;

            // Deallocate the stack memory for the exited_thread
            if (exited_thread->stack != NULL) {
                munmap(exited_thread->stack, exited_thread->stacksize * sizeof(unsigned long));
            }
            free(exited_thread);

            // if (current_thread->stack != NULL) {
            //     munmap(current_thread->stack, current_thread->stacksize * sizeof(unsigned long));
            // }
            // free(current_thread);

            return exited_tid;
        }

        // /10) Main thread is back and resumed where it was in lwp_wait, it checks it's exited field and sees it is not null, it removes that thread from the scheduler and returns its TID so lwp_wait returns.

    }

    return NO_THREAD; // No terminated threads or last runnable thread
}


void lwp_exit(int status) 
{
    //perror("exit");

    // Update the status of the current thread
    current_thread->status = MKTERMSTAT(LWP_TERM, status);

    // Check if there are threads waiting for an exit
    if (waiting_count > 0) 
    {
        thread waiting_thread = NULL;
        // There are waiting threads, grab one
        for (int i = 0; i < waiting_count; i++) {
            if (waiting_threads[i] != NULL) {
                waiting_thread = waiting_threads[i];
                waiting_threads[i] = NULL; // Remove the thread from the queue
                break; // Exit the loop after finding the first non-NULL thread
            }
        }

        if (waiting_thread != NULL) {
            // If a waiting thread exists, associate it with the exiting thread
            waiting_thread->exited = current_thread;

            // Re-admit the waiting thread to the scheduler
            current_scheduler->admit(waiting_thread);
        }
        else
        {
            //Add the current thread to the terminated threads queue
            terminated_threads[terminated_count] = current_thread;
            terminated_count++;
        }
    }

    // Remove the current thread from the scheduler
    current_scheduler->remove(current_thread);

    // Yield to the next thread
    lwp_yield();
}

tid_t lwp_gettid(void)
{
    if (current_thread != NULL)
    {
        return current_thread->tid;
    }
    return NO_THREAD;
}

void lwp_wrap(lwpfun fun, void *arg)
{
    int rval;
    rval = fun(arg);
    lwp_exit(rval);
}

void lwp_set_scheduler(scheduler fun) {
    if (RoundRobin == NULL)
    {
        struct scheduler publish = {NULL, NULL, admit, remove, next, qlen};
        RoundRobin = &publish;
    }
    if (fun == NULL) {
        // Revert to round-robin scheduling
        current_scheduler = RoundRobin;
    } else {
        // Transfer threads from the old scheduler to the new one
        if(current_scheduler != NULL)
        {
            while (current_scheduler->qlen() > 0) {
                thread t = current_scheduler->next();
                fun->admit(t);
            }
        }

        // Set the new scheduler as the current scheduler
        current_scheduler = fun;
    }
}

scheduler lwp_get_scheduler(void)
{
    return current_scheduler;
}

thread tid2thread(tid_t tid) 
{
    for (int i = 0; i < all_count; i++) 
    {
        if ((all_threads[i] != NULL) && (all_threads[i]->tid == tid)) {
            return all_threads[i];  // Found the thread with the matching tid
        }
    }

    // If no thread with the given tid is found, return NULL
    return NO_THREAD;
}


//  git add .
//  git commit -m "with rr2"
//  git checkout -b luc
//  git push origin luc2