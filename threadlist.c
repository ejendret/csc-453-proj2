#include "threadlist.h"

typedef struct ThreadList(
    thread head;
    int size;
) ThreadList;

ThreadList * initThreadList() {
    ThreadList * list = (ThreadList*)malloc(sizeof(ThreadList));
    list->head = NULL;
    list->size = 0;
}

void addThread(ThreadList * list, thread newThread){
    // If lsit is empty, set newThread to head
    if (list->head == NULL)
    {
        list->head = newThread;

        // Head should point back to itself
        list->head->next = newThread;
    }
    else {
        thread current = list->head;
        while (current->next != list->head)
        {
            current = current->next;
        }

        // Set next thread to newThread
        current->next = newThread;

        // Set newThread next to list head
        newThread->next = list->head;
    }

    // Increment list size
    list->size++;
}

void remove(ThreadList* list, thread removeThread)
{
    if ()
}