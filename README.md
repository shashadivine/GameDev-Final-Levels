# Little Red Riding Hood
Little Red Riding Hood is an expansion of the Group's Homework 4 Level Creation. It introduces little red as the player character. The player ventures into a deep, dark forest to search for their grandmother. Along the journey, the player encounter wolves that prey against them, culminating in a tense encounter with ghosts in an eerie, abandoned mansion. 

I have included a demo video of the game in case the code cannot be executed on your machine or if you encounter any technical difficulties. Otherwise, I hope you enjoy our game :).

# The game is made up of 2 levels:
**Level 1**
Advancing to LEVEL 2 requires the player to kill all 3 enemies and navigate the vast forest and find the stairs that will lead to the haunted mansion. The player have to be cautious  of the mud tiles that slow them down and of the very danger that lies ahead--of wolves wandering around the forest, ready to strike at any moment. The player will lose if you get killed by the enemies.

**Level 2**
Set in the abandoned mansion. Some of the enemies in LEVEL 2 have a larger detection range than those in LEVEL 1. The player WINS when they kill ALL of the enemies. The player will also lose if you get killed by the enemies.


# Prerequisites 
	Raylib Library
  	C++

# Running the Game 
**Windows**
- Check if the "player_state_machine.cpp" and "enemy_state_machine.cpp" are included in "scenes.h"
- Compile and run the game 

**MacOS**
 1. Install raylib on your machine.
 2. Download the repository or clone it.
 3. Navigate to main.cpp and run it. Follow the following steps:
- An error message "The preLaunchTask 'Build.OSX' terminated with exit code 2." Choose 'Debug Anyway' and then select "Open 'launch.json'"
- Refer to this repository: https://github.com/shashadivine/Raylib-Mac/tree/vscode-folder-content and copy paste the contents to the respective files. Usually 'settings.json' and 'c_cpp_properties.json' are not present, so make sure to add them in the .vscode folder as well.
	 

# Controls 
	W/A/S/D - movement 
	space - dodge 
	left mouse button - attack 

# Credits 
	Art by Sol Requiz 
	Backend by Prue Evangelista, Shasha Lee, and Sol Requiz
  	UI by Shasha Lee
