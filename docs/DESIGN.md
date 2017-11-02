# CS50 Final Project 
### DESIGN.md - design spec for KRAG
##### Group Eleph, April 2017

### Summary of Major design decisions

##### Common

Most of our server-side code will be found in the common directory. See Common for more details.

#####  Function tables for OpCode Handling

We will be implementing function tables to handle messages based on their OpCodes. A master function will take the message and search through the table with the message's opCode to return the appropriate function pointer to handle messages with that opCode. Then, the master function will call that appropriate function pointer.

##### Message_t struct

We will be implemting a struct to store messages -- all possible message fields will be created and initialized as NULL (or 0) in the struct and will be filled in when a message is built or read from the socket. 

##### Game_stats Struct

We will be implementing a Game_stats struct which itself contains a set of Team_stats structs each with their own set of FA_stats structs. Theserver will have the master Game_stats structs which it will update based on incoming messages from the Agents, and which it will use when it needs to send status updates to players.

##### Hashtable of {kragID, krag_t) pairs

The Krags will be stored in a hashtable with the kragID as the key and krag structs (each with a latitude, longitude and hint) as the items. 

### Common Directory

This directory holds all of the code that will be common to various components of the game. It includes the following programs:

* `socket_handler.c`
* `parser.c`
* `message.c`
* `game_stats.c`

##### socket_handler.c

This program handles all socket-related code, i.e. sending and receiving messages. It will have the following functionality:

* it will have an `unpack_message()` function that will translate a datagram into a message struct
* it will have a `pack_message()` function that will take in a message struct and output a message to a designated recipient
* it will handle logging - with inbound and outbound messages

##### parser.c

This program handles the nitty-gritty of parsing messages - it looks for pipe symbols and inputs the fields into a message struct.

##### message.c

This program defines the message_t struct, which will have a variable for each possible message field. It will also have getters for each of these variables.

##### game_stats.c

This program defines a game_stats_t struct. It will hold the following information:

* gameID
* elapsed time since start of game, in seconds;
* number of krags;
* number of agents (players);
* number of teams;
* Set of teams (key is name of team; item is a team_stats_t struct that holds…)
    * guideID
    * Guide player name
    * Last time GA contacted game server
    * number of krags claimed;
    * partly-revealed secret string.
    * Set of Field agents (key is pebbleID, item is a field_stats_t struct that holds…)
        * Name
        * Location
        * Team name
        * Last time FA contacted game server

It will also have various functions that relate to the information in this struct, allowing other programs to get the struct's information or update the struct's information.

### Game Server

##### User Interface

./gameserver gameId=... kff=... sf=... port=...
1. gameId provides the hexadecimal ID number of this game
2. kff provides the path to the krag file, which contains coordinates and clues for each krag
3. sf provides the path to the secret file, which contains a single line of text representing the secret.
4. port provides the port number of the Game Server

##### Inputs and Outputs

The only stdin from user is to tell the program to end. Input is in form of messages from the field agent/guide agent through sockets.

Textual output to stdout:
1. elapsed time since start of game, in seconds
2. number of krags
3. number of agents (players)
4. number of teams
5. for each team:
..1. name of its Guide Agent player
..2. name and location of each Field Agent player
..3. number of krags claimed
..4. partly-revealed secret string

##### Functional Decomposition into Modules

Uses files from common directory

`gameserver.c`
1. read_krags - Read a list of krags from a file
   1a. Puts clues in a bag
   1b. Puts krags in hashtable as keys where items are struct holding location and clue
2. create_socket - Create a DGRAM socket and bind it to the given port number
3. parse_args - just parse command line arguments
4. read_secret - Read the secret from a file

`opCode_handler.c`
1. Function table struct
2. Bunch of functions for each opCode in table    

##### Dataflow through Modules

1. Main calls to parse_args
2. Parse_args parses and validates the command line arguments and returns to main
3. Main initializes bag and hashtable
4. Main calls to read_krags
5. Read_krags puts clues into bag and krags into hashtable then returns to main
6. Main calls DGRAM_make
7. DGRAM_make creates a DGRAM socket and binds and returns to main
8. Main listens at the socket for input until the game is over and sends any input to socket_handler.c
9. socket_handler.c makes a message struct ptr and sends it to parser.c
10. Parser.c validates the input and puts it into a message struct then returns a message struct ptr back to socket_handler.c
11. Socket_handler.c packs a message into a datagram and sends it out of the socket or unpacks a datagram into a message struct and returns the struct ptr back to main 
12. Main sends the struct ptr to opcode_handler.c 
13. Opcode_handler.c performs the specific function and returns back to main
14. Main returns to step 8 until game over and then does any final clean up

##### Major Data Structures

List of krags
List of bag of clues (one for each team)
Function table (hashtable)

##### Structs

Program	utilizes all of	the structs described in common

##### Psuedo code

1. Parse args
2. Read krags from files
3. Initialize game stats struct
4. Create DGRAM socket
5. Listen at socket until game over
6. For each incoming message:
7. Initialize message struct for message
8. Get opCode from message struct and find function in function table 
9. Clean up

##### Testing Plan

1. Unit testing on parse and socket - creates and validates messages  
2. Test opcodes and functiontable - handles messages correctly
3. Then integration test for server having someone pass in commands using udp protocol mimicking sending messages to and from a FA and a GA

### Guide Agent

##### User Interface

