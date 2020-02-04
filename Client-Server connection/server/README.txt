 this is the Afagram's server!
 the server is designed to have a stateless connection with the clients and is able to create chat rooms and allows people to chat in them.
 the server is implemented through an interface and a utility library that holds all of the operations that the server must do.
 server saves the users and channels info in different txt files and upon booting up, it reads the info from those files and
 loads them into the program through which it can be edited.
 we go through all of the files and explain what each one does.
-------------------------------------------------------------------------------------------------------------------------------------------------
 main.c:
	in here we initialize the server. with initServer() we make a Resources directory if not made already.
	we also make to sub-directories Channels and Users which allows us to store their respective data as well.
	furthermore, the function loads the data of users and channels if the text files are not empty.
	other than that, we also make a server socket and hand over the control to the accepting().
 server.interface.h/c:
	in these two files we implemented an interface for the server so that the main function can use server by them.
	it is consist of four functions:
		* initChannels: which we use to load the pre-written data of channels from files.
		* initUsers: which we use to load the pre-written data of users from files.
		* createSocket: it uses the given codes to make a server socket and returns its value.
		* accepting: puts the server in the accepting mode by which it can accept requests.
 server.util.h/c:
	here is where the main funtions of the server are implemented and structs for users and channels are made.
	first, lets explain structs:
		* USER: a user has a username, password, authentication token (null if not loged-in), the path to the channel its currently in (null if none)
			the index of that channel in the channelManager (explained below, also -1 if none), and the refresh position of that user in the channel (-1 if none).
		* userManager: its a struct that essentially hold an array of users. it uses dynamic allocatin so it has to field for its size and max size.
		* CHAN: a channel has a name, a path to its file, an array of members (it holds the index of members in the userManager). and again since its dynamic allocation,
			it holds a size and a max size for the array
		* channelManager: just like userManager, channelManager holds an array of channels.
	there is also a struct for holding the response. this struct is used to get the result and response of each function and is sent trough specific functions:
		* RES: it holds an array for its type (successful, error or list), an array to hold the string contents and a JSON to hold complex and list responses
			(although a single JSON would be sufficient since it can hold string content as well).
	there are quite a few functions in this library and here is the list and a brief:
		* addUser, addChannel: also used in initChannel and initServer, used to add a channel or user to channelManager or userManager.
		* sendStringResponse, sendListResponse: send the respective part of the response for the client.
		* extrDiInput: some requests sent by the user have to arguments (for example login dana, afa), this function extracts them if they math the pattern.
			      if not returns -1.
		* extrMonoInput: some requests have one argument (logout *authToken*), this function extracts them if the match the pattern. if not returns -1.
		* msgExtr: for the request send, since the message can have some characters and patters which other fields cannot, we wrote a different function
			  for this request. returns 0 upon completen and -1 otherwise.
		* handleSignUp: handles the request register *username*, *password* and fills the response correspondingly. wont register if username is taken.
		* handleLogIn: handles the request login *username*, *password* and fills the response correspondinlgy. wont log in if usrename or password are incorrect.
				if loged in, creates an authentication token.
		* createChannel: handles the request create channel *channelName*, *authToken* and fills the response. wont create if the name is already taken or the user doesnt exist.
		* joinChannel: handles the request join channel *channelName*, *authToken* and fills the response. wont join if user or channel doestn exist.
		* sendMessage: handles the request send *message*, *authToken* and fills the response. wont send if user doesnt exist or is not in a channel.
		* refresh: handles the request refresh *authToken* and fills the response. updates the refresh position of the user if successful.
				wont work if user doesnt exist or is not in a channel.
		* listMembers: handles the request channel members *authToken* and fills the response. wont do if user doesnt exist or is not in a channel.
		* leaveChannel: handles the request leave *authToken* and fills the response. wont do if user doesnt exist or is not in a channel.
		* logoutUser: handles the request logout *authToken* and fills the response. wont do if user doesnt exist.
	there are some functions that are only used by the above functions and are not ment to be used by interface and therefore not in the header:
		* checkUsername: finds the user in the userManager and returns the index (in userManager). if user doesnt exist returns -1.
		* checkAuthToken: validates the auth token given to it and returns the index (in userManager) if found and -1 if not.
		* checkChannelName: validates the channel name given to it and returns the index (in channelManager) if found and -1 if not.
		* writeUser: writes the user's data (username and password) in the users file.
		* createToken: creates a 29 charactar string consist of numbers and lower and upper case letters.
		* addUserToChannel: if joinChannel is successful, it calls this function to add the user to the channel. it writes channel info in user struct and vice versa.
		* addMessageToList: adds the message to the listContent of response. used by refresh to gather the messages needed to send.
		* addMemberToList: adds the member to the listContent of response. used by listMembers to gather members to send.
		* rmUsFromChan: called by leaveChannel, removes the user from channel and vice versa.