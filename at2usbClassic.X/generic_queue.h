#ifndef GENERIC_QUEUE_H
#define GENERIC_QUEUE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 15
#define MAX_ELEMENT_SIZE 6 // Maximum size of an element, adjust as needed

typedef struct {
    uint8_t buffer[MAX_QUEUE_SIZE * MAX_ELEMENT_SIZE];
    size_t element_size;
    uint8_t front;
    uint8_t rear;
    uint8_t size;
} GenericQueue;

void GenericQueue_Init(GenericQueue *q, size_t element_size);
int GenericQueue_IsEmpty(GenericQueue *q);
int GenericQueue_IsFull(GenericQueue *q);
void GenericQueue_Enqueue(GenericQueue *q, const void *item);
void GenericQueue_Dequeue(GenericQueue *q, void *item);
void GenericQueue_Remove(GenericQueue *q, const void *item);
bool GenericQueue_Contains(GenericQueue *q, const void *item);
void GenericQueue_CopyToArray(GenericQueue *q, void *array);

#endif /* GENERIC_QUEUE_H */