./guideagent guideId=... team=... player=... host=... port=...
1. guideId provides the hexadecimal ID number for this player;
2. Team provides the name of the team to which the agent belongs;
3. player provides the name of the guide agent;
4. host provides the host name of the Game Server (e.g., flume.cs.dartmouth.edu);
5. port provides the port number of the Game Server.

##### Inputs and Output

Input is in form of messages from the field agent/game server.
Outputs messages to server/field agent, visual interface to user

##### Functional decomposition into modules

`guideagent.c`
1. Parse_args() - handle command-line arguments
2. Build_interface() - make the ncurses user interface
3. Create_socket() - create a DGRAM (UDP) socket

`Opcode_handler.c`
1. Function table struct
2. Functions for each opCode in table

##### Dataflow through Modules

1. Main calls to parse_args 
2. Parse_args parses and validates the command line arguments and returns to main
3. Main calls to DGRAM_make
4. DGRAM_make creates socket and returns to main
5. Main sends initial GA_status to gamer server, listens at socket, and periodically updates GA_status. Sends any hints to message.c
6. Main listens at the socketfor input until the game is over and send input to socket_handler.c
7. socket_handler.c handles socket stuff and takes a udp message and sends to parser.c
8. Parser.c packs and unpacks message and validates the input and puts it into a message struct then calls to opcode.c 
9. Opcode.c  performs the specific function and returns back to parser.c then socket_handler.c then main
10. Main does any final clean up

##### Major Data Structures

Hashtable (Function table for storing opCodes)
Array of clues

##### Structs

Program	utilizes all of	the structs described in common

##### Pseudocode for logic / algorithmic flow

1. Check command line args
2. Create DGRAM socket
3. Announce presence to game server by sending GA_status
4. Listen at socket for messages until game over 
    1. Periodically send GA_status
    2. Listen for user input (hints to be sent to field agent via game server)
    3. For each incoming message:
        1. Initialize message struct for message and validate message
        2. Get opcode from message struct and find function in function table 
    4. Update UI
5. Clean up

##### Testing Plan
1. Test each handler and make sure that each update (time, location) etc are correctly communicated to the fieldagent or gameserver
2. Test the UI to make sure game stats are updated appropriately and hints can be entered successfully

### Field Agent

##### User Interface

The Field Agent will have user interface on the Pebble Smart watch -- the UI will be a series of menus and displays that enable the user to:
1. Display game statistics, including game-time, the partially-uncovered string and the number of krags
2. Claim a Krag
3. See hints sent by the Guide Agent

##### Inputs and Outputs

Inputs from server:
1. GA_HINT messages passed from the Guide Agent through the server -- displayed on the watch.
2. Response Messages from the server (largely ignored)
3. Game Status updates -- used to update Game information

Inputs from user: 
1. Button pushes (navigates the menu)
2. Inputting Krags through the UI (used to sent FA_CLAIM messages to the server)
3. Initializing a team name and player name

Outputs to Server:
1. FA_CLAIM messages when a user finds a Krag
2. FA_LOCATION messages every 15 seconds (or on request from server)
3. FA_LOG messages

Outputs to User: See User Interface above.

##### Functional Decomposition into Modules

`Field_Agent.c`
1. Connect_to_proxy()
2. Claim_krag()
3. Display_game_stats()
4. Join_game()
5. Receive_hint()
6. Send_location()
7. Handle_UI()
9. Log_status()

`Opcode_handler.c`
1. Function_table_navigate() (takes a message and runs the appropriate message on the message based on its opcode)
2. Functions for each input to handle:
   1. Receive_hint()
   2. Update_game_stats() 
   3. Handle_response()

##### Dataflow through Modules

1. Main() instantiates the UI
2. Main calls connect_to_proxy(), which connects to the proxy server
3. Main calls join_game(), which prompts the user to select the team and player name
4. Pebble reacts to any sort of prompt / trigger:
   1. Every 15 seconds, send a FA_LOCATION message to the server
   2. If a message is received, handle it with opcode_handler
   3. If the user tries to insert a krag, send a FA_CLAIM message out to the server

##### Major Data Structures

Function table - Hashtable (for storing opCodes)

##### Structs

Program utilizes all of the structs described in common

##### Pseudocode for logic / algorithmic flow

1. Boot up the Proxy
2. Allow player to choose name and select team
3. Boot up UI to show game stats / hints / KRAG input terminal
4. While the program is running (listening):
   1. Every 15 seconds, send a FA_LOCATION message to the server
   2. If a message is received, handle it with opcode_handler
   3. If the user tries to insert a krag, send a FA_CLAIM message out to the server
   4. Constantly display game stats
   5. Log status to log file with FA_LOG as appropriate

##### Testing Plan

1. Test the User Interface -- make sure that the menu navigates, that the game stats display properly, and that the user's input is properly taken when they input a Krag
2. Test that the FA Pebble correctly connects to the Proxy Server
3. Test FA_Location -- make sure that locations are sent correctly through the proxy to the game server
4. Claim sample Krags (both valid and invalid) and see whether the messages send correctly, then see whether the server handles them properly
5. Test whether incoming hints from the server are appropriately handled and displayed on the Krag 
6. Test joining the game, sending the locations, and claiming sample Krags in one full running game


### Roles of group members

*  Robert: write `opCode_handler.c` in FA, GA, and GS directories, `fieldagent.c`
*  Tanya: write `message.c`, `gamestats.c`, `guideagent.c`
*  Jay: write `socket_handler.c`, `gameserver.c`, `fieldagent.c`
*  Breonna: write `socket_handler.c`, `gameserver.c`, `guideagent.c`