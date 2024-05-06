#include "queue.h"

void Queue_Init(Queue *q) {
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}

bool Queue_IsEmpty(Queue *q) {
    return (q->size == 0);
}

bool Queue_IsFull(Queue *q) {
    return (q->size == MAX_QUEUE_SIZE);
}

void Queue_Enqueue(Queue *q, uint8_t item) {
    if (!Queue_IsFull(q)) {
        q->data[q->rear] = item;
        q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
        q->size++;
    }
}

uint8_t Queue_Dequeue(Queue *q) {
    uint8_t item;
    if (!Queue_IsEmpty(q)) {
        item = q->data[q->front];
        q->front = (q->front + 1) % MAX_QUEUE_SIZE;
        q->size--;
    }
    return item;
}

void Queue_Remove(Queue *q, uint8_t item) {
    if (!Queue_IsEmpty(q)) {
        for (uint8_t i = q->front; i != q->rear; i = (i + 1) % MAX_QUEUE_SIZE) {
            if (q->data[i] == item) {
                for (uint8_t j = i; j != q->rear; j = (j + 1) % MAX_QUEUE_SIZE) {
                    q->data[j] = q->data[(j + 1) % MAX_QUEUE_SIZE];
                }
                q->rear = (q->rear - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
                q->size--;
                break;
            }
        }
    }
}

bool Queue_Contains(Queue *q, uint8_t item) {
    for (uint8_t i = q->front; i != q->rear; i = (i + 1) % MAX_QUEUE_SIZE) {
        if (q->data[i] == item) {
            return true; // Key is found in the queue
        }
    }
    return false; // Key is not found in the queue
}

void Queue_CopyToArray(Queue *q, uint8_t *array) {
    uint8_t index = q->front;
    uint8_t count = 0;

    while (count < q->size) {
        array[count] = q->data[index];
        index = (index + 1) % MAX_QUEUE_SIZE;
        count++;
    }
}