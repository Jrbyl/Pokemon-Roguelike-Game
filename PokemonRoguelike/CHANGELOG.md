Jon Beltzhoover
beltzj03@iastate.edu

COM S 327 Pokemon Roguelike Project

Assignment 1.09: Pokemon Battles

    April 14th, 2024
        9:57 pm: Optimized code from assignment 1.08, though random seg fault bug still remains
    
    April 15th, 2024
        12:09 pm: Fixed bug with random seg fault
        12:15 pm: Created a class that holds all team data for the player
        1:03 pm: Adding the same functionality above for all NPCs
        3:54 pm: PokeMarts and PokeCenters are now functional

    April 16th, 2024
        6:17 pm: User interface constructed, but not yet functional
        9:57 pm: Optimized user interface to now take inputs, but does not progress battle

    April 17th, 2024
        11:09 am: Player can now catch Pokemon
        11:11 am: Player can now flee a wild Pokemon battle
        11:49 am: Player can now switch Pokemon in a battle
        2:13 pm: Player can now attack wild Pokemon and be attacked
        5:16 pm: Player can now use Potions
        5:58 pm: If the Players current Pokemon is knocked out, they can choose one to switch in
        9:12 pm: Found a bug where once a revive is used, that Pokemon can *sometimes* not be switched to
        9:19 pm: Player can now see the number of Pokemon a trainer has
        10:05 pm: Found a potential bug where damage may not be accurate

Assignment 1.08: Loading Pokemon

    April 8th, 2024
        4:12 pm: Temporarily removed feature for printing file info

    April 9th, 2024
        8:19 pm: Wild Pokemon spawn
        8:43 pm: Wild Pokemon will now be assigned a level based on (x, y)
        9:12 pm: Wild Pokemon now have a detailed view, including typing, moves, and stats

    April 10th, 2024
        11:22 am: User can now pick a Pokemon starter upon running the game ('w'/'s' to move up/down)
        11:53 am: Added the level up feature
        2:22 pm: Rival now has Pokemon
        3:10 pm: Hiker now has Pokemon
        3:17 pm: Other npc's now have Pokemon
        4:01 pm: Battling trainers now shows their entire team w/detailed view
        4:05 pm: Found a bug where attempting to move out of (0, 0) causes a segmentation fault

    April 11th, 2024
        3:39 am: Previous bug was unable to be fixed, comment out line 2626 in Main.cpp to test other
                 functionality besides Trainer battles 

Assignment 1.07: CSV Parsing
    
    April 1st, 2024
        5:06 pm: Created class for pokemon.csv
        5:17 pm: Created class for moves.csv
        5:29 pm: Created class for pokemon_moves.csv
        5:37 pm: Created class for pokemon_species.csv
        5:38 pm: Created class for experience.csv
        5:41 pm: Created class for type_names.csv
        5:43 pm: Created class for pokemon_stats.csv
        5:45 pm: Created class for stats.csv
        5:58 pm: Created class for pokemon_types.csv
        9:30 pm: Successfully stored and printed all .csv files to output
        9:44 pm: If first file path fails, will attempt a second file path before outputting error or printing file contents

    April 2nd, 2024
        2:46 pm: Found a bug where all valid integers/strings after an empty space register as INT_MAX
        6:43 pm: Fixed the bug where all values after an empty space would register as INT_MAX
        7:25 pm: Found a bug where an extra line is printed with all empty commas

    April 3rd, 2024
        1:20 pm: Fixed the bug where an extra line is printed with all commas
        1:22 pm: All data is able to be printed with correct format

