#ifndef __COMMON_LIB__

#define __COMMON_LIB__
#define BUFFER_LENGTH 1000

char* GetLineFromSocket(int socket){
    char c;
    char* sp = (char*)malloc(BUFFER_LENGTH);
    int i = 0;

    while (recv(socket, &c, sizeof c, 0) == 1) {
        if (c == '\n') {
            sp[i] = 0;
            return sp;
        }
        else {
            sp[i++] = c;
        }
    }

    return 0;
}

int SendLineToSocket(int socket, char* str){
    int result;
    int length = strlen(str);
    fflush(stdout);
    
    char* message = (char*)malloc(length + 1);
    strcpy(message, str);
    message[length] = '\n';
    
    result = send(socket, message, length + 1, 0);

    free(message);

    return result;
}

#endif
