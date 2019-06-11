/*
 ============================================================================
 Name        : serversocket.c
 Author      :
 Version     :
 Copyright   : Nguyen Minh Thang
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define FileQuestion "FileQuestion.txt"
#define FileAnswer "FileAnswer.txt"

//=============================================
// Init array list User
int MaximumPlayer= 3;
struct InformationUser{
    char Name[20];
    int Socket;
    int Next;
    int Check;
};

struct InformationUser UserPlayer[5];
void InitUserPlayer(){
    int i;
    for(i=0; i<MaximumPlayer; i++ ){
        memset(UserPlayer[i].Name, '\0', sizeof(UserPlayer[i].Name));
        UserPlayer[i].Check=1;
        UserPlayer[i].Socket=0;
        UserPlayer[i].Next=0;
    }

}

//-=========================================================
// Cheking user name 
int CheckUserName(char UserName[20]){
    for(int i=0; i< MaximumPlayer; i++){
        if( strcmp(UserPlayer[i].Name, UserName) == 0){
            return 1;
        }
    }
    return 0;
}

//========================================================
// Reading file return pharse with line 
char * FileData (int Line, char *File){
  int i;
  char *StrBuff = NULL;
  size_t StrBuffSize = 0;
  ssize_t LineSize;
  FILE *fp = fopen(File, "r");
  for(i=1;i<=Line;i++){
      LineSize =getline(&StrBuff, &StrBuffSize, fp);
  }
  fclose(fp);
  return StrBuff;
}

//=======================================================
//Checking special character form client's user
int CheckSpecialCharacter(char UserName[20]){
    for(int j=0; j< 20; j++){
       for(int i=65; i<= 122; i++){
            if(UserName[j]==i){
                return 0;            
            }
            if(i==90){
                i= 97;
            }
        }
    }
    return 1;
}

//=======================================================
//Checking Player's anwser true or false

int CheckAnswer(char Answer[20], int Line){
    char ReadFile[20];
    strcpy(ReadFile, FileData(Line, FileAnswer));
    if(ReadFile[0]==Answer[0]){
        return 1;
    }
    return 0;
}
//=======================================================
// main
int main(){
    InitUserPlayer();
    struct sockaddr_in addresServer;
    int valueOfSocket;
    int valueOfBind;
    int valueOfListen;
    valueOfSocket=socket(AF_INET, SOCK_STREAM, 0);
    if (valueOfSocket<0){
       printf("Error create server\n");
        return -1;
        exit(0);
    }
    else{
        printf("============Wellcom to WHO IS THE MILIONARE GAME================\n");
    }
    memset(&addresServer, '\0', sizeof(addresServer));
    addresServer.sin_port=htons(5000);
    addresServer.sin_family=AF_INET;
    inet_aton("192.168.81.12", &addresServer.sin_addr.s_addr);
    valueOfBind=bind(valueOfSocket,(struct sockaddr*)&addresServer, sizeof(addresServer));
    if (valueOfBind==0){
        printf(">> Designed by THANG NGUYEN\n");
    }
    else {
        perror("BIND: \n");
        return -1;
        exit(0);
    }
    valueOfListen=listen(valueOfSocket, 3);
    if (valueOfListen==0){
        printf(">> Waiting Player connect to server..............\n");
    }
    else {
        printf("false\n");
        return -1;
        exit(0);
    }
    struct sockaddr_in addressClient;
    socklen_t addressClient_Len;
    int valueOfAccept;
    int valueOfShutdown;
    int CountPlayer= 0;
    fd_set listSocket;
    char Message[20];

    while(1){ 
        while(CountPlayer < MaximumPlayer){
            FD_ZERO(&listSocket);
            FD_SET(valueOfSocket, &listSocket);
            int maxlength= valueOfSocket;
            select(maxlength+1, &listSocket, NULL, NULL, NULL);
            if(FD_ISSET(valueOfSocket, &listSocket)){               //Catch even connect from server
                addressClient_Len= sizeof(struct sockaddr_in);
                valueOfAccept=accept(valueOfSocket, (struct sockaddr*)&addressClient, &addressClient_Len);
                if(valueOfAccept< 0){
                    perror("ACCEPT: ");
                    return -1;
                    exit(0);
                }
                if(valueOfAccept>0){
                    FD_SET(valueOfAccept, &listSocket);             
                    char RequestInput[]= "Input User Name";
                    send(valueOfAccept, RequestInput, strlen(RequestInput), 0);            
                    select(valueOfAccept+1, &listSocket, NULL, NULL, NULL);
                    if(FD_ISSET(valueOfAccept, &listSocket)){               //waiting feedback from client
                        memset(Message, '\0', sizeof(Message));
                        recv(valueOfAccept, Message, sizeof(Message), 0);
                    }
                    
                     while(CheckUserName(Message) || CheckSpecialCharacter(Message)){ //Checking User Name and 
                        if(CheckUserName(Message)==1){                       
                            char InputAgain[]= "User name is EXITS, please input other name: ";
                            send(valueOfAccept, InputAgain, strlen(InputAgain), 0);
                        }
                        if(CheckSpecialCharacter(Message)==1){
                            char SpecialCharacter[]= "User name error, only input character A->Z and a->z. Please input again: ";
                            send(valueOfAccept, SpecialCharacter, strlen(SpecialCharacter), 0);
                        }                       
                        select(valueOfAccept+1, &listSocket, NULL, NULL, NULL); 
                        if(FD_ISSET(valueOfAccept, &listSocket)){
                           memset(Message, '\0', sizeof(Message));
                           recv(valueOfAccept, Message, sizeof(Message), 0);
                        }
                    }                                
                    for(int i=0; i< MaximumPlayer; i++ ){       //Receving notify register sucessfully
                        if(UserPlayer[i].Socket==0){
                            UserPlayer[i].Socket= valueOfAccept;
                            strcpy(UserPlayer[i].Name, Message);
                            printf("There is a new player form IP: %s\n",inet_ntoa(addressClient.sin_addr));  
                            printf("User Name: ");
                            puts(UserPlayer[i].Name);                
                            char ResgisterSuccess[]= "Resgister Successfully, please wait until full player...............";
                            send(valueOfAccept, ResgisterSuccess, strlen(ResgisterSuccess), 0);
                            CountPlayer++;
                            break;
                         }
                    }

                }
            }

        }
        FD_ZERO(&listSocket);
        for(int i=0; i< MaximumPlayer; i++){        // Receving information game to all client
            char Start[]= "\n============ START =============\n";
            char Total[30];
            sprintf(Total, ">> Sum Of Player Game: %d\n", MaximumPlayer);
            char TotalQuestion[]= ">> Total Question: 10\n";
            char YourTurn[30];
            sprintf(YourTurn, ">> Your Turn: %d\n", i+1);
            char Wait[]=">> Please wait the questions apperance..........";
            send(UserPlayer[i].Socket, Start, strlen(Start), 0);
            send(UserPlayer[i].Socket, Total, strlen(Total), 0);
            send(UserPlayer[i].Socket, TotalQuestion, strlen(TotalQuestion), 0);
            send(UserPlayer[i].Socket, YourTurn, strlen(YourTurn), 0);
            send(UserPlayer[i].Socket, Wait, strlen(Wait), 0);
            if(UserPlayer[i].Socket != 0){
                FD_SET(UserPlayer[i].Socket, &listSocket);
            }
        }
        int LineMin= 1;
        int LineMax= 6;
        int CountQuestion= 1;

        while(CountQuestion<=10){
            int i=0;
            for(i=0; i< MaximumPlayer; i++){
                for(int j= LineMin; j<= LineMax; j++){
                    send(UserPlayer[i].Socket, FileData(j, FileQuestion), strlen(FileData(j, FileQuestion)), 0);
                }
                if(UserPlayer[i].Next==0){
                    char Note[]= ">> Are you next this question? (y)";
                    send(UserPlayer[i].Socket, Note, strlen(Note), 0);
                }
                char AnswerPlayer[]= "\n>> Answer: ";
                send(UserPlayer[i].Socket, AnswerPlayer, strlen(AnswerPlayer), 0);
                if(FD_ISSET(UserPlayer[i].Socket, &listSocket)){
                    memset(Message, '\0', sizeof(Message));          
                    recv(UserPlayer[i].Socket, Message, sizeof(Message), 0);
                    if(strcmp( "y", Message)== 0){
                        char Next[]= "You are choice next, please wait the nex question...\n";
                        send( UserPlayer[i].Socket, Next, strlen(Next), 0);
                        UserPlayer[i].Next= 1;
                    }
                    else{
                        if(CheckAnswer(Message, CountQuestion)== 1){
                            char Correct[]= "Exactly! Please wait the next question...\n";
                            send(UserPlayer[i].Socket, Correct, strlen(Correct), 0);
                            char PlayerOfAnswer[30];
                            sprintf(PlayerOfAnswer, "%s's Anser is %s >>>>> Exactly", UserPlayer[i].Name, Message);
                            puts(PlayerOfAnswer);
                        }
                        else{
                            char InCorrect[]= "Wrong! You are fail. Good bye!";
                            send( UserPlayer[i].Socket, InCorrect, strlen(InCorrect), 0);
                            UserPlayer[i].Check= 0;
                            printf(">> Player %s is failed, disconnect to server!\n", UserPlayer[i].Name);
                            Display=0;
                            close(UserPlayer[i].Socket);
                        }

                    }
                }
                int PlayerWin= 0;
                int CountWin= 0;
                for(int i=0; i<MaximumPlayer; i++){
                    if(UserPlayer[i].Check==1){
                        PlayerWin= i;
                        CountWin++;
                    }
                }
                if(CountWin==1){
                    char NotifyWin[]= "You win! CONGRATULATE";
                    send(UserPlayer[PlayerWin].Socket, NotifyWin, strlen(NotifyWin), 0);
                    printf("%s is Winer\n", UserPlayer[PlayerWin].Name);
                    exit(0);
                }
            }
            LineMin+=6; 
            LineMax+=6;
            CountQuestion++;           
        }
        for(int i=0; i<MaximumPlayer; i++){
            if(UserPlayer[i].Check==1){
                char AllWin[]= "You win! CONGRATULATE";
                send(UserPlayer[i].Socket, AllWin, strlen(AllWin), 0);  
                printf("\n%s is Winer\n", UserPlayer[i].Name); 
                close(UserPlayer[i].Socket);                  
            }
        }
        exit(0);


    }
        
    return 0;
}
