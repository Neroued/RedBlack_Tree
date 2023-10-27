#include <stdio.h>
#include <stdlib.h>
#include "tree_queue.h"

static void CopyItem(const TItem *i1, TItem *i2);

void InitializeQueue(Queue *pq)
{
    pq->head = 0; // 左闭右开区间
    pq->tail = 0;
    pq->size = 0;
    pq->item = (TItem *)malloc(MAX_QUEUE_SIZE * sizeof(TItem));
}

bool QueueIsEmpty(Queue *pq)
{
    if (pq->size == 0)
        return true;
    return false;
}

bool QueueIsFull(Queue *pq)
{
    if (pq->size == MAX_QUEUE_SIZE) // 若tail与head重合表示已经循环了一圈
        return true;
    return false;
}

void EnQueue(const TItem *item, Queue *pq)
{
    if (QueueIsFull(pq))
        return;
    if (QueueIsEmpty(pq))
        CopyItem(item, pq->item+pq->tail); // 若队列为空，tail对应的位置为空，直接放入item
    else
    {
        pq->tail++;
        pq->tail %= MAX_QUEUE_SIZE;
        CopyItem(item, pq->item+pq->tail); // 若队列不为空，tail对应位置不为空，放入下一个位置}
    }
    pq->size++;
}
TItem *DeQueue(Queue *pq)
{
    if (QueueIsEmpty(pq))
        return NULL;
    pq->head++;
    pq->head %= MAX_QUEUE_SIZE;
    if (pq->size == 1) // 若仅有一个元素，为特殊情况
        pq->tail = pq->head;
    pq->size--;
    return pq->item + pq->head - 1;
}

void DeleteQueue(Queue *pq)
{
    free(pq->item);
    free(pq);
}

static void CopyItem(const TItem *i1, TItem *i2)
{
    i2->left = i1->left;
    i2->right = i1->right;
    i2->parent = i1->parent;
    i2->color = i1->color;
    i2->item.num = i1->item.num;
}   