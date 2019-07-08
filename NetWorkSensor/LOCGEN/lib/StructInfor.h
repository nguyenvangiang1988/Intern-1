/*
 * StructInfor.h
 *
 *  Created on: Jul 8, 2019
 *      Author: lhungtam
 */

#include "StructThreadCLI.h"

#ifndef STRUCTINFOR_H_
#define STRUCTINFOR_H_

//information send to ILS from LOCGEN
struct Infor{
    struct ThreadCLI *threadCLI;
    int *socketSend;
    int *socketRecieve;
    struct sockaddr_in *addressSend;
    struct sockaddr_in *addressRecieve;
};

void randomDegree(struct Infor*);

void sendData(struct Infor*);

void add(struct Infor*);

#endif /* STRUCTINFOR_H_ */
