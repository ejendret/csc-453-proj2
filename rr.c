#include "lwp.h"

thread head = NULL;
thread tail = NULL;

void rr_admit(thread new)
{
    if (head == NULL)
    {
        // Set head and tail to new, set next to NULL
        head = new;
        tail = new;
        new.next = NULL;
        new.prev = NULL;
    }
    else{
        // Set previous tail next to new and then update tail to new
        tail->next = new;
        thread temp = tail;
        tail = new;
        tail->prev = temp;
        tail->next = NULL;
    }
}

void rr_remove(thread victim){
    thread current = head;

    // Loop until end of list or victim found
    while (current != NULL && current != victim)
    {
        current = current->next;
    }

    // If victim found before end of list
    if (current != NULL)
    {
        // If victim is head
        if (current == head)
        {
            // If head is not only thread in list, update head
            if (head->next != NULL)
            {
                head = head->next;
                head->prev = NULL;
            }
            // If head is only thread in list, set head to NULL
            else
            {
                head = NULL;
            }
        }
        // If victim is tail, set the tail to the previous thread
        else if (current == tail)
        {
            tail = curent->prev;
            tail->next = NULL;
        }
        // If victim is arbitrary thread between head and tail, link the previous and next nodes
        else
        {
            // Set next of previous node to next of current node
            current->prev->next = current->next;

            // Set previous of next node to previous of current node
            current->next->prev = current->prev;
        }
    }
}

thread rr_next(){
    return_thread = NULL;
    
    // If list isn't empty
    if (head != NULL)
    {
        // Set return thread to head
        return_thread = head;

        // If head isn't only thread in list
        if (head->next != NULL)
        {
            head = head->next;
            head->prev = NULL;
        }
        // If head is only thread in list
        else{
            
            head = NULL;
        }
    }

    // Readmit the previous head to the back of the list
    rr_admit(return_thread);

    return return_thread;
}

int rr_qlen()
{
    int count = 0;
    thread current = head;
    while (current != NULL)
    {
        current = current->next;
        count++;
    }

    return count;
}
