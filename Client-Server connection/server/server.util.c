#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "cJSON.h"
#include "myJSON.h"
#include "myJSON.c"
#include "server.util.h"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
#define AUTH_TOKEN_length  30
#define AL  AUTH_TOKEN_length
#define USERS_FILE_PATH "Resources\\Users\\users.txt"
#define UFP USERS_FILE_PATH
#define CHANNELS_DIR_PATH "Resources\\Channels\\"
#define CDP CHANNELS_DIR_PATH
//typedefs.

typedef struct User{
    char username[MAX];
    char password[MAX];
    char authToken[AL];
    char *channelPath;
    int channelInd;
    int refreshPos;
}USER;
struct UserManager{
    USER *users;
    int size;
    int maxSize;
}userManager;
typedef struct Channel{
    char name[MAX];
    char *filePath;
    int *members;
    int size;
    int maxSize;
}CHAN;
struct ChannelManager{
    CHAN *channels;
    int size;
    int maxSize;
}channelManager;


//functions.
int checkUsername(char* username);
void writeUser(char* username, char* password);
void createToken(int index);
void addUserToChannel(int uInd, int chInd);
int checkAuthToken(char* authToken);
int checkChannelName(char* channelName);
void addMessageToList(char *sender, char *msg, RES* response);
void addMemberToList(char* member, RES* response);
void rmUsFromChan(int uInd, int chInd);

