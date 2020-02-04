///Implementations of server.interface.h functions.
// Created by Dana on 1/17/2020.
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdbool.h>
#include <dirent.h>
#include "cJSON.h"
#include "myJSON.h"
#include "server.util.h"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

void handleClient(int clientSocket){
    char *buffer = malloc(MAX);
    bool isCorrect = false;
    //creating and init ing response.
    RES response;
    memset(buffer , 0 , MAX);
    //receives the request from client.
    int i = 1 , return_val = MAX;
    while(return_val == MAX * ((i == 1) ? 1 : (i - 1))){
        buffer = realloc(buffer , i * MAX);
        return_val = recv(clientSocket , buffer , (i++) * MAX , MSG_PEEK);
    }
    recv(clientSocket , buffer , MAX, 0);
    //prints the request that it took.
    printf("received the request: %s", buffer);
    //checks for the available patterns.
    if(strncmp(buffer , "register " , 9) == 0){
        char username[MAX] , password[MAX];
        if(extrDiInput(buffer + 9, username , password) == 0) {
            isCorrect = true;
            handleSignUp(username, password, &response);
            sendStringResponse(clientSocket, response);
        }
    }
    else if(strncmp(buffer , "login " , 6) == 0){
        char username[MAX] , password[MAX];
        if(extrDiInput(buffer + 6, username , password) == 0) {
            isCorrect = true;
            handleLogIn(username, password, &response);
            sendStringResponse(clientSocket, response);
        }
    }
    else if(strncmp(buffer, "create channel " , 15) == 0){
        char channelName[MAX], authToken[AL];
        if(extrDiInput(buffer + 15, channelName, authToken) == 0) {
            isCorrect = true;
            createChannel(channelName, authToken, &response);
            sendStringResponse(clientSocket, response);
        }
    }
    else if(strncmp(buffer, "join channel " , 13) == 0){
        char channelName[MAX], authToken[AL];
        if(extrDiInput(buffer + 13, channelName, authToken) == 0) {
            isCorrect = true;
            joinChannel(channelName, authToken, &response);
            sendStringResponse(clientSocket, response);
        }
    }
    else if(strncmp(buffer, "send ", 5) == 0){
        char *message, authToken[AL];
        if(msgExtr(buffer + 5, &message, authToken) == 0) {
            isCorrect = true;
            sendMessage(message, authToken, &response);
            sendStringResponse(clientSocket, response);
            free(message);
        }
    }
    else if(strncmp(buffer, "refresh ", 8) == 0){
        char authToken[AL];
        if(extrMonoInput(buffer + 8, authToken) == 0){
            isCorrect = true;
            response.listContent = myJSON_CreateArray();
            refresh(authToken, &response);
            sendListResponse(clientSocket, response, 0);
            //memory leak
//            myJSON_Delete(response.listContent);
        }
    }
    else if(strncmp(buffer, "channel members " , 16) == 0){
        char authToken[AL];
        if(extrMonoInput(buffer + 16, authToken) == 0){
            isCorrect = true;
            response.listContent = myJSON_CreateArray();
            listMembers(authToken, &response);
            sendListResponse(clientSocket, response, 1);
            //memory leak
        //    myJSON_Delete(response.listContent);
        }
    }
    else if(strncmp(buffer, "leave ", 6) == 0){
        char authToken[AL];
        if(extrMonoInput(buffer + 6, authToken) == 0){
            isCorrect = true;
            leaveChannel(authToken, &response);
            sendStringResponse(clientSocket, response);
        }
    }
    else if(strncmp(buffer, "logout ", 7) == 0){
        char authToken[AL];
        if(extrMonoInput(buffer + 7, authToken) == 0){
            isCorrect = true;
            logoutUser(authToken, &response);
            sendStringResponse(clientSocket, response);
        }
    }
    //if the request pattern was not valid.
    if(!isCorrect){
        char *response =  "{\"type\":\"error\",\"content\":\"Incompatible request pattern.\"}";
        send(clientSocket, response, strlen(response), 0);
    }
    //closes the client connection.
    free(buffer);
    closesocket(clientSocket);
}
void initChannels(char* path){
    //opening the directory given by path.
    DIR *dr = opendir(path);
    struct dirent *de;
    char channelPath[MAX], channelName[MAX];
    while((de = readdir(dr)) != NULL){
        //skips the first two sub-directories.
        if((strcmp(de->d_name , ".") == 0) || (strcmp(de->d_name, "..") == 0)){continue;}
        //extracting the channel's file name and path and passing it to addChannel().
        strcpy(channelName, de->d_name);
        strcpy(channelPath, path);
        strcat(channelPath, "\\");
        strcat(channelPath, channelName);
        //removing the extension from channel name.
        channelName[strlen(channelName) - 4] = '\0';
        //adds the channel.
        addChannel(channelName, channelPath);
    }
    //close the directory.
    closedir(dr);
}
void initUsers(char* path){
    //opens the users info file given by path.
    FILE *fp = fopen(path , "r");
    char content[MAX];
    myJSON *user = myJSON_CreateObject();
    //reads the file line by line.
    while(fgets(content, MAX, fp)) {
        content[strlen(content) - 1] = 0;
        myJSON_Delete(user);
        user = myJSON_Parse(content);
        if(user == NULL){
            //if the format is not json.
            printf("Sigh! failed to initialize the users\n");
            exit(-1);
        }
        //extracting the username and password and passes the user to addUser().
        char *username = myJSON_GetItemFromObject(user, "username")->strContent;
        char *password = myJSON_GetItemFromObject(user, "password")->strContent;
        addUser(username, password);
    }
    //closes the file and delets the json.
    fclose(fp);
    myJSON_Delete(user);
}
int createSocket(){

    int server_socket, client_socket;
    struct sockaddr_in server, client;

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("I guess its just not your day!\n");
        return -1;
    }

    // Create and verify socket
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
    if (server_socket == INVALID_SOCKET) {
        wprintf(L"Socket creator doesnt cooperate. He demands sacrifice!\n");
        return -1;
    }
    else
        printf("Socket successfully created..\n");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("You created a لجباز socket! It doesn't want to bound.\n");
        return -1;
    }
    else
        printf("Socket successfully bound..\n");
    return server_socket;
}
int accepting(int serverSocket){
    //puts server in listening mode.
    if ((listen(serverSocket, 5)) != 0)
    {
        //listening failed...
        printf("Socket is deaf! Can't listen :( press F to pay respect.\n");
        return -1;
    }
    else {printf("Server listening..\n");}

    int clientSocket;
    struct sockaddr_in server, client;
    int len = sizeof(client);
    while(1) { //accepting the client.
        clientSocket = accept(serverSocket, (SA *) &client, &len);
       // if(clientSocket != -1)
            handleClient(clientSocket);
    }
}

