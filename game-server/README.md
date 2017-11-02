# Project KRAG - game-server

Team Eleph, May 2017

Computer Science 50

Dartmouth College

To build, run 'make'.

To clean up, run 'make clean'.

### Description of Game Server Usage:

Game server takes the following parameters in any order:

gameId=(hex code for gameId)
kff=(file name for file of krags)
sf=(file name for file of secret)
port=(port number)

As an example, the command line may look like this:
./gameserver gameId=5 kff=../common/fakekragfile.txt sf=fakeSecretFile.txt port=51888

Once the game server has been initialized, it will run itself. The Game stats will display on the sscreen, as well as a record of previous messages sent.

Type GAME OVER in through stdin to stop the game -- this sends GAME_OVER messages to all the players, cleans up, and leaves. This is the only accepted user input to the server.


Limitations:
We have comprehensive checks regarding players leaving the game and coming back with a new teamname or playername. However, if a player (with a particular ID) comes back with an entirely new IP address or port they are locked out of the game from beginning to end.




