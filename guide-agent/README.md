# Project KRAG - guide-agent

Team Eleph, May 2017

Computer Science 50

Dartmouth College

To build, run `make`.

To clean up, run `make clean`.

### Description of ncurses interface

Guide agent is run using a command line of form ./guideagent guideId=... team=... player=... host=... port=...

Upon running this command line, the user's terminal turns into a visual ncurses interface. The interface displays:
     1. A map of campus with all of the active field agent's last recorded locations plotted, as well as the locations of all claimed krags
     2. A panel of statistics about the game and the field agents, including a list of the unsolved clues and the partially revealed secret
     3. A box into which the user can enter stdin either to send a hint to its field agents (type hint, get prompter for player name or 'all') or to request a stsstatus update (type status then enter)

The list of unsolved clues automatically updates as FAs claim krags, but the GA has to call for a status update for the other statistics to update.

To exit the interface, the user can type control X C. The interface will automatically be exited once any of the teams reveals the entire secret.

### Limitations

The user cannot choose to send a hint to more than one FA without sending it to every FA. The user also can't see other teams' stats to know how its team is doing. 