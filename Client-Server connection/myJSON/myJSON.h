//
// Created by Dana on 1/31/2020.
// This is my implementation of JSON for C!
// Here I actually implemented the functions I needed for my project.
// I used a different approach than the one used by Dave Gamble in cJSON.
// Instead of each object having a pointer to the next and previous object of it in the list,
// I left this to be handled by the root object as it makes more sense (at least for me! :D).
// The overall approach though is kinda similar, dividing JSON objects into three different types:
// primitive type holders, arrays and pair holders (for my use I only needed string from primitive types; It can be easily extended to hold others as well).
// Primitive type holder JSON holds either an int, double, string and... .
// Array JSON is used to store a number of un-named JSON objects.
// Pair holder JSON is used to store a number of named JSON objects.
// All three types may or may not have a name.
// This implementation may be very weak compare to what a real JSON parser should offer, but again the intention
// was to use the self implemented functions for JSON not implementing a parser!
// You can contact me via github: [insert ID here] or by gmail: danaafa1380@gmail.com .
// Thank you!


#ifndef MYJSON_MYJSON_H
#define MYJSON_MYJSON_H

#define MAX 80
/**
 * @brief A simple version of JSON for C. It is divided into four sections:
 * 1:
 * @field name It is used to store the object's name. It can be NULL.
 * 2:
 * @field strContent It is used to store a simple string in the JSON.
 * 3:
 * @field pairContent holds the named JSON objects.
 * @field pairSize holds the current size of the pairContent.
 * @field maxPairSize holds the maximum size of the pairContent.
 * 4:
 * @field arrayContent holds the un-named JSON objects.
 * @field arrSize holds the current size of the arrayContent.
 * @field maxArrSize holds the maximum size of the arrayContent.
 */
typedef struct myJSON{
    char* name;

    char* strContent;

    struct myJSON **pairContent;
    int pairSize;
    int maxPairSize;

    struct myJSON **arrContent;
    int arrSize;
    int maxArrSize;
}myJSON;

/**
 * @brief Creates a pair holder object.
 * @return returns a pointer to the object created.
 */
myJSON* myJSON_CreateObject();
/**
 * @brief Creates an array object.
 * @return returns a pointer to the object created.
 */
myJSON* myJSON_CreateArray();
/**
 * @brief Creates an object to hold the given string.
 * @param string
 * @return returns a pointer to the object created.
 */
myJSON* myJSON_CreateString(char* string);
/**
 * @brief creates a copy object of the parameter and returns it.
 * @param object the object to copy from.
 * @return returns a pointer to the created object.
 */
myJSON* myJSON_CreateCopy(myJSON* object);
/**
 * @brief only de-allocates the pairContent.
 * @param toDelete object to be deleted.
 */
void myJSON_DeletePairContent(myJSON* toDelete);
/**
 * @brief only de-allocates the arrContent.
 * @param toDelete object to be deleted.
 */
void myJSON_DeleteArrContent(myJSON* toDelete);
/**
 * @brief de-allocates and deletes the object pointed by the parameter and its attachments.
 * @param toDelete
 */
void myJSON_Delete(myJSON* toDelete);
/**
 * @brief only copies the pairContent.
 * @param dest the destination object.
 * @param source the source object.
 */
void myJSON_CopyPairContent(myJSON* dest, myJSON* source);
/**
 * @brief only copies the arrContent.
 * @param dest the destination object.
 * @param source the source object.
 */
void myJSON_CopyArrContent(myJSON* dest, myJSON* source);
/**
 * @brief copies the source object to the destination object.
 * @param dest the destination object.
 * @param source the source object.
 */
void myJSON_CopyObjectToObject(myJSON* dest, myJSON* source);
/**
 * @brief adds the string with the name given to a pair holder.
 * @param object the pair holder
 * @param name the name given to the string
 * @param string the string to be added.
 */
void myJSON_AddStringToObject(myJSON* object, char* name, char* string);
/**
 * @brief adds the string to the array object.
 * @param array array object.
 * @param string the string to be added.
 */
void myJSON_AddStringToArray(myJSON* array, char* string);
/**
 * @brief adds an object with a given name to a pair holder. then de-allocates the item.
 * @param object the pair holder.
 * @param name the name given to the object.
 * @param item the object to be added.
 */
void myJSON_AddItemToObject(myJSON* object, char* name, myJSON* item);
/**
 * @brief adds an object to an array object. then de-allocates the item.
 * @param array the array object.
 * @param item the object to be added.
 */
void myJSON_AddItemToArray(myJSON* array, myJSON* item);
/**
 * @brief searches in an pair holder by a name and returns a pointer to the object.
 * @param object the pair holder.
 * @param name the name used to find the pair.
 * @return a pointer to the found object and if it didnt exist, returns NULL.
 */
myJSON* myJSON_GetItemFromObject(myJSON* object, char* name);
/**
 * @brief returns a pointer to an object with the given index in a pair holder.
 * @param object the pair holder.
 * @param index the index of the object to be returned.
 * @return a pointer to the found object and if the index exeeds the size, returns NULL.
 */
//myJSON* myJSON_GetItemFromObject(myJSON* object, int index);
/**
 * @brief returns a pointer to an object with the given index in an array object.
 * @param array the array object.
 * @param index the index of the object to be returned.
 * @return a pointer to the found object and if the index exeeds the size, returns NULL.
 */
myJSON* myJSON_GetItemFromArray(myJSON* array, int index);
/**
 * @brief returns the current size of an array object
 * @param array the array object.
 * @return the current size.
 */
int myJSON_GetArraySize(myJSON* array);
/**
 * @brief returns the current size of a pair holder object.
 * @param object the pair holder.
 * @return the current size.
 */
int myJSON_GetObjectSize(myJSON* object);
/**
 * @brief creates a string containing the object in JSON format.
 * @param object the object to be printed.
 * @return the string created.
 */
char* myJSON_Print(myJSON* object);
/**
 * @brief prints the given object to the buffer. if buffer does not have enough size (indicated by size parameter) nothing happens.
 * @param buffer the pre-allocated string.
 * @param size the maximum amount of bytes to print in.
 * @param object the object to be printed.
 * @return -1 if size is insufficient and 0 otherwise.
 */
int myJSON_PrintPreAllocated(char* buffer, int size, myJSON* object);
/**
 * @brief parses the string given and creates an object based on it.
 * @param toParse the string to parse.
 * @return a pointer to the object created. if the format is not JSON, returns NULL.
 */
myJSON* myJSON_Parse(char* toParse);
myJSON* myJSON_ParseObject(char* toParse, int* skip);
myJSON* myJSON_ParseArray(char* toParse, int* skip);
myJSON* myJSON_ParseString(char* toParse, int* skip);
myJSON* extractNext(char* iterator, int* jump);



#endif //MYJSON_MYJSON_H