Assignment 1.06: Moving to Neighboring Maps and Porting to C++
    March 24th, 2024
        1:23 pm: Started writing Moving to Neighboring Maps and Porting to C++
        3:41 pm: Changed structs from C version to classes in the C++ version

    March 25th, 2024
        2:50 pm: Made all class variables public
        3:02 pm: Type casted and added getter/setter methods as necessary
        3:10 pm: Port to C++ complete
        5:42 pm: PC can now travel between maps via the gates
        5:51 pm: Found a bug where PC will randomly spawn when they visit a new map, but works 
                 as expected after
        
    March 26th, 2024
        3:58 pm: PC can now move between maps as expected
        4:05 pm: Found a bug where the PC will leave behind an '@' (sometimes) when revisiting a map
        6:34 pm: Fixed the bug where the PC would leave behind an '@'
        7:21 pm: PC can now use fly to move to different maps
        7:26 pm: Added a pop-up window asking the user where they want to fly so the user knows they
                 have entered the fly command

Assignment 1.05: User Interface
    
    March 14th, 2024
        4:23 pm: Started writing User Interface
        5:58 pm: Implemented basic ncurses and edited displayMap() to reflect these changes

    March 15th, 2024
        1:11 pm: User can now move in any direction or choose to stay still for a turn
        1:53 pm: User can now initiate a Pokemon battle and leave them
        3:15 pm: User can now enter Pokemarts and Pokecenters, and leave them
        3:17 pm: User can now quit the game
    
    March 18th, 2024
        9:34 pm: Made it so that the user cannot rechallenge already battled trainers
        10:02 pm: Hiker and rival will now follow the player as they move

    March 19th, 2024
        1:14 pm: User can now see the trainer list with correct npc type and direction
        1:38 pm: Added messages at the top of the screen when the player does an important action
        2:30 pm: Hiker and Rival will path similar to explorer when defeated
        5:41 pm: Found a bug where attempting to scroll the trainer list scrolls the entire window

    March 20th, 2024
        6:44 pm: fixed bug with scrolling
        7:18 pm: Added a feature that ensures hiker and rival always appear at the top of the trainer list
                (since they are the most important, I want them to be seen)
        7:20 pm: Found a bug where it repeats the last character in the list 7 times
        8:42 pm: Fixed repeated trainer list bug
        8:43 pm: ready for turn in

Assignment 1.04: Player Character and Trainers

    February 22nd, 2024
        7:50 pm: Posted my Assignment 1.03 code to Piazza for other students to use
        8:17 pm: Started writing Player Character and Trainers
        8:30 pm: Created variables for hiker, rival, pacers, wanderers, explorers, and sentries

    February 24th, 2024
        4:14 pm: Characters now spawn on the map randomly (not in border/gate)
        8:38 pm: The pacer can now path correctly (up/down or left/right, depending on a random value)

    February 25th, 2024
        4:25 pm: The wanderer can now path correctly
        5:55 pm: The explorer can now path correctly
        6:04 pm: Tweaked code that handled character moving into borders/gates
        6:07 pm: Found a bug where sometimes (happened ~3 times in 15 different scenarios) a segmentation fault occurs during character movement

    February 26th, 2024
        5:26 pm: Tweaked character movement code
        7:51 pm: The hiker can now path correctly

    February 27th, 2024
        1:36 pm: The rival can now path correctly
        4:53 pm: Added the --numtrainers switch
        5:45 pm: For some reason, wanderer and pacer pathing does not path, at all
        9:04 pm: Found a bug where the --numtrainers switch causes a segmentation fault

    February 28th, 2024
        12:52 am: Pacer can now correctly path, again
        1:04 am: Wanderer can now correctly path, again
        1:07 pm: Optimized code for adding trainers using the --numtrainers switch, fixing the previous bug with --numtrainers
        2:02 pm: Successfully implemented priority queue using the prof. Shaeffer's heap
        2:46 pm: Found a bug with the priority queue that caused only 1 turn to occur
        4:48 pm: Fixed the priority queue bug, so now turns progress normally
        4:53 pm: Fixed bug that caused segmentation faults
        9:34 pm: Closing notes before due date, error is likely fixed but:
                - Spawning 15 npcs or below allows the program to run error free
                - Spawning 15 npcs or above has the chance for some pieces of terrain to get overridden with a character type (not actually the character, just the letter)
                - The above errors were last minute finds and will be fixed before Assignment 1.05 is they are to still persist

