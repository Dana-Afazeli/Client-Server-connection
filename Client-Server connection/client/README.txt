 this is Afagram's client!
 here I implemented an interface for the user to connect to the server and communicate.
-----------------------------------------------------------------------------------------------
 the interface is consist of 3 menu s: first menu, main menu and chat room or chat menu:
 first menu:
	* we enter this menu immediately after the program starts with first_menu() function.
	* in first menu we can sign up or log in.
	* sign_up(): its the sign up menu. it takes the username and input and sends the request to the server
		 	if sign up is successful, you automatically enter log in menu. if not, you stay here
			until you enter back.
	* log_in(): takes the username and password and sends the request to server and if the response is
			successful, stores the authentication token returned by the server. else, stays there
			until you enter back.
	*us_pass_input(): called by sign_up and log_in to take username and password. you can enter backspace
			and edit username and password, also by pressing back in password, you go to username again.
			also when entering password, the characters are shown as asterisks (*).
	* if first_menu return -1 then it means client entered exit and so the program terminates. else, we enter main menu
 main menu:
	* here you can do one of these things:
	* create_channel(): takes a name and sends the request to the server. if successful, you are automatically added to the channel
	 		if not, you stay there until you enter back.
	* join_channel(): takes a name and sends the request to the server. if successful, user is joined to the channel and you to the chat room.
	* channel_input(): used to input channel name for create and join channel. it supports backspace and entering back will result in going back.
	* logout(): doesnt take any additional thing and sends the request to the server. if successful, the you go back to the first menu.
chat room:
	* in here you can do 4 actions:
	* send_message(): takes a string and creates the request and sends it to the server.
	* refresh(): with this function you can see all of the new messages since your last refresh.
	* list_members(): with this function you will recieve a list of all members currently in the channel.
	* leave_channel(): with this you can leave the channel. if successful, you return to main menu.
there are some utility functions:
	* connect_to_server(): uses pre-written code to create a client socket and returns it.
	* send_request(): in this function we first create a socket with connect_to_server and 
			then sends the request passed to it. and then it recieves the response
			and interprets it.
	* us_pass_concat(): creates the request to be sent for login and register.
	* one_argument_concat(): creates the request for functions that need one argument.
	* show_json(): this function is used to print messages for refresh.
	* print_members(): it is used to show members for channel members request.

	 
