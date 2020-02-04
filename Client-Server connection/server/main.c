#include <stdio.h>
#include <dir.h>
#include <dirent.h>
#include "server.interface.h"

void initServer(){
    //makes the Resources directory if already doesnt exist.
    mkdir("Resources");
    //creates the sub directories of Resources if needed.
    mkdir("Resources\\Channels"), mkdir("Resources\\Users");
    //checks if there already exists Channels and Users data.
    DIR *dr;
    //Channels:
    dr = opendir("Resources\\Channels");
    //getting rid of . and ..
    readdir(dr), readdir(dr);
    if(readdir(dr) != NULL){
        initChannels("Resources\\Channels");
    }
    //Users:
    dr = opendir("Resources\\Users");
    //getting rid of . and ..
    readdir(dr), readdir(dr);
    if(readdir(dr) == NULL){fclose(fopen("Resources\\Users\\users.txt", "w"));}
    else{initUsers("Resources\\Users\\users.txt");}
    closedir(dr);
}

// Driver function
int main() {
    printf("hello! This is Afagram's server!\nYou can have a look but PLEASE DON'T TOUCH ANYTHING!!\nThanks! ;)\n");
    for(int i = 0; i < 80; i++){
        printf("-");
    }
    //initializes the users and channels.
    initServer();

    printf("\nCreating the socket...\n");
    int serverSocket;
    if((serverSocket = createSocket()) == -1){
        printf("Socket creation failed successfully!\nYears of academy training wasted...\nHave a nice day!\n");
        return -1;
    }
    accepting(serverSocket);
}