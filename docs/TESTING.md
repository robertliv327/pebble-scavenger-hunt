# CS50 Final Project
### TESTING.md - TESTING document for KRAG 
##### Group Eleph, May 2017
 
### Summary:

To test our project, we went through multiple steps.
* Tested the common directory using the tests found in common/commontesting
* Tested Gameserver individually (detailed below)
* Tested Field Agent individually (detailed below)
* Tested Guide Agent individually (detailed below)
* Tested all three working together (detailed below)

### Testing for Game Server:

### SERVER UNIT TESTS
All unit tests were conducted using chatclient2, a sample UDP chat client program written by David Kotz for CS50. Below are the log files from each test.

#### Success Test: One field agent and one guide agent. The field agent and guide agent join the game, then the field agent claims a krag.
The guide agent sends hints, then the field agents attempts to claim the krag multiple times. The game server then ends the game.

(Tue May 30 19:04:25 2017) FROM 129.170.212.87@50684: opCode=FA_LOCATION|gameId=0|pebbleId=111111|team=Team1|player=FA1|latitude=43.706552|longitu\
de=-72.287418|statusReq=1
(Tue May 30 19:04:25 2017) TO 129.170.212.87@50684: opCode=GAME_STATUS|gameId=5|guideId=0|numClaimed=0|numKrags=11
(Tue May 30 19:04:33 2017) FROM 129.170.214.115@47480: opCode=GA_STATUS|gameId=0|guideId=222222|statusReq=1|player=GA1|team=Team1
(Tue May 30 19:04:33 2017) TO 129.170.214.115@47480: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=11
(Tue May 30 19:04:33 2017) TO 129.170.214.115@47480: opCode=GS_AGENT|gameId=5|pebbleId=111111|lastContact=8|latitude=43.706551|longitude=-72.28741\
5|player=FA1|team=Team1
(Tue May 30 19:04:33 2017) TO 129.170.214.115@47480: opCode=GS_CLUE|kragId=91da|gameId=5|guideId=222222|clue=super Dumb clue.
(Tue May 30 19:04:42 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=44.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:04:42 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
(Tue May 30 19:04:42 2017) TO 129.170.214.115@47480: opCode=GS_CLAIMED|kragId=86da|gameId=5|guideId=222222|pebbleId=111111|latitude=44.722599|long\
itude=-72.134201
(Tue May 30 19:04:42 2017) TO 129.170.214.115@47480: opCode=GS_CLUE|kragId=84da|gameId=5|guideId=222222|clue=Dumb clue 4.
(Tue May 30 19:04:42 2017) TO 129.170.214.115@47480: opCode=GS_CLUE|kragId=88da|gameId=5|guideId=222222|clue=Jay likes their smoothies.
(Tue May 30 19:04:42 2017) TO 129.170.214.115@47480: opCode=GS_SECRET|gameId=5|guideId=222222|secret=t__________c__________t___
(Tue May 30 19:04:50 2017) FROM 129.170.214.115@47480: opCode=GA_HINT|gameId=5|guideId=222222|team=Team1|player=GA1|pebbleId=111111|hint=TargetedH\
int
(Tue May 30 19:04:50 2017) TO 129.170.212.87@50684: opCode=GA_HINT|gameId=5|guideId=222222|pebbleId=111111|player=GA1|team=Team1|hint=TargetedHint
(Tue May 30 19:04:57 2017) FROM 129.170.214.115@47480: opCode=GA_HINT|gameId=5|guideId=222222|team=Team1|player=GA1|pebbleId=*|hint=BroadcastHint
(Tue May 30 19:04:57 2017) TO 129.170.214.115@47480: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_MESSAGE|gameId=5|text=fields do not match opCode
(Tue May 30 19:05:24 2017) FROM 129.170.214.115@47480: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-\
72.1342|kragId=86DA
(Tue May 30 19:05:24 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:32 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:05:32 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:40 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:05:40 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:46 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:05:46 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:47 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:05:47 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:48 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:05:48 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:49 2017) FROM 129.170.212.87@50684: opCode=FA_CLAIM|gameId=5|pebbleId=111111|team=Team1|player=FA1|latitude=43.7226|longitude=-7\
2.1342|kragId=86DA
(Tue May 30 19:05:49 2017) TO 129.170.212.87@50684: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 19:05:55 2017) FROM 129.170.214.115@47480: opCode=GA_HINT|gameId=5|guideId=222222|team=Team1|player=GA1|pebbleId=*|hint=BroadcastHint
(Tue May 30 19:05:56 2017) TO 129.170.214.115@47480: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_MESSAGE|gameId=5|text=fields do not match opCode
(Tue May 30 19:06:40 2017) TO 129.170.214.115@47480: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=Team1
(Tue May 30 19:06:40 2017) TO 129.170.214.115@47480: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato
(Tue May 30 19:06:40 2017) TO 129.170.212.87@50684: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=Team1
(Tue May 30 19:06:40 2017) TO 129.170.212.87@50684: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato

Explanation:
1. Game server responds appropriately with a GAME_STATUS message to initial FA_LOCATION and GA_STATUS messages.
2. In response to a GA_status initial message, the server sent a GS_AGENT, GS_CLUE and GAME_STATUS message
3. In response to a FA_CLAIM message, the server sends a GS_RESPONSE to the Field Agent, and a GS_CLAIMED, GS_CLUE, GS_CLUE, AND GS_SECRET to the Guide Agent
4. In response to a GA_HINT, the game server forwards the hint to the guide agent
5. In response to an FA_CLAIM for a krag already claimed, the server responds appropriately.
6. In response to user input, the server sends TEAM_RECORD and GAME_OVER messages to the field and guide agents and quits.

