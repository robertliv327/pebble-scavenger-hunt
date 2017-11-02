# Project KRAG - field-agent

Team Eleph, May 2017

Computer Science 50

Dartmouth College

To build, run `make`.

To clean up, run `make clean`.

### Description of Pebble UI

Upon launch of the app, the user is prompted to pick a team and player name from a set list. If either of these selections contradict what the player has chosen in previous communication (i.e. before a crash), then the player will be re-prompted to pick the team or player name that they chose in previous gameplay.

After this is done, the player will see the app's main menu, which allows the player to input a krag, view hints, view game stats, or request an updated set of gamestats. Each of these windows have their own functionality:

* Input Krag: presents a pin window for the user to input a krag. Press select to input krag once all pins are set
* View Hints: presents a menu of the concatenated version of the seven most recent hints received from the GA. If the user presses select on any of the hints, the full version of the hint will appear in a new window.
* View Game Stats: presents a menu of the game's game statistics. This includes the team name, the player's name, the number of claimed krags, and the number of total krags.
* Request Updates: when selected, the pebble gets an updated version of the game stats and automatically plugs it in under "View Game Stats." A window will pop up telling the user that an updated version is available.

Upon GAME_OVER, a game over window will appear when the game is ended and will show a summary of the game's final stats.

Other notes:

* A few windows have the back button disabled - this is so that the user doesn't click back to a previous screen (or leave the game) if they are not supposed to. This makes edge cases like accidental clicks less likely to happen. Of course, holding down the back button will always allow the player to exit the game.