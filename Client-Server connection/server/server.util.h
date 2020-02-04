///functions and typedefs required for the server.
// Created by Dana on 1/16/2020.

#ifndef SERVER_SERVER_UTIL_H
#define SERVER_SERVER_UTIL_H
#endif //SERVER_SERVER_UTIL_H


#define AUTH_TOKEN_length  30
#define AL  AUTH_TOKEN_length
#define MAX 80

//typedefs.
/**
 * @brief a hand-crafted type to handle the responses that needs to be sent.
 * @field type(successful, error, list and AuthToken).
 * @field an array to store simple responses.
 * @field a pointer to a list of ordered pairs.
 * @field the number of items in the list.
 */
typedef struct Resp{
    char type[20];
    char stringContent[MAX];
    myJSON* listContent;
}RES;

//functions.
/**
 * adds the channel to the existing channels.
 * @param channelName
 * @param channelPath
 */
void addChannel(char* channelName, char* channelPath);
/**
 * adds the user to the existing users.
 * @param username
 * @param password
 */
void addUser(char* username, char* password);
/**
 * @brief sends the string content of response.
 * @param clientSocket
 * @param response
 */
void sendStringResponse(int clientSocket, RES response);
/**
 * @brief sends the list content of response.
 * @param clientSocket
 * @param response
 * @param flag indicates the protocol of refresh or list members.
 */
void sendListResponse(int clientSocket, RES response, int flag);
/**
 * @brief handles the inputs with the format: command firstArg, secondArg.
 * @param extFrom
 * @param arg1
 * @param arg2
 * @return 0 if extracted successfully -1 otherwise.
 */
int extrDiInput(char* extFrom, char* arg1, char* arg2);
/**
 * @brief handles the inputs with the format: command Arg.
 * @param extFrom
 * @param Arg
 * @return 0 if extracted successfully -1 otherwise.
 */
int extrMonoInput(char*extFrom, char* Arg);
/**
 * @brief since messages can have space and commas, a different method is required.
 * @param exfrom
 * @param msg
 * @param authToken
 * @return 0 if extracted successfully -1 otherwise.
 */
int msgExtr(char* exFrom, char** msg, char* authToken);
/**
 * @brief checks if a user with the given username already exist. if not creates the user.
 * also fills the third argument with the response to be sent.
 * @param username
 * @param password
 * @param response
 */
void handleSignUp(char* username, char* password, RES* response);
/**
 * @brief checks if a user with the given username already exist and checks its status.
 * if the user exists and is currently offline and the password is correct, the user is logged in.
 * the user is known to the server by a randomly generated authentication token.
 * also fills the third argument with the response to be sent.
 * @param username
 * @param password
 * @param response
*/
void handleLogIn(char* username, char* password, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * creates a channel with the given name if not existing.
 * also fills the third argument with the response to be sent.
 * @param channelName
 * @param authToken
 * @param response
 */
void createChannel(char* channelName, char* authToken, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * the user is joined to the channel if existing.
 * also fills the third argument with the response to be sent.
 * @param channelName
 * @param authToken
 * @param response
 */
void joinChannel(char* channelName, char* authToken, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * sends the message given to the channel currently in.
 * also fills the third argument with the response to be sent.
 * @param message
 * @param authToken
 * @param response
 */
void sendMessage(char* message, char* authToken, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * fills the second argument with all of the logs of the channel since the last refreshing.
 * @param authToken
 * @param response
 */
void refresh(char* authToken, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * fills the second argument with the list of currently online users of channel.
 * @param authToken
 * @param response
 */
void listMembers(char* authToken, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * the user will no longer be a part of the channel.
 * also fills the second argument with the response to be sent.
 * @param authToken
 * @param response
 */
void leaveChannel(char* authToken, RES* response);
/**
 * @brief validates the authentication token and upon validation,
 * the user is logged out and the authentication token is no longer valid.
 * also fills the second argument with the response to be sent.
 * @param authToken
 * @param response
 */
void logoutUser(char* authToken, RES* response);