The server's simple success test resulted in it performing perfectly. In the case of uesr error resulting in invalid message codes, the server correctly sent response code message



##### Testing for edge cases -- invalid fields from existing agents

(Tue May 30 09:55:48 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=0|pebbleId=808|team=rightteam|player=player1|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 09:55:48 2017) TO 129.170.214.115@45718: opCode=GAME_STATUS|gameId=5|guideId=0|numClaimed=0|numKrags=1
(Tue May 30 09:55:59 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=5|pebbleId=808|team=rightteam|player=player1|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 09:55:59 2017) TO 129.170.214.115@45718: opCode=GAME_STATUS|gameId=5|guideId=0|numClaimed=0|numKrags=1
(Tue May 30 09:56:32 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=5|pebbleId=808|team=wrongteam|player=player1|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 09:56:32 2017) TO 129.170.214.115@45718: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_TEAMNAME|gameId=5|text=unrecognized team name
(Tue May 30 09:56:43 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=5|pebbleId=808|team=wrongteam|player=player1|latitude=43.706552|longitude=-72.287418|statusReq=0
(Tue May 30 09:56:43 2017) TO 129.170.214.115@45718: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_TEAMNAME|gameId=5|text=unrecognized team name
(Tue May 30 09:56:55 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=5|pebbleId=888|team=rightteam|player=player1|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 09:56:55 2017) TO 129.170.214.115@45718: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_ID|gameId=5|text=unrecognized pebbleId
(Tue May 30 09:57:14 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=5|pebbleId=808|team=rightteam|player=WRONGPLAYER|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 09:57:14 2017) TO 129.170.214.115@45718: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_PLAYERNAME|gameId=5|text=unrecognized FA player
(Tue May 30 09:57:29 2017) FROM 129.170.214.115@45718: opCode=GA_STATUS|gameId=0|guideId=2c3|statusReq=1|player=Green|team=carpool
(Tue May 30 09:57:29 2017) TO 129.170.214.115@45718: opCode=GAME_STATUS|gameId=5|guideId=2c3|numClaimed=0|numKrags=1
(Tue May 30 09:57:29 2017) TO 129.170.214.115@45718: opCode=GS_CLUE|kragId=86da|gameId=5|guideId=2c3|clue=It has a million stories, but cannot tell them.
(Tue May 30 09:57:44 2017) FROM 129.170.214.115@45718: opCode=FA_LOCATION|gameId=5|pebbleId=808|team=rightteam|player=player1|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 09:57:44 2017) TO 129.170.214.115@45718: opCode=GAME_STATUS|gameId=5|guideId=0|numClaimed=0|numKrags=1
(Tue May 30 09:57:53 2017) FROM 129.170.214.115@45718: opCode=GA_STATUS|gameId=5|guideId=2c3|statusReq=1|player=Green|team=carpool
(Tue May 30 09:57:53 2017) TO 129.170.214.115@45718: opCode=GAME_STATUS|gameId=5|guideId=2c3|numClaimed=0|numKrags=1
(Tue May 30 09:58:01 2017) FROM 129.170.214.115@45718: opCode=GA_STATUS|gameId=5|guideId=BAD|statusReq=1|player=Green|team=carpool
(Tue May 30 09:58:01 2017) TO 129.170.214.115@45718: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_ID|gameId=5|text=unrecognized guideId
(Tue May 30 09:58:07 2017) FROM 129.170.214.115@45718: opCode=GA_STATUS|gameId=5|guideId=2c3|statusReq=1|player=WrongPlayer|team=carpool
(Tue May 30 09:58:07 2017) TO 129.170.214.115@45718: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_PLAYERNAME|gameId=5|text=unrecognized GA player


Explanation: We tested edge cases here, by setting up an initial field agent, then having the field agent simulate a player dropping out of the game and coming back in.
The field agent continued to enter the game with team-names, player-names, and IDs that were different from what they were when the Field Agent originally entered with.
In each case, the server correctly sent an INVALID_TEAMNAME, INVALID_ID, or INVALID_PLAYERNAME as appropriate.


### SERVER LIVE TEST

The following tests were taken with the server testing live with either the Field Agent, the Guide agent, or both.

#### SERVER-FIELD AGENT LIVE TEST

We started server and set up a fake guide agent, then had the real Field Agent enter and perform various actions:
1. Send logs
2. Send Locations
3. Recieve Hints
4. Handle Responses
5. Accept Game-overs

