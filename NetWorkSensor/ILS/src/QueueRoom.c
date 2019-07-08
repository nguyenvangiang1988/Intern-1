/*
 * QueueRoom.c
 *
 *  Created on: Jul 7, 2019
 *      Author: hungtam
 */


#include "QueueRoom.h"

void init(struct Queue *queue){
    queue->front=0;
    queue->end=0;
}

int isempty(struct Queue queue){// 1 is empty, 0 not empty
    if (queue.front==0 && queue.end==0){
        return 1;
    }
    else{
        return 0;
    }
}

void enqueue(struct Queue *queue, struct ThreadRoom *room){
    if (isempty(*queue)){//insert element first
        queue->front=room;
        queue->end=room;
        room->next=0;
    }
    else{
        queue->end->next=room;
        room->next=0;
        queue->end=room;
    }
}

struct ThreadRoom* delqueue(struct Queue *queue){//hfdsfskjfhjdfhsfhsfskjfhsdkjfhskfdjhskfjhk
    if (!isempty(*queue) && queue->end==queue->front){//del las element
        struct ThreadRoom *temp;
        temp=queue->front;
        queue->front=0;
        queue->end=0;
        return temp;
    }
    else{
        struct ThreadRoom *temp;
        temp=queue->front;
        queue->front=temp->next;
        return temp;
    }
}
