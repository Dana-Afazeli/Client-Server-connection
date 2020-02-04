//lets do this baby!
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <stdbool.h>
#include "myJSON.h"
#include "myJSON.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

 int send_request(char *);//sends the request to the server and handles the responce.
 int us_pass_input(char * , char *);//inputs the username and password. two arguments requests.
 int channel_input(char *);//inputs the channel name for create and join functions.
void us_pass_concat(char*,char*,char*);//creates the message for sign-up and log-in functions.
void one_argument_concat(char*,char*);//handles one argument concatenations.
 int connect_to_server(void);//connects to the server.
 int first_menu(void);//registration and logging handling.
 void main_menu(void);//main menu of application.
 int create_channel(void);//creates a channel and enters the chat room.
 int join_channel(void);//joins a chat room.
 void send_message(void);//sends a message in the chat room.
 void refresh(void);//refreshes the chat room.
void show_json(myJSON * , int);//function to print logs.
 void list_members(void);//lists the members of the chat room.
void print_members(myJSON*);//prints the members.
 void leave_channel(void);//leaves the current channel.
 int logout(void);//logs out of the chat room. clears the authentication token.
 void chat_room(void);//the chat room.
 int sign_up(void);//registration function.
 int log_in(void);//logging function. upon successful log-in modifies authentication token.

///authetincation token.
char auth_token[MAX];

int main(){
    // Enter the chat room.
    printf("Welcome to Afagram!\n");
    while(true){
        if(first_menu() != 0){ // registration and logging handling.
            return 0;
        }
        main_menu();
    }
    return 0;
}

int connect_to_server(){// connets to the server.
    // Using the pre-written code to connect to the server.
    int client_socket, server_socket;
	struct sockaddr_in servaddr, cli;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        exit(0);
    }

	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else{
		///printf("Socket successfully created..\n");
	}
	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
	else{
		///printf("Successfully connected to the server..\n");
	}

    return client_socket;

}

void show_json(myJSON *array , int size){//prints logs.
    for(int i = 0; i < MAX; i++)
        printf("-");
    printf("\n");

    for(int i = 0; i < size; i++){
        myJSON *item = myJSON_GetItemFromArray(array, i);
        char *name = myJSON_GetItemFromObject(item , "sender")->strContent;
        char *content = myJSON_GetItemFromObject(item , "content")->strContent;
        printf("%s: %s\n" , name , content);
    }
    for(int i = 0; i < MAX; i++)
        printf("-");
    printf("\n");
}

void print_members(myJSON *array){//prints members.
    for(int i = 0; i < MAX; i++)
        printf("-");
    printf("\n");

    for(int i = 0; i < array->arrSize; i++){
        printf("%s\n", array->arrContent[i]->strContent);
    }

    for(int i = 0; i < MAX; i++)
        printf("-");
    printf("\n");
}

int send_request(char *message){// sends the request to server.
    int client_socket = connect_to_server(); //connecting to the server.
    if(send(client_socket , message , MAX , 0) == -1){
        printf("Sending request has failed\n");
        return - 1;
    }
    char *responce = realloc(responce ,MAX);
    int i = 1 , return_val = MAX;
    while(return_val == MAX * ((i == 1) ? 1 : (i - 1))){
        responce = realloc(responce , i * MAX);
        return_val = recv(client_socket , responce , (i++) * MAX , MSG_PEEK);
    }

    recv(client_socket , responce , MAX * (i - 1) , 0);

    myJSON *res;
    res = myJSON_Parse(responce); //translating the responce to a json object.
    if(res == NULL){
        printf("Recieving responce has failed\n");
        return -1;
    }
    //extracting the result.
    char *type = myJSON_GetItemFromObject(res ,"type")->strContent;
    if(strncasecmp(type , "AuthToken" , 9) == 0){//log-int responce.
        char *auth = myJSON_GetItemFromObject(res , "content")->strContent;
        int n = 0;
        while((auth_token[n] = auth[n]) != '\0')
            n++;

        printf("Logging was Successful!\nWelcome!\n");
        return 0;
    }
    else if(strncmp(type , "List" , 4) == 0){//refresh and list members responce.
        if(strncmp(message , "refresh" , 7) == 0){
            myJSON *logs = myJSON_GetItemFromObject(res , "content");

            show_json(logs , myJSON_GetArraySize(logs));
        }
        else{
            myJSON *members = myJSON_GetItemFromObject(res , "content");
            print_members(members);
        }

    }
    else if(strncmp(type , "Successful" , 10) == 0){
        printf("Request was Successful!\n");
        return 0;
    }
    else{ // showing the reason of failure.
        char *content = myJSON_GetItemFromObject(res , "content")->strContent;
        printf("Request has failed because: %s\n" , content);
        return -1;
    }
}