(Tue May 30 20:10:47 2017) FROM 129.170.214.115@48756: opCode=GA_STATUS|gameId=0|guideId=222222|statusReq=1|player=GA1|team=carpool
(Tue May 30 20:10:47 2017) TO 129.170.214.115@48756: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3
(Tue May 30 20:10:47 2017) TO 129.170.214.115@48756: opCode=GS_CLUE|kragId=88da|gameId=5|guideId=222222|clue=Jay likes their smoothies.
(Tue May 30 20:11:15 2017) FROM 129.170.214.115@45822: opCode=FA_LOG|pebbleId=01234567|text=Joined Game!
(Tue May 30 20:11:19 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=0|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:11:19 2017) TO 129.170.214.115@45822: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3
(Tue May 30 20:11:30 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:11:31 2017) FROM 129.170.214.115@48756: opCode=GA_HINT|gameId=5|guideId=222222|team=carpool|player=GA1|pebbleId=01234567|hint=TargetedHint
(Tue May 30 20:11:31 2017) TO 129.170.214.115@45822: opCode=GA_HINT|gameId=5|guideId=222222|pebbleId=1234567|player=GA1|team=carpool|hint=TargetedHint
(Tue May 30 20:11:46 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:11:47 2017) FROM 129.170.214.115@45822: opCode=FA_CLAIM|kragId=1e1c|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:02 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:04 2017) FROM 129.170.214.115@45822: opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:04 2017) TO 129.170.214.115@45822: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_CLAIMED|kragId=86da|gameId=5|guideId=222222|pebbleId=1234567|latitude=43.72|longitude=-72.1
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_CLUE|kragId=87da|gameId=5|guideId=222222|clue=It's big and it's greeen.
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_CLUE|kragId=86da|gameId=5|guideId=222222|clue=It has a million stories, but cannot tell them.
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_SECRET|gameId=5|guideId=222222|secret=t__s__t__s__r__i__s__o__t_
(Tue May 30 20:12:17 2017) FROM 129.170.214.115@45822: opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:17 2017) TO 129.170.214.115@45822: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 20:12:18 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:29 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=1|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:29 2017) TO 129.170.214.115@45822: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=1|numKrags=3
(Tue May 30 20:12:34 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:38 2017) TO 129.170.214.115@48756: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool
(Tue May 30 20:12:38 2017) TO 129.170.214.115@48756: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato
(Tue May 30 20:12:38 2017) TO 129.170.214.115@45822: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool
(Tue May 30 20:12:38 2017) TO 129.170.214.115@45822: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato

1. Send logs -- The game server ignored the log files as appropriate
2. Send Locations -- The game server accurately recorded the locations and updated the guide agent with those locations
3. Recieve Hints -- The game server sent hints broadcasted from the Guide Agent
4. Handle Responses -- The game server sent INVALID_TEAMNAME responses as appropriate
5. Accept Game-overs -- the field agent accepted the game server's Game-over.


### Testing for Field Agent

To test the field agent, we formulated a sequence of events that would test every possible interaction of the Field Agent. This sequence of events was logged to the terminal output via app logs and ouput relating to the string messages send and received through the outbox. 

The sequence of events is as follows: We set up our game server and connected a guide agent to it via a chat client. Then we booted the pebble and joined the game under name "Robert" and team "carpool". We then had the guide agent send the player a hint. The player then went and viewed this hint, expanding it to view the entire hint. We then had the user go and enter three krags. The first krag we inserted was intentionally invalid and it was properly ignored. The second krag we inserted was valid, and it was claimed successfully. The third krag was a duplicate of the second, and the FA was correctly informed that it was an attempted duplicate claim. The user then requests a status update, which sends an FA_LOCATION with statusreq set to 1. The FA correctly gets a game status message in response. At this point, the game server entered "GAME OVER" from the terminal, which ended the game. Throughout this process, we are correctly sending FA_LOCATION messages every 15 seconds.

The game server's log of this test is included below, as is the log from the field agent.

Outside of this test, we also checked to make sure that functionality is correct when a player quits the app and rejoins it. This process correctly works if the player chooses the same player name and team name that it chose in the previous interaction. This process correctly prompts the player to re-pick a player name or team name if the player tries to choose one that it did not use in previous communication.

**GAME SERVER'S LOG FILE**

```
(Tue May 30 20:10:47 2017) FROM 129.170.214.115@48756: opCode=GA_STATUS|gameId=0|guideId=222222|statusReq=1|player=GA1|team=carpool
(Tue May 30 20:10:47 2017) TO 129.170.214.115@48756: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3
(Tue May 30 20:10:47 2017) TO 129.170.214.115@48756: opCode=GS_CLUE|kragId=88da|gameId=5|guideId=222222|clue=Jay likes their smoothies.
(Tue May 30 20:11:15 2017) FROM 129.170.214.115@45822: opCode=FA_LOG|pebbleId=01234567|text=Joined Game!
(Tue May 30 20:11:19 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=0|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:11:19 2017) TO 129.170.214.115@45822: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3
(Tue May 30 20:11:30 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:11:31 2017) FROM 129.170.214.115@48756: opCode=GA_HINT|gameId=5|guideId=222222|team=carpool|player=GA1|pebbleId=01234567|hint=TargetedHint
(Tue May 30 20:11:31 2017) TO 129.170.214.115@45822: opCode=GA_HINT|gameId=5|guideId=222222|pebbleId=1234567|player=GA1|team=carpool|hint=TargetedHint
(Tue May 30 20:11:46 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:11:47 2017) FROM 129.170.214.115@45822: opCode=FA_CLAIM|kragId=1e1c|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:02 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:04 2017) FROM 129.170.214.115@45822: opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:04 2017) TO 129.170.214.115@45822: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_CLAIMED|kragId=86da|gameId=5|guideId=222222|pebbleId=1234567|latitude=43.72|longitude=-72.1
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_CLUE|kragId=87da|gameId=5|guideId=222222|clue=It's big and it's greeen.
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_CLUE|kragId=86da|gameId=5|guideId=222222|clue=It has a million stories, but cannot tell them.
(Tue May 30 20:12:04 2017) TO 129.170.214.115@48756: opCode=GS_SECRET|gameId=5|guideId=222222|secret=t__s__t__s__r__i__s__o__t_
(Tue May 30 20:12:17 2017) FROM 129.170.214.115@45822: opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:17 2017) TO 129.170.214.115@45822: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 20:12:18 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:29 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=1|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:29 2017) TO 129.170.214.115@45822: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=1|numKrags=3
(Tue May 30 20:12:34 2017) FROM 129.170.214.115@45822: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
(Tue May 30 20:12:38 2017) TO 129.170.214.115@48756: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool
(Tue May 30 20:12:38 2017) TO 129.170.214.115@48756: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato
(Tue May 30 20:12:38 2017) TO 129.170.214.115@45822: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool
(Tue May 30 20:12:38 2017) TO 129.170.214.115@45822: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato
```

