#include "rr.h"
#include "lwp.h"

#include <stdio.h>
#include <stdlib.h>

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
    list->head = NULL;
    list->tail = NULL;
}

// Global variable to hold the current RR scheduler
// static scheduler current_scheduler = NULL;
static LinkedList list;

// // Initialize the RR scheduler
void init(void) {
    if (list.head == NULL) {
        initLinkedList(&list);
    }

    if (current_scheduler == NULL) {
        current_scheduler = (scheduler)malloc(sizeof(struct scheduler));
        current_scheduler->init = init;
        current_scheduler->shutdown = shutdown;
        current_scheduler->admit = admit;
        current_scheduler->rem = rem;
        current_scheduler->next = next;
        current_scheduler->qlen = qlen;
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
    printf("in admit\n");
    if (new == NULL) {
        return; // Do not admit NULL threads
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = new;
    newNode->next = NULL;
    printf("after malloc\n");

    if (list.head == NULL) {
        printf("in if\n");
        list.head = newNode;
        list.tail = newNode;
    } else {
        printf("in else\n");
        list.tail->next = newNode;
        list.tail = newNode;
    }
}

// Remove a thread from the round-robin queue
void rem(thread victim) {
    if (victim == NULL || list.head == NULL) {
        return; // Do not rem NULL threads or from an empty queue
    }

    if (list.head->data == victim) {
        Node* next_node = list.head->next;
        free(list.head);
        list.head = next_node;
        if (list.head == NULL) {
            list.tail = NULL;
        }
    } else {
        Node* prev = list.head;
        while (prev->next != NULL && prev->next->data != victim) {
            prev = prev->next;
        }
        if (prev->next != NULL) {
            Node* removed = prev->next;
            prev->next = removed->next;
            free(removed);
            if (removed == list.tail) {
                list.tail = prev;
            }
        }
    }
}

// Select the next thread to schedule (round-robin)
thread next(void) {
    if (list.head == NULL) {
        return NULL;
    }

    Node* next_node = list.head;
    list.head = next_node->next;

    if (list.head == NULL) {
        list.tail = NULL;
    }

    thread next_thread = next_node->data;
    free(next_node);

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

void print_scheduler() {
    printf("Num in queue: %d\n", qlen());
    printf("Scheduler Queue:\n");

    Node* current = list.head;
    while (current != NULL) {
        printf("Thread: %p\n", current->data);
        current = current->next;
    }
}
