/*
 * StructInfor.c
 *
 *  Created on: Jul 8, 2019
 *      Author: lhungtam
 */


#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "StructInfor.h"

void randomDegree(struct Infor *infor){
    struct ThreadRoom *room;
    while (1){
        room=infor->threadCLI->room;
        while (room!=0){
            room->degree=rand()%40;
            room=room->next;
        }
        sleep(infor->threadCLI->timeSend);
    }
}

void sendData(struct Infor *infor){
    struct ThreadRoom *room, *roomEnd;
    roomEnd=(struct ThreadRoom*)malloc(sizeof(struct ThreadRoom));
    roomEnd->nameRooom=-1;

    connect(*infor->socketSend, (struct sockaddr*)infor->addressSend, sizeof(struct sockaddr));

    while (1){
        room=infor->threadCLI->room;
        while (room != 0){
            if (room->status==0){
                room=room->next;
                continue;
            }
            send(*infor->socketSend, room, sizeof(struct ThreadRoom), 0);
            room=room->next;
        }
        send(*infor->socketSend, roomEnd, sizeof(struct ThreadRoom), 0);
        sleep(infor->threadCLI->timeSend);
    }
}

void add(struct Infor *infor){
    struct ThreadRoom *threadRoom;
    threadRoom=(struct ThreadRoom *)malloc(sizeof(struct ThreadRoom));
    threadRoom->next=infor->threadCLI->room;
    infor->threadCLI->room=threadRoom;

    infor->threadCLI->numberRoomCurrent++;
    infor->threadCLI->room->degree=0;
    infor->threadCLI->room->nameRooom=infor->threadCLI->numberRoomCurrent;
    infor->threadCLI->room->status=1;

    pthread_create(&infor->threadCLI->room->p, NULL, (void*)&randomDegree, (void*)infor);
}