int us_pass_input(char * username , char * password){// util function to input username and password.
    int n;
    printf("For undoing or exiting, enter \"back\"\n");
    while(true){ // inputing loop.
            printf("Enter Username:\n");
            memset(username , 0 , MAX);
            n = 0;
            while((username[n++] = getchar()) != '\n')
                ;
            username[n - 1] = '\0';
            if(strncmp(username , "back" , 4) == 0) //exiting condition.
                return -1;

            printf("Enter password:\n");
            n = 0;
            memset(password , 0 , MAX);
            while(((password[n] = getch()) != '\r') && ((password[n]) != '\n')){
                if(password[n] == 8){
                    printf("\b \b");
                    n -= 1;
                    continue;
                }
                printf("*");
                n++;
            }
            password[n] = '\n';
            printf("\n");
            if(strncmp(password , "back" , 4) == 0) //inputing username again.
                continue;
            else
                return 0;
        }
}

    void us_pass_concat(char *message , char *username , char *password){// util function for concatenation.
    strcat(message, username);
    strcat(message, ", ");
    strcat(message, password);
    return;
}

int first_menu(){// sign-in and log-in menu.
    char buffer[MAX];
    int n;
    while(true){
        printf("Please sign-up/login to continue(enter 1 or 2 and press enter):\n1: Sign-up\n2: Login\nFor termination enter \"exit\"\n");
        memset(buffer , 0 , sizeof(buffer));
        n = 0;
        while((buffer[n++] = getchar()) != '\n')
            ;
        if(strncmp(buffer , "exit" , 4) == 0){ // termination condition.
            printf("Have a nice time!\nStopping client...\n");
            return -1;
        }
        else if(buffer[0] == '1'){
            if(sign_up() == 0){
                if(log_in() == 0){
                    return 0;
                }
                else{
                    continue;
                }
            }
        }
        else if(buffer[0] == '2'){
            if(log_in() == 0){
                return 0; //logging complete.
            }
            else{
                continue; //returned because of back.
            }
        }
    }
}

int sign_up(){// sign-up fucntion.
    char username[MAX];
    char password[MAX];
    char message[MAX] = {};
    while(true){
        if(us_pass_input(username , password) != 0)
            return -1;
        memset(message , 0 , MAX);
        strcat(message, "register ");//composing the messsage.
        us_pass_concat(message , username , password);

        if(send_request(message) == 0){
            printf("Entering log-in menu...\n");
            return 0;
        }
        else{
            continue;
        }
    }
}

int log_in(){// log-in function.
    char username[MAX];
    char password[MAX];
    char message[MAX] = {};
    while(true){
        if(us_pass_input(username , password) != 0){
            return -1;
        }

        memset(message , 0 , MAX);
        strcat(message , "login ");//composing the messsage.
        us_pass_concat(message , username , password);

        if(send_request(message) == 0){
            return 0;
        }
        else{
            continue;
        }
    }
}