**OUTPUT FROM FIELD AGENT TERMINAL**

```
roberts-mbp-6:field-agent robertliv327$ pebble install --emulator basalt --logs
Installing app...
App install succeeded.
[20:11:14] essage_outbox.c:69> app_message_open() called with app_message_outbox_size_maximum().
[20:11:14] essage_outbox.c:72> This consumes 8200 bytes of heap memory, potentially more in the future!
[20:11:14] message_inbox.c:13> app_message_open() called with app_message_inbox_size_maximum().
[20:11:14] message_inbox.c:16> This consumes 8200 bytes of heap memory, potentially more in the future!
[20:11:14] pkjs> PebbleKit JS Environment ready!
[20:11:14] ssage_handler.c:96> Got AppKeyJSReady: 0123456789abcdef0123456789abcdef

[20:11:14] pkjs> BT-TX: sendAppMessage success: '{AppKeyJSReady : 0123456789abcdef0123456789abcdef}'
[20:11:14] pkjs> WEBSOCKET: connected to ws://flume.cs.dartmouth.edu:35845
[20:11:15] ssage_handler.c:48> Tick.
[20:11:15] pkjs> BT-RX: AppMessage from Pebble received: {"4":1,"AppKeyPebbleId":1} 
[20:11:15] ssage_handler.c:197> Requested pebbleId.

[20:11:15] ssage_handler.c:161> Message successfully sent.

[20:11:15] ssage_handler.c:143> Got AppKeyPebbleId: 0123456789abcdef0123456789abcdef

[20:11:15] ssage_handler.c:248> Sent message.

[20:11:15] pkjs> WEBSOCKET-TX: opCode=FA_LOG|pebbleId=01234567|text=Joined Game!
[20:11:15] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOG|pebbleId=01234567|text=Joined Game!","AppKeySendMsg":"opCode=FA_LOG|pebbleId=01234567|text=Joined Game!"} 
[20:11:15] ssage_handler.c:383> Sent FA_LOG

[20:11:15] pkjs> BT-TX: sendAppMessage success: '{AppKeyPebbleId : 0123456789abcdef0123456789abcdef}'
[20:11:15] ssage_handler.c:161> Message successfully sent.

[20:11:16] ssage_handler.c:48> Tick.
[20:11:17] player_window.c:91> Joined game with player: Robert
[20:11:17] ssage_handler.c:48> Tick.
[20:11:18] ssage_handler.c:48> Tick.
[20:11:18] e_team_window.c:91> Joined game with team: carpool
[20:11:19] ssage_handler.c:48> Tick.
[20:11:19] ssage_handler.c:223> Requested location.

[20:11:19] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:11:19] ssage_handler.c:161> Message successfully sent.

[20:11:19] ssage_handler.c:248> Sent message.

[20:11:19] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=0|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:11:19] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=0|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=0|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:11:19] ssage_handler.c:305> Sent FA_LOCATION

[20:11:19] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:11:19] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:11:19] pkjs> WEBSOCKET-RX: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3
[20:11:19] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:11:19] ssage_handler.c:161> Message successfully sent.

[20:11:19] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3

[20:11:19] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=0|numKrags=3}'
[20:11:20] ssage_handler.c:48> Tick.
[20:11:21] ssage_handler.c:48> Tick.
[20:11:22] ssage_handler.c:48> Tick.
[20:11:23] ssage_handler.c:48> Tick.
[20:11:24] ssage_handler.c:48> Tick.
[20:11:25] ssage_handler.c:48> Tick.
[20:11:26] ssage_handler.c:48> Tick.
[20:11:27] ssage_handler.c:48> Tick.
[20:11:28] ssage_handler.c:48> Tick.
[20:11:29] ssage_handler.c:48> Tick.
[20:11:30] ssage_handler.c:48> Tick.
[20:11:30] ssage_handler.c:69> 15 seconds went off - location request
[20:11:30] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:11:30] ssage_handler.c:223> Requested location.

[20:11:30] ssage_handler.c:161> Message successfully sent.

[20:11:30] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:11:30] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:11:30] ssage_handler.c:248> Sent message.

[20:11:30] ssage_handler.c:305> Sent FA_LOCATION

[20:11:30] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:11:30] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:11:30] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:11:30] ssage_handler.c:161> Message successfully sent.

[20:11:31] ssage_handler.c:48> Tick.
[20:11:31] pkjs> WEBSOCKET-RX: opCode=GA_HINT|gameId=5|guideId=222222|pebbleId=1234567|player=GA1|team=carpool|hint=TargetedHint
[20:11:31] pCode_handler.c:92> Received hint: TargetedHint

[20:11:31] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=GA_HINT|gameId=5|guideId=222222|pebbleId=1234567|player=GA1|t
[20:11:31] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=GA_HINT|gameId=5|guideId=222222|pebbleId=1234567|player=GA1|team=carpool|hint=TargetedHint}'
[20:11:32] ssage_handler.c:48> Tick.
[20:11:33] ssage_handler.c:48> Tick.
[20:11:34] ssage_handler.c:48> Tick.
[20:11:35] ssage_handler.c:48> Tick.
[20:11:36] ssage_handler.c:48> Tick.
[20:11:37] hint_window.c:55> Player chose to look at hint 0

[20:11:37] ssage_handler.c:48> Tick.
[20:11:38] ssage_handler.c:48> Tick.
[20:11:39] ssage_handler.c:48> Tick.
[20:11:40] ssage_handler.c:48> Tick.
[20:11:41] ssage_handler.c:48> Tick.
[20:11:42] ssage_handler.c:48> Tick.
[20:11:43] ssage_handler.c:48> Tick.
[20:11:44] ssage_handler.c:48> Tick.
[20:11:45] ssage_handler.c:48> Tick.
[20:11:46] ssage_handler.c:48> Tick.
[20:11:46] ssage_handler.c:69> 15 seconds went off - location request
[20:11:46] ssage_handler.c:223> Requested location.

[20:11:46] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:11:46] ssage_handler.c:161> Message successfully sent.

[20:11:46] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:11:46] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:11:46] ssage_handler.c:248> Sent message.

[20:11:46] ssage_handler.c:305> Sent FA_LOCATION

[20:11:46] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:11:46] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:11:46] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:11:46] ssage_handler.c:161> Message successfully sent.

[20:11:47] field_agent.c:103> Krag inputed was 1 e 1 c
[20:11:47] ssage_handler.c:223> Requested location.

[20:11:47] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:11:47] ssage_handler.c:161> Message successfully sent.

[20:11:47] ssage_handler.c:48> Tick.
[20:11:47] pkjs> WEBSOCKET-TX: opCode=FA_CLAIM|kragId=1e1c|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:11:47] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_CLAIM|kragId=1e1c|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_CLAIM|kragId=1e1c|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:11:47] ssage_handler.c:248> Sent message.

[20:11:47] ssage_handler.c:352> Sent FA_CLAIM

[20:11:47] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:11:47] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:11:47] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:11:47] ssage_handler.c:161> Message successfully sent.

[20:11:48] ssage_handler.c:48> Tick.
[20:11:49] ssage_handler.c:48> Tick.
[20:11:50] ssage_handler.c:48> Tick.
[20:11:51] ssage_handler.c:48> Tick.
[20:11:52] ssage_handler.c:48> Tick.
[20:11:53] ssage_handler.c:48> Tick.
[20:11:54] ssage_handler.c:48> Tick.
[20:11:55] ssage_handler.c:48> Tick.
[20:11:56] ssage_handler.c:48> Tick.
[20:11:57] ssage_handler.c:48> Tick.
[20:11:58] ssage_handler.c:48> Tick.
[20:11:59] ssage_handler.c:48> Tick.
[20:12:00] ssage_handler.c:48> Tick.
[20:12:01] ssage_handler.c:48> Tick.
[20:12:02] ssage_handler.c:48> Tick.
[20:12:02] ssage_handler.c:69> 15 seconds went off - location request
[20:12:02] ssage_handler.c:223> Requested location.

[20:12:02] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:12:02] ssage_handler.c:161> Message successfully sent.

[20:12:02] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:12:02] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:12:02] ssage_handler.c:248> Sent message.

[20:12:02] ssage_handler.c:305> Sent FA_LOCATION

[20:12:02] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:12:02] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:12:02] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:12:02] ssage_handler.c:161> Message successfully sent.

[20:12:03] ssage_handler.c:48> Tick.
[20:12:03] field_agent.c:103> Krag inputed was 8 6 d a
[20:12:03] ssage_handler.c:223> Requested location.

[20:12:03] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:12:03] ssage_handler.c:161> Message successfully sent.

[20:12:03] ssage_handler.c:248> Sent message.

[20:12:03] pkjs> WEBSOCKET-TX: opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:12:03] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:12:03] ssage_handler.c:352> Sent FA_CLAIM

[20:12:03] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:12:03] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:12:03] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:12:03] ssage_handler.c:161> Message successfully sent.

[20:12:04] pkjs> WEBSOCKET-RX: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
[20:12:04] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!

[20:12:04] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!}'
[20:12:04] ssage_handler.c:48> Tick.
[20:12:05] ssage_handler.c:48> Tick.
[20:12:06] ssage_handler.c:48> Tick.
[20:12:07] ssage_handler.c:48> Tick.
[20:12:08] ssage_handler.c:48> Tick.
[20:12:09] ssage_handler.c:48> Tick.
[20:12:10] ssage_handler.c:48> Tick.
[20:12:11] ssage_handler.c:48> Tick.
[20:12:12] ssage_handler.c:48> Tick.
[20:12:13] ssage_handler.c:48> Tick.
[20:12:14] ssage_handler.c:48> Tick.
[20:12:15] ssage_handler.c:48> Tick.
[20:12:16] ssage_handler.c:48> Tick.
[20:12:17] field_agent.c:103> Krag inputed was 8 6 d a
[20:12:17] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:12:17] ssage_handler.c:223> Requested location.

[20:12:17] ssage_handler.c:161> Message successfully sent.

[20:12:17] ssage_handler.c:48> Tick.
[20:12:17] pkjs> WEBSOCKET-TX: opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:12:17] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_CLAIM|kragId=86da|gameId=5|pebbleId=01234567|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:12:17] ssage_handler.c:248> Sent message.

[20:12:17] ssage_handler.c:352> Sent FA_CLAIM

[20:12:17] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:12:17] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:12:17] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:12:17] ssage_handler.c:161> Message successfully sent.

[20:12:17] pkjs> WEBSOCKET-RX: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
[20:12:17] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag al
[20:12:17] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!}'
[20:12:18] ssage_handler.c:48> Tick.
[20:12:18] ssage_handler.c:69> 15 seconds went off - location request
[20:12:18] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:12:18] ssage_handler.c:223> Requested location.

[20:12:18] ssage_handler.c:161> Message successfully sent.

[20:12:18] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:12:18] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:12:18] ssage_handler.c:248> Sent message.

[20:12:18] ssage_handler.c:305> Sent FA_LOCATION

[20:12:18] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:12:18] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:12:18] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:12:18] ssage_handler.c:161> Message successfully sent.

[20:12:19] ssage_handler.c:48> Tick.
[20:12:20] ssage_handler.c:48> Tick.
[20:12:21] ssage_handler.c:48> Tick.
[20:12:22] ssage_handler.c:48> Tick.
[20:12:23] ssage_handler.c:48> Tick.
[20:12:24] ssage_handler.c:48> Tick.
[20:12:25] ssage_handler.c:48> Tick.
[20:12:26] ssage_handler.c:48> Tick.
[20:12:27] ssage_handler.c:48> Tick.
[20:12:28] ssage_handler.c:48> Tick.
[20:12:29] ssage_handler.c:223> Requested location.

[20:12:29] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:12:29] ssage_handler.c:161> Message successfully sent.

[20:12:29] ssage_handler.c:248> Sent message.

[20:12:29] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=1|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:12:29] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=1|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=1|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:12:29] ssage_handler.c:305> Sent FA_LOCATION

[20:12:29] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:12:29] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:12:29] pkjs> WEBSOCKET-RX: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=1|numKrags=3
[20:12:29] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:12:29] ssage_handler.c:161> Message successfully sent.

[20:12:29] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=1|numKrags=3

[20:12:29] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=GAME_STATUS|gameId=5|guideId=222222|numClaimed=1|numKrags=3}'
[20:12:29] ssage_handler.c:48> Tick.
[20:12:30] ssage_handler.c:48> Tick.
[20:12:31] ssage_handler.c:48> Tick.
[20:12:32] ssage_handler.c:48> Tick.
[20:12:33] ssage_handler.c:48> Tick.
[20:12:34] ssage_handler.c:48> Tick.
[20:12:34] ssage_handler.c:69> 15 seconds went off - location request
[20:12:34] pkjs> BT-RX: AppMessage from Pebble received: {"3":1,"AppKeyLocation":1} 
[20:12:34] ssage_handler.c:223> Requested location.

[20:12:34] ssage_handler.c:161> Message successfully sent.

[20:12:34] pkjs> WEBSOCKET-TX: opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342
[20:12:34] pkjs> BT-RX: AppMessage from Pebble received: {"1":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342","AppKeySendMsg":"opCode=FA_LOCATION|gameId=5|pebbleId=01234567|statusReq=0|player=Robert|team=carpool|latitude=43.7226|longitude=-72.1342"} 
[20:12:34] ssage_handler.c:248> Sent message.

[20:12:34] ssage_handler.c:305> Sent FA_LOCATION

[20:12:34] ssage_handler.c:133> Got AppKeyLocation: 43.7226|-72.1342

[20:12:34] ssage_handler.c:155> Got AppKeySendError: 43.7226|-72.1342

[20:12:34] pkjs> BT-TX: sendAppMessage success: '{AppKeyLocation : 43.7226|-72.1342}'
[20:12:34] ssage_handler.c:161> Message successfully sent.

[20:12:35] ssage_handler.c:48> Tick.
[20:12:36] ssage_handler.c:48> Tick.
[20:12:37] ssage_handler.c:48> Tick.
[20:12:38] ssage_handler.c:48> Tick.
[20:12:38] pkjs> WEBSOCKET-RX: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool
[20:12:38] pkjs> WEBSOCKET-RX: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato
[20:12:38] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool

[20:12:38] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=TEAM_RECORD|gameId=5|numPlayers=1|numClaimed=1|team=carpool}'
[20:12:38] pCode_handler.c:275> Received GAME_OVER!

[20:12:38] ssage_handler.c:110> Got AppKeyrecvMsg: opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato

[20:12:38] pkjs> BT-TX: sendAppMessage success: '{AppKeyRecvMsg : opCode=GAME_OVER|gameId=5|secret=thisisthesecretitisapotato}'
[20:12:39] ssage_handler.c:48> Tick.
[20:12:40] ssage_handler.c:48> Tick.
[20:12:41] ssage_handler.c:48> Tick.
[20:12:42] ssage_handler.c:48> Tick.
[20:12:43] ssage_handler.c:48> Tick.
^Croberts-mbp-6:field-agent robertliv327$ 
```

