/*
 * StructThreadRoom.h
 *
 *  Created on: Jul 8, 2019
 *      Author: lhungtam
 */

#include <pthread.h>

#ifndef STRUCTTHREADROOM_H_
#define STRUCTTHREADROOM_H_

struct ThreadRoom{
    pthread_t p;
    int returnValue;
    int status;
    int degree; //0 stop 1 running
    int nameRooom; //number of room
    struct ThreadRoom *next;
};

#endif /* STRUCTTHREADROOM_H_ */
