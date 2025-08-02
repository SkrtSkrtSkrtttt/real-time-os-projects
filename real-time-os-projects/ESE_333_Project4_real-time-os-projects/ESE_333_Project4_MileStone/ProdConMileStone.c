//Naafiul Hossain
//ESE 333 Milestone Part 4 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

const int max = 10000000; // Number of messages to be sent
pthread_mutex_t lock; // Mutex for thread synchronization
int length = 0; // Number of entries in the linked list
pthread_t tid[2]; // Array to store thread IDs

struct node* head; // Pointer to the head of the linked list
struct node* tail; // Pointer to the tail of the linked list

struct node {
    struct node* next;
    int data;
};

// Consumer thread function
void *consumer(void *vargp) {
    int count = 0;
    while (count < max) {
        pthread_mutex_lock(&lock); // Lock the mutex to ensure exclusive access

        while (head != NULL) {
            if (head->data != count) {
                printf("ERROR! data %d should be %d!\n", head->data, count);
                break;
            }

            count++;

            if (count % 100000 == 0) { // Print only when count is a multiple of 10000
                printf("\nConsumer Removed: %d", count);
            }

            // Remove the node from the linked list
            if (head == tail) {
                free(head);
                head = NULL;
                tail = NULL;
            } else {
                struct node* tmp = head;
                head = head->next;
                free(tmp);
            }
        }

        pthread_mutex_unlock(&lock); // Unlock the mutex
    }
    return NULL;
}

// Producer thread function
void *producer(void *vargp) {
    int count = 0;
    while (count < max) {
        pthread_mutex_lock(&lock); // Lock the mutex to ensure exclusive access

        struct node* tmp = malloc(sizeof(struct node));
        tmp->data = count;
        tmp->next = NULL;

        if (head == NULL) {
            head = tmp;
        } else {
            tail->next = tmp;
        }

        tail = tmp;

        if (count % 100000 == 0) { // Print only when count is a multiple of 10000
            printf("Producer added: %d", count);
        }

        count++;
        pthread_mutex_unlock(&lock); // Unlock the mutex
    }
    return NULL;
}

int main() {
    pthread_mutex_init(&lock, NULL); // Initialize the mutex
    pthread_create(&tid[0], NULL, &producer, NULL); // Create the producer thread
    pthread_create(&tid[1], NULL, &consumer, NULL); // Create the consumer thread
    pthread_join(tid[1], NULL); // Wait for the consumer thread to finish
    pthread_join(tid[0], NULL); // Wait for the producer thread to finish
    if (head != NULL) {
        printf("ERROR! List not empty\n");
    }
    exit(0);
}

