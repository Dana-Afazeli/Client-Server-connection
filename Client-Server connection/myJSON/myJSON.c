#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "myJSON.h"

myJSON* myJSON_CreateObject(){
    //creates and initializes the object.
    myJSON* obj = malloc(sizeof(myJSON));
    //unused part.
    obj->name = obj->strContent = NULL;
    obj->arrContent = (myJSON**)NULL;
    obj->arrSize = obj->maxArrSize = 0;
    //main part.
    obj->pairContent = malloc(sizeof(myJSON*));
    obj->maxPairSize = 1;
    obj->pairSize = 0;
    //return the object.
    return obj;
}
myJSON* myJSON_CreateArray(){
    //creates and initializes the object.
    myJSON* arr = malloc(sizeof(myJSON));
    //unused part.
    arr->name = arr->strContent = NULL;
    arr->pairContent = (myJSON**)NULL;
    arr->pairSize = arr->maxPairSize = 0;
    //main part.
    arr->arrContent = malloc(sizeof(myJSON*));
    arr->maxArrSize = 1;
    arr->arrSize = 0;
    //return the object.
    return arr;
}
myJSON* myJSON_CreateString(char* string){
    //creates and initializes the object.
    myJSON* str = malloc(sizeof(myJSON));
    //unused part.
    str->name = NULL;
    str->pairContent = str->arrContent = (myJSON**)NULL;
    str->arrSize = str->maxArrSize = str->pairSize = str->maxPairSize = 0;
    //main part.
    str->strContent = malloc(strlen(string) + 1);
    strcpy(str->strContent, string);
    //returns teh object.
    return str;
}
myJSON* myJSON_CreateCopy(myJSON* object){
    //creates the object.
    myJSON* obj = myJSON_CreateObject();
    myJSON_CopyObjectToObject(obj, object);
    //return the object.
    return obj;
}
void myJSON_DeletePairContent(myJSON* toDelete){
    if(toDelete->pairContent != NULL){
        for(int i = 0; i < toDelete->pairSize; i++){
            myJSON_Delete(toDelete->pairContent[i]);
        }
        free(toDelete->pairContent);
        toDelete->pairContent = NULL;
        toDelete->pairSize = toDelete->maxPairSize = 0;
    }
}
void myJSON_DeleteArrContent(myJSON* toDelete){
    if(toDelete->arrContent != NULL){
        for(int i = 0; i < toDelete->arrSize; i++){
            myJSON_Delete(toDelete->arrContent[i]);
        }
        free(toDelete->arrContent);
        toDelete->arrContent = NULL;
        toDelete->arrSize = toDelete->maxArrSize = 0;
    }
}
void myJSON_Delete(myJSON* toDelete){
    //deallocates the name and strContent.
    free(toDelete->name), free(toDelete->strContent);
    //deallocates all children in pairContent and arrContent.
    myJSON_DeletePairContent(toDelete);
    myJSON_DeleteArrContent(toDelete);
    //free s the pointer itself.
    free(toDelete);
}
void myJSON_CopyPairContent(myJSON* dest, myJSON* source){
    //deallocates first.
    myJSON_DeletePairContent(dest);
    if(source->pairContent != (myJSON**)NULL) {
        //copies the items.
        dest->pairSize = source->pairSize;
        dest->maxPairSize = source->maxPairSize;
        dest->pairContent = malloc(dest->maxPairSize * sizeof(myJSON *));
        for (int i = 0; i < dest->pairSize; i++) {
            dest->pairContent[i] = myJSON_CreateObject();
            myJSON_CopyObjectToObject(dest->pairContent[i], source->pairContent[i]);
        }
    }
    else{ dest->pairContent = (myJSON**)NULL;}
}
void myJSON_CopyArrContent(myJSON* dest, myJSON* source){
    //deallocates first.
    myJSON_DeleteArrContent(dest);
    if(source->arrContent != (myJSON**)NULL) {
        //copies the items.
        dest->arrSize = source->arrSize;
        dest->maxArrSize = source->maxArrSize;
        dest->arrContent = malloc(dest->maxArrSize * sizeof(myJSON *));
        for (int i = 0; i < dest->arrSize; i++) {
            dest->arrContent[i] = myJSON_CreateObject();
            myJSON_CopyObjectToObject(dest->arrContent[i], source->arrContent[i]);
        }
    }
    else{ dest->arrContent = (myJSON**)NULL;}
}
void myJSON_CopyObjectToObject(myJSON* dest, myJSON* source){
    //copies the name and strContent.
        //deallocates the dest name and dest strContent first.
    free(dest->name), free(dest->strContent);
    if(source->name != NULL){
        dest->name = malloc(strlen(source->name) + 1), strcpy(dest->name, source->name);
    }
    else{ dest->name = NULL; }
    if(source->strContent != NULL){
        dest->strContent = malloc(strlen(source->strContent) + 1), strcpy(dest->strContent, source->strContent);
    }
    else{ dest->strContent = NULL; }
    //copies the pairContent and arrContent.
    myJSON_CopyPairContent(dest, source);
    myJSON_CopyArrContent(dest, source);
}
void myJSON_AddStringToObject(myJSON* object, char* name, char* string){
    //resize if needed.
    if(object->pairSize == object->maxPairSize){
        object->pairContent = realloc(object->pairContent, (object->maxPairSize *= 2)* sizeof(myJSON*));
    }
    //adds the object.
    object->pairContent[object->pairSize] = myJSON_CreateString(string);
    object->pairContent[object->pairSize]->name = malloc(strlen(name) + 1);
    strcpy(object->pairContent[object->pairSize]->name, name);
    object->pairSize++;
}
void myJSON_AddStringToArray(myJSON* array, char* string){
    //resize if needed.
    if(array->arrSize == array->maxArrSize){
        array->arrContent = realloc(array->arrContent, (array->maxArrSize *= 2) * sizeof(myJSON*));
    }
    //adds the object.
    array->arrContent[array->arrSize] = myJSON_CreateString(string);
    array->arrSize++;
}
void myJSON_AddItemToObject(myJSON* object, char* name, myJSON* item){
    //resize if needed.
    if(object->pairSize == object->maxPairSize){
        object->pairContent = realloc(object->pairContent, (object->maxPairSize *= 2)* sizeof(myJSON*));
    }
    //adds the object.
    object->pairContent[object->pairSize] = myJSON_CreateCopy(item);
    if(name != NULL) {
        object->pairContent[object->pairSize]->name = malloc(strlen(name) + 1);
        strcpy(object->pairContent[object->pairSize]->name, name);
    }
    object->pairSize++;
    //deallocate the item
    myJSON_Delete(item);
}
void myJSON_AddItemToArray(myJSON* array, myJSON* item){
    //resize if needed.
    if(array->arrSize == array->maxArrSize){
        array->arrContent = realloc(array->arrContent, (array->maxArrSize *= 2)* sizeof(myJSON*));
    }
    //adds the object.
    array->arrContent[array->arrSize] = myJSON_CreateCopy(item);
    array->arrSize++;
    //deallocate the item
    myJSON_Delete(item);
}
myJSON* myJSON_GetItemFromObject(myJSON* object, char* name){
    //iterate through the pairContent.
    for(int i = 0; i < object->pairSize; i++){
        if(strcmp(name, object->pairContent[i]->name) == 0){ return object->pairContent[i]; }
    }
    //if not found.
    return NULL;
}
//myJSON* myJSON_GetItemFromObject(myJSON* object, int index){
//    //if index exeeds the boundaries.
//    if(index >= object->pairSize){ return NULL; }
//    else{ return object->pairContent[index]; }
//}
myJSON* myJSON_GetItemFromArray(myJSON* array, int index){
    //if index exeeds the boundaries.
    if(index >= array->arrSize){ return NULL; }
    else{ return array->arrContent[index]; }
}
int myJSON_GetArraySize(myJSON* array){
    return array->arrSize;
}
int myJSON_GetObjectSize(myJSON* object){
    return object->pairSize;
}
char* myJSON_Print(myJSON* object){
    //making buffer.
    char* buffer = calloc(MAX, sizeof(char));
    int size = MAX;
    //since the print function is called, it is guaranteed that the name is null.
    //and with our use it is guaranteed that when calling print, we call a pair holder.
    buffer[0]= '{';
    for(int i = 0; i < object->pairSize; i++){
        //if resize is needed.
        while(myJSON_PrintPreAllocated(buffer, size - strlen(buffer) - 10, object->pairContent[i]) == -1){
            buffer = realloc(buffer, (size += MAX));
        }
        //prints comma if needed.
        if(i != object->pairSize - 1){ buffer[strlen(buffer) + 1] = '\0'; buffer[strlen(buffer)] = ',';}
    }
    buffer[strlen(buffer) + 1] = '\0'; buffer[strlen(buffer)] = '}';
    return buffer;
}
int myJSON_PrintPreAllocated(char* buffer, int size, myJSON* object){
    //makes a temporary string to contain the object.
    char* temp = calloc(MAX, sizeof(char));
    int tempSize = MAX;
    //prints the object into temp.
    if(object->name != NULL) {
        temp[0] = '\"';
        while (strlen(object->name) > tempSize - 4) { temp = realloc(temp, (tempSize += MAX)); }
        strcat(temp, object->name), strcat(temp, "\":");
    }
    //seeing what the content is.
    if(object->strContent != NULL){
        while(strlen(object->strContent) > tempSize - strlen(temp) - 7){  temp = realloc(temp, (tempSize += MAX)); }
        strcat(temp, "\""), strcat(temp, object->strContent), strcat(temp, "\"");
    }
    else if(object->pairContent != NULL){
        strcat(temp, "{");
        for(int i = 0; i < object->pairSize; i++){
            while(myJSON_PrintPreAllocated(temp, tempSize - strlen(temp) - 10, object->pairContent[i]) == -1){
                temp = realloc(temp, (tempSize += MAX));
            }
            //prints comma if needed.
            if(i != object->pairSize - 1){ temp[strlen(temp) + 1] = '\0'; temp[strlen(temp)] = ',';}
        }
        strcat(temp, "}");
    }
    else if(object->arrContent != NULL){
        strcat(temp, "[");
        for(int i = 0; i < object->arrSize; i++){
            while(myJSON_PrintPreAllocated(temp, tempSize - strlen(temp) - 10, object->arrContent[i]) == -1){
                temp = realloc(temp, (tempSize += MAX));
            }
            //prints comma if needed.
            if(i != object->arrSize - 1){ temp[strlen(temp) + 1] = '\0'; temp[strlen(temp)] = ',';}
        }
        strcat(temp, "]");
    }
    if(strlen(temp) >= size){ return -1; }
    else{ strcat(buffer, temp); return 0; }
}
myJSON* myJSON_Parse(char* toParse){
    //we only need to parse pair object.
    int dummy;
    myJSON* obj =  myJSON_ParseObject(toParse, &dummy);
    return obj;
}
myJSON* myJSON_ParseObject(char* toParse, int* skip){
    char* iterator = toParse;
    //primitive checking.
    if(iterator[0] != '{' || iterator[1] != '\"'){ return NULL; }
    //extracting the object.
    myJSON* parsed = myJSON_CreateObject();
    while(iterator[0] != '}'){
        int jump = 0;
        char *name = malloc(MAX);
        int size = MAX;
        int length = (int)(strchr(iterator + 2, '\"') - (iterator + 2));
        //resize if needed.
        while (size < length + 2) {
            name = realloc(name, (size += MAX));
        }
        //extracts the name.
        strncpy(name, iterator + 2, length);
        name[length] = '\0';
        //extracts the content.
        iterator += length + 3;
        if(iterator[0] != ':'){ return NULL; }
        myJSON* content;
        content = extractNext( iterator, &jump);
        iterator += jump;
        if (content == NULL) { return NULL; }
        //adds the content.
        myJSON_AddItemToObject(parsed, name, content);
        //updating the iterator.
        free(name);
    }
    //return the object.
    *skip = (int)(iterator - toParse) + 2;
    return parsed;
}
myJSON* myJSON_ParseArray(char* toParse, int* skip){
    char* iterator = toParse;
    //primitive checking.
    if(iterator[0] != '['){ return NULL; }
    //extracting the object.
    myJSON* parsed = myJSON_CreateArray();
    while(iterator[0] != ']') {
        int jump = 0;
        //extracts the content.
        myJSON *content;
        content = extractNext( iterator, &jump);
        iterator += jump;
        if (content == NULL) { return NULL; }
        //adds the content.
        myJSON_AddItemToArray(parsed, content);
    }
    //return the object.
    *skip = (int)(iterator - toParse) + 2;
    return parsed;
}
myJSON* myJSON_ParseString(char* toParse, int* skip){
    myJSON* parsed;
    char *string = malloc(MAX);
    int size = MAX;
    if(toParse[0] != '\"') { return NULL;}
    int length = (int)(strchr(toParse + 1, '\"') - (toParse + 1));
    //resize if needed.
    while (size < length + 2) {
        string = realloc(string, (size += MAX));
    }
    //extracts the name.
    strncpy(string,toParse + 1, length);
    string[length] = '\0';
    //returning the object.
    parsed = myJSON_CreateString(string);
    *skip = strlen(string) + 3;
    free(string);
    return  parsed;
}
myJSON* extractNext(char* iterator, int* jump){
    if (iterator[1] == '\"') {
        return myJSON_ParseString(iterator + 1, jump);
    } else if (iterator[1] == '{') {
        return myJSON_ParseObject(iterator + 1, jump);
    } else if (iterator[1] == '[') {
        return myJSON_ParseArray(iterator + 1, jump);
    } else {
        //if it doesnt match the pattern.
        return NULL;
    }
}