//Implementations.
void addChannel(char* channelName, char* channelPath){
    //resize if needed.
    if(channelManager.size == channelManager.maxSize){
        channelManager.channels = realloc(channelManager.channels, sizeof(CHAN)*(channelManager.maxSize += 10));
    }
    //adds the channel to channel manager.
    channelManager.channels[channelManager.size].maxSize = 0;
    channelManager.channels[channelManager.size].size = 0;
    channelManager.channels[channelManager.size].members = 0;
    channelManager.channels[channelManager.size].filePath = malloc(strlen(channelPath) + 1);
    strcpy(channelManager.channels[channelManager.size].filePath , channelPath);
    strcpy(channelManager.channels[channelManager.size].name, channelName);
    channelManager.size++;
}
void addUser(char* username, char* password){
    //resize if needed.
    if(userManager.size == userManager.maxSize){
        userManager.users = realloc(userManager.users, sizeof(USER)*(userManager.maxSize += 10));
    }
    //adds the user to user manager.
    userManager.users[userManager.size].authToken[0] = '\0';
    userManager.users[userManager.size].channelInd = 0;
    userManager.users[userManager.size].channelPath = 0;
    userManager.users[userManager.size].refreshPos = 0;
    strcpy(userManager.users[userManager.size].username, username);
    strcpy(userManager.users[userManager.size].password, password);
    userManager.size++;
}
void sendStringResponse(int clientSocket, RES response){
    //creates the json to be sent.
    myJSON *res = myJSON_CreateObject();
    myJSON_AddStringToObject(res, "type", response.type);
    myJSON_AddStringToObject(res, "content", response.stringContent);
    //prints the json to a string.
    char *toSend = myJSON_Print(res);
    //sends the string.
    printf("The response sent:\n%s\n", toSend);
    send(clientSocket, toSend, strlen(toSend) + 1, 0);
    //de-allocating the json.
    myJSON_Delete(res);
    free(toSend);
    return;
}
void sendListResponse(int clientSocket, RES response, int flag) {
    //creates the json to be sent.
    myJSON *res = myJSON_CreateObject();
    myJSON_AddStringToObject(res, "type", response.type);
    //adds the list to the json
    myJSON_AddItemToObject(res, "content", response.listContent);
    //print the json in a string.
    char *toSend = myJSON_Print(res);
    //send the string.
    printf("The response sent:\n%s\n", toSend);
    send(clientSocket, toSend, strlen(toSend) + 1, 0);
    //de-allocating the json.
    myJSON_Delete(res);
    free(toSend);
    return;
}
int extrDiInput(char* exFrom, char* exTo1, char* exTo2){
    int i;
    char c = 1;
    //extracting the first part.
    sscanf(exFrom, "%c" , &c);
    for(i = 0; (c != ',') ; i++){
        //space and null are not allowed.
        if(c == ' ' || c == 0){i = 0; break;}
        exTo1[i] = c;
        sscanf(exFrom + i + 1, "%c" , &c);
    }
    //terminates the string.
    exTo1[i] = 0;
    //checks if the first part is valid.
    if(exTo1[0] == 0 || exFrom[i + 1] != ' '){return -1;}
    //extracting the second part.
    sscanf(exFrom + strlen(exTo1) + 2, "%c" , &c);
    for(i = 0; (c != '\n'); i++){
        //space and null are not allowed.
        if(c == ' ' || c == 0){i = 0; break;}
        exTo2[i] = c;
        sscanf(exFrom + i + 1 + strlen(exTo1) + 2, "%c" , &c);
    }
    //terminates the string.
    exTo2[i] = 0;
    //checks if the second part is valid.
    if(exTo2[0] == 0 || exFrom[i + 1 + strlen(exTo1) + 2] != 0){return -1;}
    //returns with no error.
    return 0;
}
int extrMonoInput(char*exFrom, char* Arg){
    int i;
    char c = 1;
    //reads the string character by character.
    sscanf(exFrom, "%c" , &c);
    for(i = 0; (c != '\n') ; i++){
        //space and null are not allowed.
        if(c == ' ' || c == 0){i = 0; break;}
        Arg[i] = c;
        sscanf(exFrom + i + 1, "%c" , &c);
    }
    //terminates the string.
    Arg[i] = 0;
    //checks if the pattern is valid.
    if(Arg[0] == 0 || exFrom[i + 1 ] != 0){return -1;}
    //returns with no error.
    return 0;
}
int msgExtr(char* exFrom, char** msg, char* authToken){
    //since message is very flexible, we start from the end.
    //checking the failing conditions.
    if(exFrom[(int)strlen(exFrom) - 1] != '\n' ||
       exFrom[(int)strlen(exFrom) - 1 - AL] != ' ' ||
       exFrom[(int)strlen(exFrom) - 2 - AL] != ',' ){return -1;}
    //extracting the second part.
    char c;
    for(int i = 0; i < AL - 1; i++){
        sscanf(exFrom + strlen(exFrom) - AL + i, "%c", &c);
        if(c == '\0'){return -1;}
        authToken[i] = c;
    }
    authToken[AL - 1] = '\0';
    *msg = malloc((int)strlen(exFrom) - AL - 1);
    strncpy(*msg , exFrom, (int)strlen(exFrom) - AL - 1);
    (*msg)[(int)strlen(exFrom) - AL - 2] = '\0';
    return 0;
}
void handleSignUp(char* username, char* password, RES* response){
    //if the username is already taken
    if(checkUsername(username) != -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "Username is already taken");
        return;
    }
    //creates the user.
    addUser(username, password);
    writeUser(username, password);
    //filling the response accordingly.
    strcpy(response->type, "Successful");
    strcpy(response->stringContent, "User registered successfully");
}
int checkUsername(char* username){
    //iterates through all of the current users and upon matching names, returns the index.
    for(int i = 0; i < userManager.size; i++){
        if(strcmp(username, userManager.users[i].username) == 0){return i;}
    }
    //if no matches found the return -1.
    return -1;
}
void writeUser(char* username, char* password){
    //creates the json to hold the data.
    myJSON *userData = myJSON_CreateObject();
    myJSON_AddStringToObject(userData, "username", username);
    myJSON_AddStringToObject(userData, "password", password);
    //prints the json into a string.
    char *toWrite = myJSON_Print(userData);
    //appends the string to the file.
    FILE *userFile = fopen(UFP, "a");
    fprintf(userFile, "%s\n", toWrite);
    //closes the file and de-allocates the json.
    fclose(userFile);
    myJSON_Delete(userData);
    free(toWrite);
    return;
}
void handleLogIn(char* username, char* password, RES* response){
    int index = checkUsername(username);
    //if the username or password are not valid.
    if(index == -1 || strcmp(userManager.users[index].password, password) != 0){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "username or password are incorrect.");
        return;
    }
    //if the user is already logged-in.
    if(userManager.users[index].authToken[0] != '\0'){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "user is already logged-in.");
        return;
    }
    //create an authentication token.
    createToken(index);
    //filling the response.
    strcpy(response->type, "AuthToken");
    strcpy(response->stringContent, userManager.users[index].authToken);
}
void createToken(int index){
    //creates a AL - 1 character token using numbers and alphabet(a-z, A-Z).
    char tokenCreated[AL];
    memset(tokenCreated, 0, AL);
    srand(time(0));
    int random;
    for(int i = 0; i < AL - 1; i++){
        random = rand() % 62;
        //interpreting the number generated.
        if(random >= 0 && random <= 9){tokenCreated[i] = '0' + random;}
        else if(random > 9 && random <= 35){tokenCreated[i] = 'a' + random - 10;}
        else{tokenCreated[i] = 'A' + random - 36;}
    }

    //copying the token to the user[index].
    strcpy(userManager.users[index].authToken, tokenCreated);
    return;
}
void createChannel(char* channelName, char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //if the channel name already exists.
    if(checkChannelName(channelName) != -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "channel already exists.");
        return;
    }
    //creates the channel path.
    char channelPath[MAX];
    strcpy(channelPath,CDP);
    strcat(channelPath, channelName);
    strcat(channelPath, ".txt");
    //creates the channel and writes the first message in it.
    FILE *chPtr = fopen(channelPath, "a");
    myJSON *msg = myJSON_CreateObject();
    char zapas[2 * MAX];
    strcpy(zapas, userManager.users[uInd].username);
    strcat(zapas, " joined the channel!");
    myJSON_AddStringToObject(msg, "sender", "server");
    myJSON_AddStringToObject(msg, "content", zapas);
    char *message = myJSON_Print(msg);
    fprintf(chPtr, "%s\n", message);
    addChannel(channelName, channelPath);
    addUserToChannel(uInd, channelManager.size - 1);
    //prevent memory leak.
    fclose(chPtr);
    myJSON_Delete(msg);
    free(message);
    //filling the response.
    strcpy(response->type, "Successful");
    strcpy(response->stringContent, "Channel created successfully.");
}
void joinChannel(char* channelName, char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //if the channel name doesn't exist.
    int chInd = checkChannelName(channelName);
    if(chInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "channel doesn't exist.");
        return;
    }
    //user joins the channel.
    addUserToChannel(uInd, chInd);
    //writes the joining message in the channel file.
    FILE *chPtr = fopen(channelManager.channels[chInd].filePath, "a");
    myJSON *msg = myJSON_CreateObject();
    char zapas[2 * MAX];
    strcpy(zapas, userManager.users[uInd].username);
    strcat(zapas, " joined the channel!");
    myJSON_AddStringToObject(msg, "sender", "server");
    myJSON_AddStringToObject(msg, "content", zapas);
    char *message = myJSON_Print(msg);
    fprintf(chPtr,"%s\n", message);
    //memory leak.
    fclose(chPtr);
    myJSON_Delete(msg);
    free(message);
    //filling the response.
    strcpy(response->type, "Successful");
    strcpy(response->stringContent, "User joined channel successfully.");
}
void addUserToChannel(int uInd, int chInd){
    //setting the pointers and initializations.
    userManager.users[uInd].channelPath = channelManager.channels[chInd].filePath;
    userManager.users[uInd].refreshPos = 0;
    userManager.users[uInd].channelInd = chInd;
    //resizes channel if needed.
    if(channelManager.channels[chInd].size == channelManager.channels[chInd].maxSize){
        channelManager.channels[chInd].members = realloc(channelManager.channels[chInd].members, sizeof(int)*(channelManager.channels[chInd].maxSize += 10));
    }
    channelManager.channels[chInd].members[channelManager.channels[chInd].size++] = uInd;
}
int checkAuthToken(char* authToken){
    //iterates through all users and if auth token is found the index is returned.
    for(int i = 0; i < userManager.size; i++){
        if(strcmp(userManager.users[i].authToken, authToken) == 0){return i;}
    }
    //if not found then return -1.
    return -1;
}
int checkChannelName(char* channelName){
    //iterates through all channels if found, returns the index.
    for(int i = 0; i < channelManager.size; i++){
        if(strcmp(channelManager.channels[i].name, channelName) == 0){return i;}
    }
    //if not found returns -1.
    return -1;
}
void sendMessage(char* message, char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //if user is not in a channel.
    if(userManager.users[uInd].channelPath == NULL){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not in a channel.");
        return;
    }
    //sending the message and writing it in the channel file.
    myJSON *msg = myJSON_CreateObject();
    myJSON_AddStringToObject(msg, "sender", userManager.users[uInd].username);
    myJSON_AddStringToObject(msg, "content", message);
    char* toWrite = myJSON_Print(msg);
    FILE *fp = fopen(userManager.users[uInd].channelPath , "a");
    fprintf(fp, "%s\n", toWrite);
    //memory leak.
    myJSON_Delete(msg);
    fclose(fp);
    free(toWrite);
    //filling the response.
    strcpy(response->type, "Successful");
    strcpy(response->stringContent, "message sent successfully.");
}
void refresh(char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //if user is not in a channel.
    if(userManager.users[uInd].channelPath == NULL){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not in a channel.");
        return;
    }
    strcpy(response->type, "List");
    //opens the file and reads line by line until it reaches the refreshing point.
    FILE *fp = fopen(userManager.users[uInd].channelPath, "r");
    myJSON *message;
    char *content = malloc(MAX);
    content[0] = 0;
    char contentYadak[MAX] = {};
    for(int i = 0; fgets(contentYadak, MAX, fp); i++){
        if(contentYadak[strlen(contentYadak) - 1] != '\n'){
            content = realloc(content, strlen(content) + MAX);
            strcat(content, contentYadak);
            continue;
        }
        else{
            strcat(content, contentYadak);
        }
        if(i == userManager.users[uInd].refreshPos++){
            message = myJSON_Parse(content);
            char *sender = myJSON_GetItemFromObject(message, "sender")->strContent;
            char *msg = myJSON_GetItemFromObject(message, "content")->strContent;
            addMessageToList(sender, msg, response);
            free(sender), free(msg);
            myJSON_Delete(message);
        }
        memset(content, 0, strlen(content));
        memset(contentYadak, 0, MAX);
    }
    //memory leak.
    fclose(fp);
    free(content);
}
void addMessageToList(char *sender, char *msg, RES* response){
    //create an object to hold the sender and message.
    myJSON* message = myJSON_CreateArray();
    myJSON_AddStringToObject(message, "sender", sender);
    myJSON_AddStringToObject(message,"content", msg);
    //adds the object to list.
    myJSON_AddItemToArray(response->listContent, message);
    //memory leak.
//    myJSON_Delete(message);
}
void listMembers(char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //if user is not in a channel.
    if(userManager.users[uInd].channelPath == NULL) {
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not in a channel.");
        return;
    }
    //fills the response with the members of the corresponding channel.
    strcpy(response->type, "List");
    int chInd = userManager.users[uInd].channelInd;
    for(int i = 0; i < channelManager.channels[chInd].size; i++){
        addMemberToList(userManager.users[channelManager.channels[chInd].members[i]].username, response);
    }
}
void addMemberToList(char* member, RES* response){
    //creates an object to hold the member
    myJSON* mem = myJSON_CreateString(member);
    //adds the object to list.
    myJSON_AddItemToArray(response->listContent, mem);
    //memory leak
//    myJSON_Delete(mem);
}
void leaveChannel(char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //if user is not in a channel.
    if(userManager.users[uInd].channelPath == NULL){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not in a channel.");
        return;
    }
    //writes the message in the channel.
    myJSON* msg = myJSON_CreateObject();
    char zapas[2 * MAX];
    strcpy(zapas, userManager.users[uInd].username);
    strcat(zapas, " left the channel.");
    myJSON_AddStringToObject(msg, "sender", "server");
    myJSON_AddStringToObject(msg, "content", zapas);
    char *toWrite = myJSON_Print(msg);
    FILE* fp = fopen(userManager.users[uInd].channelPath, "a");
    fprintf(fp, "%s\n", toWrite);
    //removes the user from channel and channel from user channel.
    rmUsFromChan(uInd, userManager.users[uInd].channelInd);
    userManager.users[uInd].refreshPos = 0;
    userManager.users[uInd].channelInd = -1;
    userManager.users[uInd].channelPath = 0;
    //filling the response.
    strcpy(response->type, "Successful");
    strcpy(response->stringContent, "user left the channel successfully.");
    //memory leak.
    fclose(fp);
    myJSON_Delete(msg);
    free(toWrite);
}
void rmUsFromChan(int uInd, int chInd){
    //finds the user in the channel.
    int ind = 0;
    while(strcmp(userManager.users[channelManager.channels[chInd].members[ind++]].authToken, userManager.users[uInd].authToken)){;}
    for(; ind < channelManager.channels[chInd].size; ind++){
        userManager.users[channelManager.channels[chInd].members[ind - 1]] = userManager.users[channelManager.channels[chInd].members[ind]];
    }
    channelManager.channels[chInd].size--;
    //reallocates the members pointer to prevent memory leak.
    if(channelManager.channels[chInd].maxSize - channelManager.channels[chInd].size >= 10){
        channelManager.channels[chInd].members = realloc(channelManager.channels[chInd].members, sizeof(int) * (channelManager.channels[chInd].maxSize-= 10));
    }
}
void logoutUser(char* authToken, RES* response){
    //if the authentication token is invalid.
    int uInd = checkAuthToken(authToken);
    if(uInd == -1){
        strcpy(response->type, "error");
        strcpy(response->stringContent, "User is not logged-in.");
        return;
    }
    //sets the auth token for the user to null.
    userManager.users[uInd].authToken[0] = 0;
    //fills the response.
    strcpy(response->type, "Successful");
    strcpy(response->stringContent, "user loged-out successfully.");
}