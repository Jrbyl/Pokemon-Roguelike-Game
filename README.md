Jon Beltzhoover
beltzj03@iastate.edu

COM S 327 Pokemon Roguelike Project

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DISCLAIMER: Assignment 1.03 code was posted to Piazza for other students to use
            Assignment 1.04 code was posted to Piazza for other students to use
            Assignment 1.05 code was posted to Piazza for other students to use
            Assignment 1.06 code was posted to Piazza for other students to use
            Assignment 1.07 code was posted to Piazza for other students to use
            Assignment 1.08 code was posted to Piazza for other students to use
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Assignment 1.09: Pokemon Battles

    Description:

    Starting from progress made in Assignment 1.08: Loading Pokemon, this part of the project involved creating
    the battle interface for battling/capturing wild Pokemon and challenging trainers. This required allowing
    Pokemon to use moves, the player to use items, and be able to switch Pokemon in/out of battle. In addition,
    the player is able to flee battle, but only against wild Pokemon.

    Instructions:

    New controls were added for battle, which are shown in the interface

    To compile: g++ Main.cpp heap.c -o main -lncurses
    To run code with the numtrainers switch: ./main --numtrainers X ('X' must be a positive integer input by user)
    To run code without switch (default 10 npcs): ./main
    Run Makefile: make Main

    When run: Use controls from poke327_s2024_assignment1.05.pdf, including 'f' to "fly"

Assignment 1.08: Loading Pokemon

    Description:

    Starting from the progress made in Assignment 1.07: CSV Parsing, this part of the project involved adding
    player Pokemon, trainer Pokemon, and wild Pokemon to the world with their associated data (level, moveset,
    etc.). Wild Pokemon will appear out of the tall grass at a 10% chance, showing a detailed view of their
    level, typing, learned moves, and stats. When battling a trainer, the same detailed view will show
    for every member of their team. Laslty, the player is able to choose their very own starter, from
    three selected at random, to add to their team.

    Instructions:

    To compile: g++ Main.cpp heap.c -o main -lncurses
    To run code with the numtrainers switch: ./main --numtrainers X ('X' must be a positive integer input by user)
    To run code without switch (default 10 npcs): ./main
    Run Makefile: make Main

    When run: Use controls from poke327_s2024_assignment1.05.pdf, including 'f' to "fly"

Assignment 1.07: CSV Parsing

    Description:

    Starting from the progress made in Assignment 1.06: Moving to Neighboring Maps and Porting to C++, this
    part of the project involved finally getting all of the data for our Pokemon. Using a shared database,
    .csv files were read to get the data for all Pokemon up to Generation 8. Each file had its own designated
    class to store said information. For this assignment, regular game functions are not available, but are
    still compiled at runtime. Lastly, to pick the file to be read, use the name of the file, without the .csv,
    as the switch (eg. ./main "fileName").

    Instructions:

    To compile: g++ Main.cpp heap.c -o main -lncurses
    To run code with the file switch, use any of the following: ./main pokemon
                                                                ./main moves
                                                                ./main pokemon_moves
                                                                ./main pokemon_species
                                                                ./main experience
                                                                ./main type_names
                                                                ./main pokemon_stats
                                                                ./main stats
                                                                ./main pokemon_types
    Run Makefile: make Main

Assignment 1.06: Moving to Neighboring Maps and Porting to C++

    Description:

    Starting from the progress made in Assignment 1.05: User Interface, this part of the project involved
    porting the entire thing into C++. To do this, all C structs had to made into C++ classes, which allowed 
    inheritance to be extremely useful. In addition, the fly feature has been reintroduced, which allows
    the user to "fly" or "teleport" to a set of coordinates. Lastly, the player can now move between maps
    with ease and player data (i.e. defeated trainers) is saved per map in the world (of maps).

    Instructions:

    To compile: g++ Main.cpp heap.c -o main -lncurses
    To run code with the numtrainers switch: ./main --numtrainers X ('X' must be a positive integer input by user)
    To run code without switch (default 10 npcs): ./main
    Run Makefile: make Main

    When run: Use controls from poke327_s2024_assignment1.05.pdf, including 'f' to "fly"

