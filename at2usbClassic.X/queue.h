/* 
 * File:   queue.h
 * Author: BojanSofronievski
 *
 * Created on April 22, 2024, 11:02 PM
 */

#ifndef QUEUE_H
#define	QUEUE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_QUEUE_SIZE 6

typedef struct {
    uint8_t data[MAX_QUEUE_SIZE];
    uint8_t front;
    uint8_t rear;
    uint8_t size; // New: to keep track of the number of elements in the queue
} Queue;

void Queue_Init(Queue *q);
bool Queue_IsEmpty(Queue *q);
bool Queue_IsFull(Queue *q);
void Queue_Enqueue(Queue *q, uint8_t item);
uint8_t Queue_Dequeue(Queue *q);
void Queue_Remove(Queue *q, uint8_t item);
bool Queue_Contains(Queue *q, uint8_t item);
void Queue_CopyToArray(Queue *q, uint8_t *array);


#ifdef	__cplusplus
}
#endif

#endif	/* QUEUE_H */

