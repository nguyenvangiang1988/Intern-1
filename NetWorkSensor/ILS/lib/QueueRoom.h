/*
 * QueueRoom.h
 *
 *  Created on: Jul 7, 2019
 *      Author: hungtam
 */

#include "StructThreadRoom.h"

#ifndef QUEUEROOM_H_
#define QUEUEROOM_H_

struct Queue{
    struct ThreadRoom *front;
    struct ThreadRoom *end;
};

void init(struct Queue*);

int isempty(struct Queue);// 1 is empty, 0 not empty

void enqueue(struct Queue*, struct ThreadRoom*);

struct ThreadRoom* delqueue(struct Queue*);

#endif /* QUEUEROOM_H_ */
