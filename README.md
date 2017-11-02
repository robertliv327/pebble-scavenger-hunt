# CS50 Project KRAG

Team Eleph - May 2017

## Usage: 

To build, run `make`.

To clean up, run `make clean`.

To run `gameserver`: First build the gameserver by calling `make`, then call gameserver via the command line as follows:

`./gameserver gameId=... kff=... sf=... port=...`

* gameId provides the hexadecimal ID number of this game;
* kff provides the path to the krag file, which contains coordinates and clues for each krag;
* sf provides the path to the secret file, which contains a single line of text representing the secret.
* port provides the port number of the Game Server.

To run `guideagent`: First build the guideagent by calling `make`, then call guideagent via the command line as follows:

`./guideagent guideId=... team=... player=... host=... port=...`

* guideId provides the hexadecimal ID number for this player;
* team provides the name of the team to which the Agent belongs;
* player provides the name of the guide Agent;
* host provides the host name of the Game Server (e.g., flume.cs.dartmouth.edu);
* port provides the port number of the Game Server.

To run `fieldagent`: First build the fieldagent by calling `make`, then call fieldagent via the command line as follows:

`pebble install --emulator basalt --logs` for the emulator or...

`pebble install --logs --phone IPADDRESS` for the pebble watch`