Assignment 1.05: User Interface

    Description:

    Starting from the progress made in Assignment 1.04: Player Character and Trainers, this part of the 
    project involved creating the user interface so that the player had the ability to move, initiate battle,
    open the Pokemart/Pokecenter, and view a list of all the trainers on the map. So, the player can now move 
    in any direction or choose to stand still and let a turn play out. In addition, when the player runs into 
    an npc a battle is initiated, indicated by a pop-up window. Lastly, when the player stands over a Pokemart 
    or Pokecenter they have the option to open the menu for the respective building.

    Instructions:

    To compile: gcc Main.c heap.c -o main -lncurses
    To run code with the numtrainers switch: ./main --numtrainers X ('X' must be a positive integer input by user)
    To run code without switch (default 10 npcs): ./main
    Run Makefile: make Main

    When run: Use controls from poke327_s2024_assignment1.05.pdf

Assignment 1.04: Player Character and Trainers

    Description:

    Starting from progress made in Assignment 1.03: Path Finding, this part of the project involved
    putting the hiker and rival, the path finding npcs, as well as the pacer, wanderer, explorer, and 
    sentry on the map and able to move. The pacer is randomly set to move either up/down or left/right,
    based on a random number, until it hits an infinite cost barrier in which it will turn around. The 
    explorer operates on similar code as the pacer, except it can move in any of the eight directions
    around it. The wanderer uses the exact same code as the explorer, except it is bound to the terrain 
    it spawns in. Lastly, the sentry stands in one spot, unable to move. In conclusion, the current game
    is run by a priority queue which determines which npc on the map has the highest priority and the lowest
    cost, in which that npc will go first, followed by the next. 

    Instructions:

    To compile: gcc heap.c Main.c -o main
    To run code with the numtrainers switch: ./main --numtrainers X ('X' must be a positive integer input by user)
    To run code without switch (default 10 npcs): ./main
    Run Makefile: make Main

    NOTE: The described error below has likely been fixed, but I have been unable to replicate it:
          Code runs perfectly with 10 or less npcs, but there is a small chance of error when using more
          than 10 npcs. If an error, noted in CHANGELOG, were to occur, please recompile as the chance
          of error is low, but not impossible. (I want to show that it does work)

Assignment 1.03: Path Finding

    Description:

    Starting from progress made in Assignment 1.02: Moving from map to map, this part of the project 
    involved creating the path finding algorithms for the hiker and the rival character, which we will
    eventually implement to chase the player and challenge them to a Pokemon battle. As of the present, 
    the player character, denoted by '@', will randomly spawn on the road and the movement costs of the
    hiker and rival will display under the current map. In addition, these NPC's are incapable of
    passing the border or accessing the gates. The border/gates are not printed as part of the algorithm
    since neither the rival or hiker can bypass them.

    Instructions:

    No added controls were made since Assignment 1.02: Moving from map to map. When compiled, the 
    Dijkstra's for the hiker and rival will show below the command line. Compile with gcc Main.c -o main.
    Run with ./main.

Assignment 1.02: Moving from map to map

    Description:

    Starting from the progress already made in Assignment 1.01: Terrain Generation, this part
    of the project involved creating user inputs and expanding the size of the game map. To 
    start, a world map of 401 x 401 maps was created, with (200, 200) being the center, internally.
    Externally, the user can see the coordinates in a range of x:(-200, 200) y:(-200, 200) with 
    (0, 0) being the center. The user can now use inputs such as 'n', 's', 'e', 'w', 'f', or 'q'
    to either move in a cardinal direction, fly to a set of coordinates, or quit the game. Lastly, 
    the egdes of the world are impassable and the spawn rate of PokeCenters and PokeMarts decrease
    as you get farther from the center of the map.

    Instructions:

    When prompted, use 'n', 's', 'e', or 'w' to travel in one of the four cardinal directions.
    To use "fly," type 'f' followed by two coordinates (ex. f 100 100) to travel to the given 
    coordinates. Any coordinates given out of bounds will simply re-display the current map. 
    Type 'q' to quit the game.

Assignment 1.01: Terrain Generation

    Description:

    This starting point of the project involved generating the natural terrain the 
    player will walk over to catch Pokemon and battle trainers! Everything being completely
    random, the north-south and east-west roads will be in a new spot every time a
    map is generated. In addition, the PokeCenters and PokeMarts will spawn connected to
    the road, to make it easily accessible to the player. Lastly, the natural terrain is 
    different every time, including clearings, tall grass, mountains, water, and forests!

    Instructions:

    Simply compile and run the code. No specific buttons need to be pressed yet
    as there is no playable character, trainer battles, or Pokemon to catch!