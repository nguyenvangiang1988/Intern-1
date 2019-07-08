/*
 * StructThreadCLI.c
 *
 *  Created on: Jul 8, 2019
 *      Author: lhungtam
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StructThreadCLI.h"

void initThreadCLI(struct ThreadCLI *threadCLI){//note can use pointer in argument
    threadCLI->room=0;
    threadCLI->numberRoomCurrent=0;
    threadCLI->fileName=(char*)malloc(sizeof(char)*20);
    memset(threadCLI->fileName, '\0', sizeof(threadCLI));
    strcpy(threadCLI->fileName, "./config.xml");
    threadCLI->timeSend=2;
}

void stop(struct ThreadCLI threadCLI, int numberRoom){
    struct ThreadRoom *q;
    q=threadCLI.room;
    while (q != 0){
        if (q->nameRooom == numberRoom){
            q->status=0;
            break;
        }
        q=q->next;
    }
}

void start(struct ThreadCLI threadCLI, int numberRoom){
    struct ThreadRoom *q;
    q=threadCLI.room;
    while (q != 0){
        if (q->nameRooom == numberRoom){
            q->status=1;
            break;
        }
        q=q->next;
    }
}

void writeFileConfig(struct ThreadCLI *threadCLI){
    FILE *f;
    f=fopen(threadCLI->fileName, "w");
    time_t now = time(0);
    char *timeLocal=ctime(&now);
    struct ThreadRoom *q;
    q=threadCLI->room;

    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(f, "<configuration>\n");
    fprintf(f, "\t<object name=\"LOCGEN\"/>\n");
    fprintf(f, "\t<time send=\"%d\" update_file=\"%s\"/>\n", threadCLI->timeSend, timeLocal);
    fprintf(f, "\t<listroom total_room=\"%d\">\n", threadCLI->numberRoomCurrent);

    while (q != 0){
        fprintf(f, "\t\t<room name=\"%d\" status=\"%d\"/>\n", q->nameRooom, q->status);
        fprintf(stdout, "<room name=\"%d\" status=\"%d\"/>\n", q->nameRooom, q->status);
        q=q->next;
    }
    fprintf(f, "\t</listroom>\n");
    fprintf(f, "</configuration>\n");
    fclose(f);
}

void delete(struct ThreadCLI *threadCLI, int numberRoom){
    if (threadCLI->room != 0 && threadCLI->room->nameRooom==numberRoom){
        struct ThreadRoom *temp;
        temp=threadCLI->room;
        threadCLI->room=temp->next;
        pthread_cancel(temp->p);
        free(temp);
        threadCLI->numberRoomCurrent--;
        return;
    }
    struct ThreadRoom *room;
    room=threadCLI->room;
    while (room->next != 0){
        if (room->next->nameRooom == numberRoom){
            struct ThreadRoom *temp;
            temp=room->next;
            room->next=temp->next;
            pthread_cancel(temp->p);
            free(temp);
            threadCLI->numberRoomCurrent--;
            return;
        }
        room=room->next;
    }
}

void setTimeSend(struct ThreadCLI *threadCLI, int timeSend){
    threadCLI->timeSend=timeSend;
}
