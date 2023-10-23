#include "rr.h"
#include "lwp.h"

// Define a structure for a node in the linked list
typedef struct Node {
    thread data;        // Thread data
    struct Node* next;  // Pointer to the next node
} Node;

// Define the linked list structure
typedef struct LinkedList {
    Node* head; // Pointer to the first node
    Node* tail; // Pointer to the last node
} LinkedList;

// Initialize the linked list
void initLinkedList(LinkedList* list) {
    perror("in init ll");
    list->head = NULL;
    list->tail = NULL;
}

// Global variable to hold the current RR scheduler
// static scheduler current_scheduler = NULL;
static LinkedList list;
// static LinkedList *origin;

// // Initialize the RR scheduler
void init(void) {
    // if (current_scheduler == NULL) 
    // {
    //     current_scheduler = (scheduler)malloc(sizeof(struct scheduler));
    //     current_scheduler->init = init;
    //     current_scheduler->shutdown = shutdown;
    //     current_scheduler->admit = admit;
    //     current_scheduler->rem = rem;
    //     current_scheduler->next = next;
    //     current_scheduler->qlen = qlen;
    // }
    perror("in init");
    if (list.head == NULL) 
    {
        initLinkedList(&list);
    }
}

// Tear down the RR scheduler and free resources
void shutdown(void) {
    while (list.head) {
        Node* next_node = list.head->next;
        free(list.head);
        list.head = next_node;
    }
}

// Add a thread to the round-robin queue
void admit(thread new) {
    // printf("in admit\n");
    if (new == NULL) {
        return; // Do not admit NULL threads
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = new;
    newNode->next = NULL;
    // printf("after malloc\n");

    if (list.head == NULL) {
        // printf("in if\n");
        list.head = newNode;
        list.tail = newNode;
        if(list.head->data->tid == 1)
        {
            perror("head == 1");
        }
        // origin = &list;
    } else {
        // printf("in else\n");
        list.tail->next = newNode;
        list.tail = newNode;
    }
}

// Remove a thread from the round-robin queue
void remove(thread victim) {
    if(victim->tid == 1)
    {
        perror("victim == 1");
    }
        else if(victim->tid == 2)
    {
        perror("victim == 2");
    }
        if(victim->tid == 3)
    {
        perror("victim == 3");
    }
        else if(victim->tid == 4)
    {
        perror("victim == 4");
    }
        if(victim->tid == 5)
    {
        perror("victim == 5");
    }
        else if(victim->tid == 6)
    {
        perror("victim == 6");
    }
    else
    {
        perror("victim == ?");
    }
    perror("remove");
    if (victim == NULL || list.head == NULL) {
        return; // Do not remove NULL threads or from an empty queue
    }

    if (list.head->data->tid == victim->tid) {
        Node* next_node = list.head->next;
        if (list.head == list.tail) {
            list.tail = next_node;
        }
        free(list.head);
        list.head = next_node;
        if (list.head == NULL) {
            list.tail = NULL;
        }
    } else {
        Node* prev = list.head;
        while (prev->next != NULL && prev->next->data->tid != victim->tid) {
            prev = prev->next;
        }
        if (prev->next != NULL) {
            Node* removed = prev->next;
            prev->next = removed->next;
            if (removed == list.tail) {
                list.tail = prev; // Update the tail pointer
            }
            free(removed);
        }

    }
        if(list.head->data->tid == 1)
    {
        perror("head == 1");
    }
        else if(list.head->data->tid == 2)
    {
        perror("head == 2");
    }
}

// Select the next thread to schedule (round-robin)
thread next(void) {
    if (list.head == NULL) {
        perror("head empty");
        return NULL;
    }
    perror("head not empty");

    // Track the current node in the list
    static Node *current_node = NULL;

    if (current_node == NULL) {
        perror("current node empty, set to head: START");
        current_node = list.head;
        // list.tail = current_node;
    } else {
        current_node = current_node->next;
        if (current_node == NULL) 
        {
            perror("current node empty, set to head: END1");
            // list.tail->next = list.head;
            // list.tail = list.head;
            current_node = list.head;  // Reset to the head when reaching the end
            // list.tail = current_node;
        }
    }

    thread next_thread = current_node->data;

    perror("leave next");

    return next_thread;
}

// Get the number of threads in the queue
int qlen(void) {
    int count = 0;
    Node* current = list.head;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

// void print_scheduler(void) {
//     printf("Num in queue: %d\n", qlen());
//     printf("Scheduler Queue:\n");

//     Node* current = list.head;
//     while (current != NULL) {
//         printf("Thread: %p\n", current->data);
//         current = current->next;
//     }
// }