void main_menu(){// the main menu of application.
    char buffer[MAX];
    int n;
    while(true){
        printf("Please choose an action:\n1: Create channel\n2: Join channel\n3: Logout\n"
               "You can also logout by entering \"exit\"\n");
        memset(buffer , 0 , sizeof(buffer));
        n = 0;
        while((buffer[n++] = getchar()) != '\n')
            ;
        if(buffer[0] == '1'){
            if(create_channel() == 0){
              //  if(join_channel() == 0)
                    chat_room();
            }
            continue;
        }
        if(buffer[0] == '2'){
            if(join_channel() == 0){
                chat_room();
            }
            continue;
        }
        if(buffer[0] == '3' ||(strncmp(buffer , "exit" , 4) == 0)){
            if(logout() == 0)
                return;
            else
                continue;
        }
    }
}

int channel_input(char *name){// util function for create and join channel.
    int n = 0;
    printf("For exiting, enter \"back\"\n");
    while(true){
        printf("Enter the name of the channel:\n");
        memset(name, 0 ,MAX);
        n = 0;
        while((name[n++] = getchar()) != '\n')
            ;
        name[n - 1] = '\0';
        if(strncmp(name , "back" , 4) == 0) //exiting condition.
            return -1;
        else
            return 0;
    }
}

void one_argument_concat(char *message ,char *argument){// util for concatenation.
    strcat(message , argument);
    strcat(message , ", ");
    strcat(message , auth_token);
    strcat(message , "\n");
    return;
}

int create_channel(){// creates a channel.
    char name[MAX];
    char message[MAX] = {};

    while(true){
        if(channel_input(name) != 0)
            return -1;
        memset(message , 0 , MAX);
        strcat(message , "create channel ");// composing the message.
        one_argument_concat(message, name);

        if(send_request(message) == 0){
            return 0;
        }
        else
            continue;
    }
}

int join_channel(){// joins a channel.
    char name[MAX];
    char message[MAX] = {};

    while(true){
        if(channel_input(name) != 0)
            return -1;
        memset(message , 0 , MAX);
        strcat(message , "join channel ");
        one_argument_concat(message , name);

        if(send_request(message) == 0){
            return 0;
        }
        else
            continue;
    }
}

int logout(){// logs out of the server.
    char message[MAX] = "logout ";
    strcat(message , auth_token);
    strcat(message , "\n");
    if(send_request(message) == 0)
        return 0;
    else
        return -1;
}

void chat_room(){// the chatting room of the application.
    char buffer[MAX];
    int n;
    printf("Welcome! choose an action:\nYou can also leave channel by entering \"exit\"\n");
    while(true){
        printf("1: Send message\n2: Refresh\n3: Channel members\n4: Leave channel\n");

        memset(buffer , 0 , sizeof(buffer));
        n = 0;
        while((buffer[n++] = getchar()) != '\n')
            ;
        if(buffer[0] == '1'){
            send_message();
            continue;
        }
        else if(buffer[0] == '2'){
            refresh();
            continue;
        }
        else if(buffer[0] == '3'){
            list_members();
            continue;
        }
        else if(buffer[0] == '4' || (strncmp(buffer , "exit" , 4) == 0)){
            leave_channel();
            break;
        }
    }
}

void send_message(){//sends a message.
    char message_to_send[3 * MAX];
    char message[MAX] = {};
    int n = 0;

    printf("Enter your message:\n");
    while((message_to_send[n++] = getchar()) != '\n') //inputs the message.
        ;
    message_to_send[n - 1] = '\0';
    strcat(message , "send ");// composing the message.
    one_argument_concat(message , message_to_send);

    send_request(message);
    return;
}

void refresh(){//refreshes the messages
    char message[MAX] = {};

    strcat(message , "refresh ");//composing the message.
    strcat(message , auth_token);
    strcat(message , "\n");

    send_request(message);
    return;
}

void list_members(){//lists the members.
    char message[MAX] = {};

    strcat(message , "channel members ");//composing the message.
    strcat(message , auth_token);
    strcat(message ,"\n");

    send_request(message);
    return;
}

void leave_channel(){//leaves the channel.
    char message[MAX] = {};

    strcat(message , "leave ");
    strcat(message , auth_token);
    strcat(message , "\n");

    send_request(message);
    return;
}
