#include "rr.h"
#include "lwp.h"

thread head = NULL;
thread tail = NULL;

void admit(thread new)
{
    if (head == NULL)
    {
        // Set head and tail to new, set next to NULL
        head = new;
        tail = new;
        new->next = NULL;
        new->prev = NULL;
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

void remove(thread victim){
    // perror("A");
    thread current = head;
    // perror("B");
    // Loop until end of list or victim found
    while (current != NULL && current != victim)
    {
        current = current->next;
    }
    //perror("C");
    // If victim found before end of list
    if (current != NULL)
    {
        //perror("D");
        // If victim is head
        if (current == head)
        {
            // If head is not only thread in list, update head
            if (head->next != NULL)
            {
                //perror("E");
                head = head->next;
                head->prev = NULL;
            }
            // If head is only thread in list, set head to NULL
            else
            {
                //perror("F");
                head = NULL;
            }
        }
        // If victim is tail, set the tail to the previous thread
        else if (current == tail)
        {
            //perror("G");
            tail = current->prev;
            tail->next = NULL;
        }
        // If victim is arbitrary thread between head and tail, link the previous and next nodes
        else
        {
            //perror("H");
            // Set next of previous node to next of current node
            current->prev->next = current->next;

            // Set previous of next node to previous of current node
            current->next->prev = current->prev;
        }
    }
}

thread next(){

    thread return_thread = NULL;
    
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
        // // If head is only thread in list
        // else{
            
        //     head = NULL;
        // }
    }
    if (return_thread != NULL)
    {
        // Readmit the previous head to the back of the list
        admit(return_thread);
    }

    return return_thread;
}

int qlen()
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