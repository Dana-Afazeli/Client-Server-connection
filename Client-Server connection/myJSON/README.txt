 Created by Dana on 1/31/2020.
 This is my implementation of JSON for C!
 Here I actually implemented the functions I needed for my project.
 I used a different approach than the one used by Dave Gamble in cJSON.
 Instead of each object having a pointer to the next and previous object of it in the list,
 I left this to be handled by the root object as it makes more sense (at least for me! :D).
 The overall approach though is kinda similar, dividing JSON objects into three different types:
 primitive type holders, arrays and pair holders (for my use I only needed string from primitive types; It can be easily extended to hold others as well).
 Primitive type holder JSON holds either an int, double, string and... .
 Array JSON is used to store a number of un-named JSON objects.
 Pair holder JSON is used to store a number of named JSON objects.
 All three types may or may not have a name.
 This implementation may be very weak compare to what a real JSON parser should offer, but again the intention
 was to use the self implemented functions for JSON not implementing a parser!
 You can contact me via github: [insert ID here] or by gmail: danaafa1380@gmail.com .
 Thank you!
-----------------------------------------------------------------------------------------------------------------------------------------
exact explanation of each function is provided in the header file. so here we take an overall look to see what does this library does:

*object creating functions:
	there are 3 functions that allocate memory for a JSON object and return a pointer to it:
		* myJSON_CreateObject: which creates an object that holds pair contents.
		* myJSON_CreateArray:  which creates an object that holds an array of objects.
		* myJSON_CreateString: which creates an object that is essentially an un-named string.
	there is an additional function for creating an exact copy of another object.
		* myJSON_CreateCopy
*de-allocating functions:
	there is actually one main de-allocating function:
		* myJSON_Delete: which de-allocates an objects contents and deletes it. it also deletes the children of the object.
	but there are 2 tool functions that are used by this function which can be used alone:
		* myJSON_DeletePairContent: which only de-allocates the pair content pointer and the objects pair children.
		* myJSON_DeleteArrContent: this function only de-allocates the array content and its chideren of the object.
*copy functions:
	there is a function that copies one object to another regardless of their types (string, array or pair holder):
		* myJSON_CopyObjectToObject: worth mentioning that useless but previously allocated memory in the destination object will be deleted.
	this function also uses 2 functions that can be used alone:
		* myJSON_CopyPairContent: only copies the pair content and its belongings.
		* myJSON_CopyArrContent:  only copies the array content and its belongings.
*addition functions:
	there are 4 functions provided to add any kind of object or string to any kind of object (ofcourse adding a string to a string object is somewhat redundant):
		* myJSON_AddStringToObject: since its adding it to a pair holder it also need a name for it.
		* myJSON_AddStringToArray: unlike previous one, it doesnt require a name for the string.
		* myJSON_AddItemToObject: adds the item (which is an object) to the source, the name provided over-writes the previous name if its not null.
		* myJSON_AddItemToArray: adds the item to the source which is an array hence not needing any name.
*get functions:
	here are functions used to get different items or sizes from an object:
		* myJSON_GetItemFromObject: there are two implementation of this function: one searches based on the name given to the function and other by the index of the item.
		* myJSON_GetItemFromArray: returns a pointer to the object pointed by the index given in the array list.
		* myJSON_GetArraySize: returns the current size for an array container.
		* myJSON_GetObjectSize: returns the current size for a pair container.
*print functions:
	functions used to print an object in a string:
		* myJSON_Print: prints in un-formatted type the object given and returns a pointer to the result.
		* myJSON_PrintPreAllocated: it takes a buffer and available size in the buffer and if its sufficient, prints the object in it and returns 0, otherwise nothing happens and returns -1.
*parse functions:
	functions used to parse a string to an object: we have to note that hre we had to put some limitations for the user, for example user should not use " in the name or content of any of their objects.
		* myJSON_Parse: based on our need we only assumed that the inputs are pair holders, but this can easily be fixed. returns null if pattern didnt mathch.
		* myJSON_ParseObject: parse the string based on the format of a pair holder, if didnt match, returns null.
		* myJSON_ParseArray: parse the string based on the format of an array, if didnt match, returns null.
		* myJSON_ParseString: parse the string based on teh format of a string, if didnt mathc, returns null.