## Documentation for testing for guide agent

###  UNIT TESTS
All unit tests were conducted using chatclient2, a sample UDP chat client program written by David Kotz for CS50. Below are the log files from each test.

#### One example of a success Test: Gameserver and one guide agent and two field agents. The guide agent and field agent join the game, then the field agents claim krags, revealing the secret. The guide agent calls for a status update then sends one of the field agents a hint. It then sends both field agents a hint. There are some border cases also tested in this log - a player that doesn't exist "claims" a krag and this leads to an invalid player error. A FA also claims a krag already claimed and this leads to an error as well. 

##### Log for guide agent during test

(Tue May 30 21:13:35 2017) FROM 129.170.214.115@51888: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=0|numKrags=11
(Tue May 30 21:13:35 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=91da|gameId=5|guideId=707|clue=super Dumb clue.
(Tue May 30 21:13:47 2017) FROM 129.170.214.115@51888: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=0|numKrags=11
(Tue May 30 21:13:47 2017) FROM 129.170.214.115@51888: opCode=GS_AGENT|gameId=5|pebbleId=808|lastContact=3|latitude=43.706551|longitude=-72.287415|player=Tanya|team=aq\
ua
(Tue May 30 21:14:10 2017) FROM 129.170.214.115@51888: opCode=GS_CLAIMED|kragId=84da|gameId=5|guideId=707|pebbleId=808|latitude=48.222221|longitude=-12.222222
(Tue May 30 21:14:10 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=84da|gameId=5|guideId=707|clue=Dumb clue 4.
(Tue May 30 21:14:10 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=88da|gameId=5|guideId=707|clue=Jay likes their smoothies.
(Tue May 30 21:14:10 2017) FROM 129.170.214.115@51888: opCode=GS_SECRET|gameId=5|guideId=707|secret=______t__________i________
(Tue May 30 21:14:18 2017) FROM 129.170.214.115@51888: opCode=GS_CLAIMED|kragId=88da|gameId=5|guideId=707|pebbleId=808|latitude=48.222221|longitude=-12.222222
(Tue May 30 21:14:18 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=87da|gameId=5|guideId=707|clue=It's big and it's greeen.
(Tue May 30 21:14:18 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=85da|gameId=5|guideId=707|clue=Dumb clue 5.
(Tue May 30 21:14:18 2017) FROM 129.170.214.115@51888: opCode=GS_SECRET|gameId=5|guideId=707|secret=__i___t______e___i______t_
(Tue May 30 21:14:41 2017) FROM 129.170.214.115@51888: opCode=GS_CLAIMED|kragId=81da|gameId=5|guideId=707|pebbleId=808|latitude=48.222221|longitude=-12.222222
(Tue May 30 21:14:41 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=92da|gameId=5|guideId=707|clue=super Dumb clue 2.
(Tue May 30 21:14:41 2017) FROM 129.170.214.115@51888: opCode=GS_CLUE|kragId=83da|gameId=5|guideId=707|clue=Dumb clue 3.
(Tue May 30 21:14:41 2017) FROM 129.170.214.115@51888: opCode=GS_SECRET|gameId=5|guideId=707|secret=__is__t______et__i______to
(Tue May 30 21:14:46 2017) FROM 129.170.214.115@51888: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=3|numKrags=11
(Tue May 30 21:14:46 2017) FROM 129.170.214.115@51888: opCode=GS_AGENT|gameId=5|pebbleId=909|lastContact=51|latitude=43.703785|longitude=-72.287132|player=Robert|team=\
aqua
(Tue May 30 21:14:46 2017) FROM 129.170.214.115@51888: opCode=GS_AGENT|gameId=5|pebbleId=808|lastContact=62|latitude=43.706551|longitude=-72.287415|player=Tanya|team=a\
qua
(Tue May 30 21:15:31 2017) TO 129.170.214.115@51888: opCode=GA_HINT|gameId=5|guideId=707|pebbleId=2a|player=Bob|team=aqua|hint=its a big field in the middle of campus
(Tue May 30 21:15:43 2017) TO 129.170.214.115@51888: opCode=GA_HINT|gameId=5|guideId=707|pebbleId=808|player=Bob|team=aqua|hint=hey this is a clue for tanya

##### Log for game server during test

(Tue May 30 21:13:35 2017) FROM 129.170.213.207@60432: opCode=GA_STATUS|gameId=0|guideId=707|statusReq=1|player=Bob|team=aqua
(Tue May 30 21:13:35 2017) TO 129.170.213.207@60432: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=0|numKrags=11
(Tue May 30 21:13:35 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=91da|gameId=5|guideId=707|clue=super Dumb clue.
(Tue May 30 21:13:43 2017) FROM 129.170.213.207@50545: opCode=FA_LOCATION|gameId=0|pebbleId=808|team=aqua|player=Tanya|latitude=43.706552|longitude=-72.287418|statusReq=1
(Tue May 30 21:13:43 2017) TO 129.170.213.207@50545: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=0|numKrags=11
(Tue May 30 21:13:46 2017) FROM 129.170.213.207@60432: opCode=GA_STATUS|gameId=5|guideId=707|statusReq=1|player=Bob|team=aqua
(Tue May 30 21:13:46 2017) TO 129.170.213.207@60432: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=0|numKrags=11
(Tue May 30 21:13:46 2017) TO 129.170.213.207@60432: opCode=GS_AGENT|gameId=5|pebbleId=808|lastContact=3|latitude=43.706551|longitude=-72.287415|player=Tanya|team=aqua
(Tue May 30 21:13:54 2017) FROM 129.170.213.207@50545: opCode=FA_LOCATION|gameId=0|pebbleId=909|team=aqua|player=Robert|latitude=43.703784|longitude=-72.287132|statusReq=1
(Tue May 30 21:13:54 2017) TO 129.170.213.207@50545: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=0|numKrags=11
(Tue May 30 21:13:59 2017) FROM 129.170.213.207@50545: opCode=FA_CLAIM|gameId=5|pebbleId=808|team=aqua|player=Robert|latitude=48.222222|longitude=-12.222222|kragId=85DA
(Tue May 30 21:13:59 2017) TO 129.170.213.207@50545: opCode=GS_RESPONSE|respCode=SH_ERROR_INVALID_PLAYERNAME|gameId=5|text=unrecognized FA player
(Tue May 30 21:14:09 2017) FROM 129.170.213.207@50545: opCode=FA_CLAIM|gameId=5|pebbleId=808|team=aqua|player=Tanya|latitude=48.222222|longitude=-12.222222|kragId=84DA
(Tue May 30 21:14:10 2017) TO 129.170.213.207@50545: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
(Tue May 30 21:14:10 2017) TO 129.170.213.207@60432: opCode=GS_CLAIMED|kragId=84da|gameId=5|guideId=707|pebbleId=808|latitude=48.222221|longitude=-12.222222
(Tue May 30 21:14:10 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=84da|gameId=5|guideId=707|clue=Dumb clue 4.
(Tue May 30 21:14:10 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=88da|gameId=5|guideId=707|clue=Jay likes their smoothies.
(Tue May 30 21:14:10 2017) TO 129.170.213.207@60432: opCode=GS_SECRET|gameId=5|guideId=707|secret=______t__________i________
(Tue May 30 21:14:18 2017) FROM 129.170.213.207@50545: opCode=FA_CLAIM|gameId=5|pebbleId=808|team=aqua|player=Tanya|latitude=48.222222|longitude=-12.222222|kragId=88DA
(Tue May 30 21:14:18 2017) TO 129.170.213.207@50545: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
(Tue May 30 21:14:18 2017) TO 129.170.213.207@60432: opCode=GS_CLAIMED|kragId=88da|gameId=5|guideId=707|pebbleId=808|latitude=48.222221|longitude=-12.222222
(Tue May 30 21:14:18 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=87da|gameId=5|guideId=707|clue=It's big and it's greeen.
(Tue May 30 21:14:18 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=85da|gameId=5|guideId=707|clue=Dumb clue 5.
(Tue May 30 21:14:18 2017) TO 129.170.213.207@60432: opCode=GS_SECRET|gameId=5|guideId=707|secret=__i___t______e___i______t_
(Tue May 30 21:14:28 2017) FROM 129.170.213.207@50545: opCode=FA_CLAIM|gameId=5|pebbleId=808|team=aqua|player=Tanya|latitude=48.222222|longitude=-12.222222|kragId=88DA
(Tue May 30 21:14:28 2017) TO 129.170.213.207@50545: opCode=GS_RESPONSE|respCode=SH_CLAIMED_ALREADY|gameId=5|text=krag already claimed!
(Tue May 30 21:14:41 2017) FROM 129.170.213.207@50545: opCode=FA_CLAIM|gameId=5|pebbleId=808|team=aqua|player=Tanya|latitude=48.222222|longitude=-12.222222|kragId=81DA
(Tue May 30 21:14:41 2017) TO 129.170.213.207@50545: opCode=GS_RESPONSE|respCode=SH_CLAIMED|gameId=5|text=claimed krag!
(Tue May 30 21:14:41 2017) TO 129.170.213.207@60432: opCode=GS_CLAIMED|kragId=81da|gameId=5|guideId=707|pebbleId=808|latitude=48.222221|longitude=-12.222222
(Tue May 30 21:14:41 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=92da|gameId=5|guideId=707|clue=super Dumb clue 2.
(Tue May 30 21:14:41 2017) TO 129.170.213.207@60432: opCode=GS_CLUE|kragId=83da|gameId=5|guideId=707|clue=Dumb clue 3.
(Tue May 30 21:14:41 2017) TO 129.170.213.207@60432: opCode=GS_SECRET|gameId=5|guideId=707|secret=__is__t______et__i______to
(Tue May 30 21:14:45 2017) FROM 129.170.213.207@60432: opCode=GA_STATUS|gameId=5|guideId=707|statusReq=1|player=Bob|team=aqua
(Tue May 30 21:14:45 2017) TO 129.170.213.207@60432: opCode=GAME_STATUS|gameId=5|guideId=707|numClaimed=3|numKrags=11
(Tue May 30 21:14:45 2017) TO 129.170.213.207@60432: opCode=GS_AGENT|gameId=5|pebbleId=909|lastContact=51|latitude=43.703785|longitude=-72.287132|player=Robert|team=aqua
(Tue May 30 21:14:45 2017) TO 129.170.213.207@60432: opCode=GS_AGENT|gameId=5|pebbleId=808|lastContact=62|latitude=43.706551|longitude=-72.287415|player=Tanya|team=aqua
(Tue May 30 21:15:32 2017) FROM 129.170.213.207@60432: opCode=GA_HINT|gameId=5|guideId=707|pebbleId=2a|player=Bob|team=aqua|hint=its a big field in the middle of campus
(Tue May 30 21:15:32 2017) TO 129.170.213.207@50545: opCode=GA_HINT|gameId=5|guideId=707|pebbleId=2a|player=Bob|team=aqua|hint=its a big field in the middle of campus
(Tue May 30 21:15:32 2017) TO 129.170.213.207@50545: opCode=GA_HINT|gameId=5|guideId=707|pebbleId=2a|player=Bob|team=aqua|hint=its a big field in the middle of campus|gameId=5|gu\
ideId=707|pebbleId=2a|player=Bob|team=aqua|hint=its a big field in the middle of campus
(Tue May 30 21:15:44 2017) FROM 129.170.213.207@60432: opCode=GA_HINT|gameId=5|guideId=707|pebbleId=808|player=Bob|team=aqua|hint=hey this is a clue for tanya
