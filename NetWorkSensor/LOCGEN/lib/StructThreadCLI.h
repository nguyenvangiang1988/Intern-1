/*
 * StructThreadCLI.h
 *
 *  Created on: Jul 8, 2019
 *      Author: lhungtam
 */

#include "StructThreadRoom.h"

#ifndef STRUCTTHREADCLI_H_
#define STRUCTTHREADCLI_H_

struct ThreadCLI{
    struct ThreadRoom *room;
    char *fileName;
    int timeSend;
    int numberRoomCurrent;
};

void initThreadCLI(struct ThreadCLI*);

void stop(struct ThreadCLI, int);

void start(struct ThreadCLI, int);

void writeFileConfig(struct ThreadCLI*);

void delete(struct ThreadCLI*, int);

void setTimeSend(struct ThreadCLI*, int);

#endif /* STRUCTTHREADCLI_H_ */
