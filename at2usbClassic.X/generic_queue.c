#include "generic_queue.h"
#include <string.h>

void GenericQueue_Init(GenericQueue *q, size_t element_size) {
    q->element_size = element_size;
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}

int GenericQueue_IsEmpty(GenericQueue *q) {
    return (q->size == 0);
}

int GenericQueue_IsFull(GenericQueue *q) {
    return (q->size == MAX_QUEUE_SIZE);
}

void GenericQueue_Enqueue(GenericQueue *q, const void *item) {
    if (!GenericQueue_IsFull(q)) {
        memcpy(&q->buffer[q->rear * q->element_size], item, q->element_size);
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
        q->size++;
    }
}

void GenericQueue_Dequeue(GenericQueue *q, void *item) {
    if (!GenericQueue_IsEmpty(q)) {
        memcpy(item, &q->buffer[q->front * q->element_size], q->element_size);
        q->front = (q->front + 1) % MAX_QUEUE_SIZE;
        q->size--;
    }
}

void GenericQueue_Remove(GenericQueue *q, const void *item) {
    if (!GenericQueue_IsEmpty(q)) {
        for (uint8_t i = q->front; i != q->rear; i = (i + 1) % MAX_QUEUE_SIZE) {
            if (memcmp(&q->buffer[i * q->element_size], item, q->element_size) == 0) {
                for (uint8_t j = i; j != q->rear; j = (j + 1) % MAX_QUEUE_SIZE) {
                    memcpy(&q->buffer[j * q->element_size],
                           &q->buffer[(j + 1) % MAX_QUEUE_SIZE * q->element_size],
                           q->element_size);
                }
                q->rear = (q->rear - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
                q->size--;
                break;
            }
        }
    }
}

bool GenericQueue_Contains(GenericQueue *q, const void *item) {
    if (!GenericQueue_IsEmpty(q)) {
        for (uint8_t i = q->front; i != q->rear; i = (i + 1) % MAX_QUEUE_SIZE) {
            if (memcmp(&q->buffer[i * q->element_size], item, q->element_size) == 0) {
                return true; // Item is found in the queue
            }
        }
    }
    return false; // Item is not found in the queue
}

void GenericQueue_CopyToArray(GenericQueue *q, void *array) {
    uint8_t index = q->front;
    uint8_t count = 0;
    while (count < q->size) {
        memcpy((char*)array + (count * q->element_size),
               &q->buffer[index * q->element_size],
               q->element_size);
        index = (index + 1) % MAX_QUEUE_SIZE;
        count++;
    }
}