Assignment 1.03: Path Finding

    February 12th, 2024
        3:45 pm: Player is now randomly spawned, somewhere on the road
        8:30 pm: Found a bug that caused Dijkstra's to result in segmentation fault

    February 13th, 2024
        2:05 pm: Created structs that hold character type (player, hiker, rival) and their associated info/costs
        7:15 pm: Implemented Dijkstra's
        9:00 pm: Found a bug where Dijkstra shows incorrect values, only for the hiker
        9:05 pm: Fixed bug where Dijkstra values were incorrect for the hiker (it was a typo)

    February 14th, 2024
        12:30 pm: Found a bug where Dijkstra values show incorrectly the first time a map is visited, but show correctly ever visit after
        5:17 pm: Fixed the bug where Dijkstra values were incorrect first time a map was visited

    February 15th, 2024
        12:15 pm: Found a bug where a segmentation fault occurs when going west

    February 19th, 2024
        2:26 pm: Fixed the bug where segmentation fault occurs when going west
        3:14 pm: Optimized code for memeory allocation and freeing memory

        *All bugs mentioned in this section have been fixed!*

Assignment 1.02: Moving from map to map
    
    January 30th, 2024
        9:45 pm: Started writing Moving from map to map
        10:06 pm: Struggling with pointers
        10:12 pm: Cleaned up code from Assignment 1.01 (overcame the previous struggle)

    February 3rd, 2024
        4:25 pm: Created user inputs on command line
        4:38 pm: Found a bug where user input buffers and doesn't print error message

    February 4th, 2024
        3:05 pm: Fixed bug where user input buffers
    
    February 5th, 2024
        2:43 pm: Found a bug where west gate does not correlate with newly generated map (other directions work fine)
        3:30 pm: Stored generated maps in 2d array "world"
        10:16 pm: Fixed the bug where west gate would not correlate with newly generated map

    February 6th, 2024
        7:46 pm: Travel between maps is now possible and roads connect between them
        10:14 pm: Made it so that incorrect inputs simply clear the command line while everything else stays intact
        11:47 pm: PokeCenters and PokeMarts will now spawn according to the formula (((-45d) / 200) + 50) /100, where 'd' is the Manhattan distance from the center

    February 7th, 2024
        1:05 pm: World borders will now all be impassable
        1:20 pm: Tweaked fly so that internal coordinates match up with user coordinates (before, entering f 200 200 would take you to (0, 0))

        *All bugs mentioned in this section have been fixed!*

Assignment 1.01: Terrain Generation
    
    January 27th, 2024
        3:45 pm: Started writing Terrain Generation
        4:00 pm: Created map border and declared region variables
        4:28 pm: Created a funcion to print the map

    January 28th, 2024
        6:22 pm: Found a bug where road goes over border
        6:38 pm: Fixed bug where road goes over border
        6:48 pm: Found a bug where road goes into border multiple times instead of the other gate
        7:51 pm: Fixed bug where road goes into border multiple times
        8:40 pm: Completed generation of East-West road
        9:07 pm: Completed generation of North-South road

    January 29th, 2024
        8:56 pm: Started generation of PokeCenters and PokeMarts
        9:38 pm: Found a bug where PokeCenter will sometimes replace road if N-S and E-W roads intersect 
                 near each other
        10:12 pm: Fixed bug where PokeCenter replaces road
        10:51 pm: Completed generation of PokeCenters and PokeMarts
        10:54 pm: Started generation of natural terrain

    January 30th, 2024
        2:56 pm: Completed generation of natural terrain
        6:51 pm: Natural terrain now generates in a cleaner/smoother way
        6:56 pm: Submission of Assignment 1.01

        *All bugs mentioned in this section have been fixed!*