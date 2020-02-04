///Interface through which you can use the server.
// Created by Dana on 1/17/2020.

#ifndef SERVER_SERVER_INTERFACE_H
#define SERVER_SERVER_INTERFACE_H
#endif //SERVER_SERVER_INTERFACE_H

//functions
/**
 * @brief reads the text file containing channel names that are already created
 * and passes them to the channel manager.
 * @param path
 */
void initChannels(char* path);
/**
 * @brief reads the text file containing users that are already created
 * and passes them to the user manager.
 * @param path
 */
void initUsers(char* path);
/**
 * @brief creates a socket for the server to use.
 * @return socket created by the function.
 */
int createSocket();
/**
 * @brief puts server in listening status and accepts request from clients
 * and also handles those requests. basically the server will stay
 * in this function unless the process is terminated by force.
 * @param serverSocket
 * @return -1 if could not listen.
 */
int accepting(int serverSocket);