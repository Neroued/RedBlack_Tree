// 队列的数组实现
#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#define MAX_QUEUE_SIZE 100

#include "rb_tree.h" 

typedef Node TItem;

typedef struct queue
{
    unsigned int head; // 分别存放队首队尾下标
    unsigned int tail;
    unsigned int size;
    TItem *item; // 数组指针
} Queue;

// 初始化队列
void InitializeQueue(Queue *pq);

// 队列是否为空
bool QueueIsEmpty(Queue *pq);

// 队列是否已满
bool QueueIsFull(Queue *pq);

// 入队操作
void EnQueue(const TItem *item, Queue *pq);

// 出队操作
TItem *DeQueue(Queue *pq);

// 清除队列
void DeleteQueue(Queue *pq);

#endif