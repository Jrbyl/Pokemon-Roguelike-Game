#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>
#include <cmath>
#include <cstring>
#include <string>
#include <unistd.h>
#include <ncurses.h>
#include <algorithm>
#include "heap.h"
#define WORLD 401
#define ROWS 21
#define COLS 80
#define POTION 20
#define SGRASS '.'
#define TGRASS ':'
#define FOREST '^'
#define WATER '~'
#define ROCK '%'
#define BORDER '%'
#define ROAD '#'
#define GATE '#'
#define CENTER 'C'
#define MART 'M'
#define PLAYER '@'
#define INF 99999999
#define RIVAL 'r'
#define HIKER 'h'
#define PACER 'p'
#define WANDERER 'w'
#define SENTRY 's'
#define EXPLORER 'e'

using namespace std;

class pokemon {
    public:
     int id;
     string identifier;
     int species_id;
     int height;
     int weight;
     int base_experience;
     int order;
     int is_default;

     vector<string> typing;
     vector<string> moves;
     vector<int> stats;
     vector<int> ivs;
     int currentHP;
     int currentEXP;
     int growth_id;
     int lvl;
     int index;
     bool shiny;
     bool ko;
};

class moves {
    public:
     int id;
     string identifier;
     int generation_id;
     int type_id;
     int power;
     int pp;
     int accuracy;
     int priority;
     int target_id;
     int damage_class_id;
     int effect_id;
     int effect_chance;
     int contest_type_id;
     int contest_effect_id;
     int super_contest_effect_id;
};

class pokemon_moves {
    public:
     int pokemon_id;
     int version_group_id;
     int move_id;
     int pokemon_move_method_id;
     int level;
     int order;
};

class pokemon_species {
    public:
     int id;
     string identifier;
     int generation_id;
     int evolves_from_species_id;
     int evolution_chain_id;
     int color_id;
     int shape_id;
     int habitat_id;
     int gender_rate;
     int capture_rate;
     int base_happiness;
     int is_baby;
     int hatch_counter;
     int has_gender_differences;
     int growth_rate_id;
     int forms_switchable;
     int is_legendary;
     int is_mythical;
     int order;
     int conquest_order;
};

class experience {
    public:
     int growth_rate_id;
     int level;
     int experience;
};

class type_names {
    public:
     int type_id;
     int local_language_id;
     string name;
};

class pokemon_stats {
    public:
     int pokemon_id;
     int stat_id;
     int base_stat;
     int effort;
};

class stats {
    public:
     int id;
     int damage_class_id;
     string identifier;
     int is_battle_only;
     int game_index;
};

class pokemon_types {
    public:
     int pokemon_id;
     int type_id;
     int slot;
};

class pokemon_Data {
    public:
     vector<pokemon> poke;
     vector<moves> move;
     vector<pokemon_moves> poke_move;
     vector<pokemon_species> poke_spec;
     vector<experience> exp;
     vector<type_names> type_name;
     vector<pokemon_stats> poke_stat;
     vector<stats> stat;
     vector<pokemon_types> poke_type;
};

class coords {
    public:
     int x;
     int y;
     char *directionX, *directionY;
     int trainerX, trainerY, distanceX, distanceY;

    coords() {
        directionX = nullptr;
        directionY = nullptr;
    }

    ~coords() {
        delete[] directionX;
        delete[] directionY;
    }

    const char* getDirectionX() const {
        return directionX;
    }

    void setDirectionX(const char* newDirectionX) {
        delete[] directionX;
        directionX = new char[strlen(newDirectionX) + 1];
        strcpy(directionX, newDirectionX);
    }

    const char* getDirectionY() const {
        return directionY;
    }

    void setDirectionY(const char* newDirectionY) {
        delete[] directionY;
        directionY = new char[strlen(newDirectionY) + 1];
        strcpy(directionY, newDirectionY);
    }
};

class pc_items {
    public:
     int revives;
     int potions;
     int pokeballs;
};

class pc {
    public:
     class coords coords;
     int** cost;
     char currSpace;
     char nextSpace;
     char spawnGate;
     int currCost;
     int nextCost;
};

class player_pokemon {
    public:
     vector<pokemon> poke;
     pc_items items;
};

class hiker {
    public:
     class coords coords;
     int** cost;
     int** pathCost;
     int pathed;
     int screenPos;
     char currSpace;
     char nextSpace;
     int currCost;
     int defeated;
     int direction;
     vector<pokemon> poke;
};

class rival {
    public:
     class coords coords;
     int** cost;
     int** pathCost;
     int pathed;
     int screenPos;
     char currSpace;
     char nextSpace;
     int currCost;
     int defeated;
     int direction;
     vector<pokemon> poke;
};

class non_pc {
    public:
     char npcType;
     class coords coords;
     char currSpace;
     char nextSpace;
     int currCost;
     char homeTerrain;
     int direction;
     int defeated;
     int screenPos;
     vector<pokemon> poke;
};

class map {
    public:
     int** map;
     int visited;
     class pc player;
     int** generalCost;
     //class npc* npc;
     vector<non_pc> npc;
     class hiker hiker;
     class rival rival;
};

class gates {
    public:
     int X1;
     int Y1;
     int X2;
     int Y2; 
};

class path_t {
    public:
     heap_node_t *hn;
     uint8_t pos[2];
     int32_t cost;
};

class cost_t {
    public:
     heap_node_t *hn;
     int32_t cost;
};

enum dim_t {
     dim_x,
     dim_y,
     num_dims
};

map world[WORLD][WORLD];
gates gate[WORLD][WORLD];
pokemon_Data pokemonData;
player_pokemon pc_team;
char usrInput;
int usrX;
int usrY;

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t cost_cmp(const void *key, const void *with) {
  return ((cost_t *) key)->cost - ((cost_t *) with)->cost;
}

void setRivalCosts(int x, int y) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (world[y][x].map[i][j] == BORDER || world[y][x].map[i][j] == ROCK || world[y][x].map[i][j] == WATER || world[y][x].map[i][j] == FOREST) {
                world[y][x].rival.cost[i][j] = INF;
                world[y][x].generalCost[i][j] = INF;
            }
            if (world[y][x].map[i][j] == ROAD || world[y][x].map[i][j] == SGRASS) {
                world[y][x].rival.cost[i][j] = 10;
                world[y][x].generalCost[i][j] = 10;
            }
            if (world[y][x].map[i][j] == MART || world[y][x].map[i][j] == CENTER) {
                world[y][x].rival.cost[i][j] = 50;
                world[y][x].generalCost[i][j] = 50;
            }
            if (world[y][x].map[i][j] == TGRASS) {
                world[y][x].rival.cost[i][j] = 20;
                world[y][x].generalCost[i][j] = 20;
            }

            if (i == 0 || i == ROWS - 1) {
                world[y][x].rival.cost[i][j] = INF;
                world[y][x].generalCost[i][j] = INF;
            }
            if (j == 0 || j == COLS - 1) {
                world[y][x].rival.cost[i][j] = INF;
                world[y][x].generalCost[i][j] = INF;
            }
        }
    }
}

void setHikerCosts(int x, int y) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (world[y][x].map[i][j] == BORDER || world[y][x].map[i][j] == WATER) {
                world[y][x].hiker.cost[i][j] = INF;
            }
            if (world[y][x].map[i][j] == ROAD || world[y][x].map[i][j] == SGRASS) {
                world[y][x].hiker.cost[i][j] = 10;
            }
            if (world[y][x].map[i][j] == MART || world[y][x].map[i][j] == CENTER) {
                world[y][x].hiker.cost[i][j] = 50;
            }
            if (world[y][x].map[i][j] == TGRASS || world[y][x].map[i][j] == ROCK || world[y][x].map[i][j] == FOREST) {
                world[y][x].hiker.cost[i][j] = 15;
            }

            if (i == 0 || i == ROWS - 1) {
                world[y][x].hiker.cost[i][j] = INF;
            }
            if (j == 0 || j == COLS - 1) {
                world[y][x].hiker.cost[i][j] = INF;
            }
        }
    }
}

void setPlayerCosts(int x, int y) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (world[y][x].map[i][j] == BORDER || world[y][x].map[i][j] == ROCK || world[y][x].map[i][j] == WATER || world[y][x].map[i][j] == FOREST) {
                world[y][x].player.cost[i][j] = INF;
            }
            if (world[y][x].map[i][j] == ROAD || world[y][x].map[i][j] == SGRASS || world[y][x].map[i][j] == MART || world[y][x].map[i][j] == CENTER) {
                world[y][x].player.cost[i][j] = 10;
            }
            if (world[y][x].map[i][j] == TGRASS) {
                world[y][x].player.cost[i][j] = 20;
            }

            if (i == 0 || i == ROWS - 1) {
                world[y][x].player.cost[i][j] = INF;
            }
            if (j == 0 || j == COLS - 1) {
                world[y][x].player.cost[i][j] = INF;
            }
        }
    }
}

void dijkstra_pathing(int xOrigin, int yOrigin, int xCoord, int yCoord, char npc) {
    
    static path_t path[ROWS][COLS], *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y;

    if (!initialized) {
        for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLS; x++) {
            path[y][x].pos[dim_y] = y;
            path[y][x].pos[dim_x] = x;
        }
        }
        initialized = 1;
    }
    
    for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLS; x++) {
        path[y][x].cost = INF;
        }
    }

    path[yOrigin][xOrigin].cost = 0;

    heap_init(&h, path_cmp, NULL);

    for (y = 1; y < ROWS - 1; y++) {
        for (x = 1; x < COLS - 1; x++) {
            if (world[yCoord][xCoord].map[y][x] != BORDER || world[yCoord][xCoord].map[y][x] != WATER) {
                path[y][x].hn = heap_insert(&h, &path[y][x]);
            }
        }
    }

    if (npc == RIVAL) {
        while ((p = (path_t *)heap_remove_min(&h))) {
            p->hn = NULL;
            //Down
            if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y] - 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y] - 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
            }
            //Left
            if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y]][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y]][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
            }
            //Right
            if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y]][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y]][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
            }
            //Up
            if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y] + 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y] + 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
            }
            //Down/Left
            if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y] - 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y] - 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
            }
            //Down/Right
            if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y] - 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y] - 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
            }
            //Up/Right
            if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y] + 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y] + 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
            }
            //Up/Left
            if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost > world[yCoord][xCoord].rival.cost[p->pos[dim_y] + 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = world[yCoord][xCoord].rival.cost[p->pos[dim_y] + 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
            }
        }

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                world[yCoord][xCoord].rival.pathCost[i][j] = path[i][j].cost;
            }
        }
        //world[yCoord][xCoord].rival.pathed = 1;
    }
    if (npc == HIKER) {
        while ((p = (path_t *)heap_remove_min(&h))) {
            p->hn = NULL;
            //Down
            if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y] - 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y] - 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
            }
            //Left
            if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y]][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y]][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
            }
            //Right
            if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y]][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y]][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
            }
            //Up
            if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y] + 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y] + 1][p->pos[dim_x]] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
            }
            //Down/Left
            if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y] - 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y] - 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
            }
            //Down/Right
            if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y] - 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y] - 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
            }
            //Up/Right
            if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y] + 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y] + 1][p->pos[dim_x] + 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
            }
            //Up/Left
            if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost > world[yCoord][xCoord].hiker.cost[p->pos[dim_y] + 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost)) {
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = world[yCoord][xCoord].hiker.cost[p->pos[dim_y] + 1][p->pos[dim_x] - 1] + path[p->pos[dim_y]][p->pos[dim_x]].cost;
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
            }
        }

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                world[yCoord][xCoord].hiker.pathCost[i][j] = path[i][j].cost;
            }
        }
        //world[yCoord][xCoord].hiker.pathed = 1;
    }
}

void generateBorder(int xCoord, int yCoord) {
    
    //generate border
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i > 0 && i < (ROWS - 1) && j > 0 && j < (COLS - 1)) {
                world[yCoord][xCoord].map[i][j] = SGRASS;
            }
            else {
                world[yCoord][xCoord].map[i][j] = BORDER;
            }
        }
    }

}

void generateTerrain(int xCoord, int yCoord) {
    //generate regions
    const char region[10] = {TGRASS, FOREST, TGRASS, ROCK, WATER, TGRASS, WATER, ROCK, FOREST, TGRASS};
    for(int i = 0; i < 35; i++){
        int randX = rand() % (COLS - 2) + 1;
        int randY = rand() % (ROWS - 2) + 1;
        int randHeight = rand() % (ROWS - 2) + 1; 
        int randWidth = rand() % (COLS - 2) + 1;
        int randTerrain = rand() % 10;
        for(int j = randY; j < randHeight + randY && j < ROWS-1; j++){
            for(int k = randX; k < randWidth + randX && k < COLS-1; k++){
                world[yCoord][xCoord].map[j][k] = region[randTerrain];
            }
        }
    }
    //ensures there is at least one TGRASS and one SGRASS region
    char setRegion[3] = {TGRASS, SGRASS, TGRASS};
    for(int i = 0; i < 3; i++){
        int randX = rand() % (COLS - 2) + 1;
        int randY = rand() % (ROWS - 2) + 1;
        int randHeight = rand() % (ROWS - 2) + 1; 
        int randWidth = rand() % (COLS - 2) + 1;
        for(int j = randY; j < randHeight + randY && j < ROWS-1; j++){
            for(int k = randX; k < randWidth + randX && k < COLS-1; k++){
                world[yCoord][xCoord].map[j][k] = setRegion[i];
            }
        }
    }
}

void checkGates(int xCoord, int yCoord) {
    //Temp variables for holding old gate values (four for readability)
    int tmpX1, tmpY1, tmpX2, tmpY2;

    //Checks West
    if (world[yCoord][xCoord - 1].visited == 1) {
        tmpY1 = gate[yCoord][xCoord - 1].Y2;
        gate[yCoord][xCoord].Y1 = tmpY1;
    }
    //Checks South
    if (world[yCoord - 1][xCoord].visited == 1) {
        tmpX2 = gate[yCoord - 1][xCoord].X1;
        gate[yCoord][xCoord].X2 = tmpX2;
    }
    //Checks East
    if (world[yCoord][xCoord + 1].visited == 1) {
        tmpY2 = gate[yCoord][xCoord + 1].Y1;
        gate[yCoord][xCoord].Y2 = tmpY2;
    }
    //Checks North
    if (world[yCoord + 1][xCoord].visited == 1) {
        tmpX1 = gate[yCoord + 1][xCoord].X2;
        gate[yCoord][xCoord].X1 = tmpX1;
    }
}

void generateRoads(int xIntersect, int yIntersect, int xCoord, int yCoord) {
    //generate road
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            //East-West Road
            if (i == gate[yCoord][xCoord].Y1 && j <= xIntersect) {
                if (j == 0) {
                    world[yCoord][xCoord].map[i][j] = GATE;
                }
                else {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            if (i == gate[yCoord][xCoord].Y2 && j >= xIntersect) {
                if (j == (COLS - 1)) {
                    world[yCoord][xCoord].map[i][j] = GATE;
                }
                else {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            if (gate[yCoord][xCoord].Y1 < gate[yCoord][xCoord].Y2) {
                if (j == xIntersect && i >= gate[yCoord][xCoord].Y1 && i <= gate[yCoord][xCoord].Y2) {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            else {
                if (j == xIntersect && i <= gate[yCoord][xCoord].Y1 && i >= gate[yCoord][xCoord].Y2) {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            
            //North-South Road
            if (j == gate[yCoord][xCoord].X1 && i <= yIntersect) {
                if (i == 0) {
                    world[yCoord][xCoord].map[i][j] = GATE;
                }
                else {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            if (j == gate[yCoord][xCoord].X2 && i >= yIntersect) {
                if (i == (COLS - 1)) {
                    world[yCoord][xCoord].map[i][j] = GATE;
                }
                else {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            if (gate[yCoord][xCoord].X1 < gate[yCoord][xCoord].X2) {
                if (i == yIntersect && j >= gate[yCoord][xCoord].X1 && j <= gate[yCoord][xCoord].X2) {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
            else {
                if (i == yIntersect && j <= gate[yCoord][xCoord].X1 && j >= gate[yCoord][xCoord].X2) {
                    world[yCoord][xCoord].map[i][j] = ROAD;
                }
            }
        }
    }

    //Create border around world (edge maps)
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (xCoord == 0 && j == 0) {
                world[yCoord][xCoord].map[i][j] = BORDER;
            }
            if (xCoord == (WORLD - 1) && j == (COLS - 1)) {
                world[yCoord][xCoord].map[i][j] = BORDER;
            }
            if (yCoord == 0 && i == (ROWS - 1)) {
                world[yCoord][xCoord].map[i][j] = BORDER;
            }
            if (yCoord == (WORLD - 1) && i == 0) {
                world[yCoord][xCoord].map[i][j] = BORDER;
            }
        }
    }
}

double manhattanDistance(int xCoord, int yCoord) {
    double manDis = abs(200 - xCoord) + abs(200 - yCoord);
    return manDis;
}

void generateBuildings(int xIntersect, int yIntersect, int xCoord, int yCoord) {
    
    // PokeCenter/Mart distance formula
    // (((-45*d) / 200) + 50) / 100
    double manDis = manhattanDistance(xCoord, yCoord);
    double spawnChance = (((-45 * manDis) / 200) + 50) / 100;
    double spawnCriteria = (double)rand() / RAND_MAX;
    
    //generate PC and PM
    int randLoc = rand() % 2;
    int cLoc, mLoc;

    if (randLoc == 0) {
         cLoc = rand() % ((xIntersect - 1) - 2 + 1) + 2;
         mLoc = rand() % (78 - (xIntersect + 1) + 1) + 2;
    }
    else {
        cLoc = rand() % (78 - (xIntersect + 1) + 1) + 2;
        mLoc = rand() % ((xIntersect - 1) - 2 + 1) + 2;
    }

    if (randLoc == 1) {
         mLoc = rand() % ((xIntersect - 1) - 2 + 1) + 2;
         cLoc = rand() % (78 - (xIntersect + 1) + 1) + 2;
    }
    else {
        mLoc = rand() % (78 - (xIntersect + 1) + 1) + 2;
        cLoc = rand() % ((xIntersect - 1) - 2 + 1) + 2;
    }

    if (cLoc == gate[yCoord][xCoord].X1 || cLoc == gate[yCoord][xCoord].X2) {
        cLoc = cLoc - 1;
    }
    if (mLoc == gate[yCoord][xCoord].X1 || mLoc == gate[yCoord][xCoord].X2) {
        mLoc = mLoc - 1;
    }
    //Checks location and determines a chance for buildings to spawn
    if ((xCoord == 200 && yCoord == 200) || spawnCriteria <= spawnChance) {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (cLoc < xIntersect) {
                    if (i == gate[yCoord][xCoord].Y1 && j == cLoc) {
                        if (world[yCoord][xCoord].map[i - 1][j] == ROAD) {
                            world[yCoord][xCoord].map[i - 2][j] = CENTER;
                        }
                        else {
                            world[yCoord][xCoord].map[i - 1][j] = CENTER;
                        }
                    }
                }
                else {
                    if (i == gate[yCoord][xCoord].Y2 && j == cLoc) {
                        if (world[yCoord][xCoord].map[i - 1][j] == ROAD) {
                            world[yCoord][xCoord].map[i - 2][j] = CENTER;
                        }
                        else {
                            world[yCoord][xCoord].map[i - 1][j] = CENTER;
                        }
                    }
                }

                if (mLoc > xIntersect) {
                    if (i == gate[yCoord][xCoord].Y2 && j == mLoc) {
                        if (world[yCoord][xCoord].map[i + 1][j] == ROAD) {
                        world[yCoord][xCoord].map[i + 2][j] = MART;
                        }
                        else {
                            world[yCoord][xCoord].map[i + 1][j] = MART;
                        }
                    }
                }
                else {
                    if (i == gate[yCoord][xCoord].Y1 && j == mLoc) {
                        if (world[yCoord][xCoord].map[i + 1][j] == ROAD) {
                            world[yCoord][xCoord].map[i + 2][j] = MART;
                        }
                        else {
                            world[yCoord][xCoord].map[i + 1][j] = MART;
                        }
                    }
                }
            }
        }
    }
}

void placeRandPC(int xIntersect, int yIntersect, int xCoord, int yCoord) {
    int randLoc = rand() % 2;

    if (randLoc == 0) {
        world[yCoord][xCoord].player.coords.y = gate[yCoord][xCoord].Y1;
        world[yCoord][xCoord].player.coords.x = rand() % ((xIntersect - 1) - 3 + 1) + 3;

        while (world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == HIKER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == RIVAL
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == PACER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == WANDERER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == SENTRY
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == EXPLORER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == FOREST
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == ROCK
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == WATER) 
        {
            world[yCoord][xCoord].player.coords.x = rand() % ((xIntersect - 1) - 3 + 1) + 3;
        }
    }
    else if (randLoc == 1) {
        world[yCoord][xCoord].player.coords.y = gate[yCoord][xCoord].Y2;
        world[yCoord][xCoord].player.coords.x = rand() % (78 - (xIntersect + 1) + 1) + (xIntersect + 1);

        while (world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == HIKER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == RIVAL
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == PACER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == WANDERER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == SENTRY
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == EXPLORER
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == FOREST
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == ROCK
            || world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] == WATER) 
        {
            world[yCoord][xCoord].player.coords.x = rand() % (78 - (xIntersect + 1) + 1) + (xIntersect + 1);
        }
    }
    world[yCoord][xCoord].player.currSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x];
    world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] = PLAYER;
    world[yCoord][xCoord].generalCost[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] = INF;
}

void placePC(int xCoord, int yCoord) {
    if (world[yCoord][xCoord].player.spawnGate == 'n') {
        world[yCoord][xCoord].player.coords.y = 1;
        world[yCoord][xCoord].player.coords.x = gate[yCoord][xCoord].X1;
        world[yCoord][xCoord].player.spawnGate = ' ';
    }
    else if (world[yCoord][xCoord].player.spawnGate == 'w') {
        world[yCoord][xCoord].player.coords.y = gate[yCoord][xCoord].Y1;
        world[yCoord][xCoord].player.coords.x = 1;
        world[yCoord][xCoord].player.spawnGate = ' ';
    }
    else if (world[yCoord][xCoord].player.spawnGate == 's') {
        world[yCoord][xCoord].player.coords.y = (ROWS - 2);
        world[yCoord][xCoord].player.coords.x = gate[yCoord][xCoord].X2;
        world[yCoord][xCoord].player.spawnGate = ' ';
    }
    else if (world[yCoord][xCoord].player.spawnGate == 'e') {
        world[yCoord][xCoord].player.coords.y = gate[yCoord][xCoord].Y2;
        world[yCoord][xCoord].player.coords.x = (COLS - 2);
        world[yCoord][xCoord].player.spawnGate = ' ';
    }
    else {
        return;
    }

    world[yCoord][xCoord].player.currSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x];
    world[yCoord][xCoord].player.currCost = world[yCoord][xCoord].player.cost[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x];
    world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] = PLAYER;
}

void placeRandNPC(int xCoord, int yCoord, int numTrainers) {
    int x, y;
    if (numTrainers > 0) {
        x = rand() % (78 - 3 + 1) + 3;
        y = rand() % (18 - 3 + 1) + 3;
        while (world[yCoord][xCoord].map[y][x] == CENTER 
        || world[yCoord][xCoord].map[y][x] == MART
        || world[yCoord][xCoord].map[y][x] == ROAD
        || world[yCoord][xCoord].map[y][x] == PLAYER
        || world[yCoord][xCoord].rival.cost[y][x] == INF
        || y == 0 || y == ROWS || x == 0 || x == COLS) {
            x = rand() % (78 - 3 + 1) + 3;
            y = rand() % (18 - 3 + 1) + 3;
        }
        world[yCoord][xCoord].rival.coords.x = x;
        world[yCoord][xCoord].rival.coords.y = y;

        world[yCoord][xCoord].rival.currSpace = world[yCoord][xCoord].map[y][x];
        world[yCoord][xCoord].rival.currCost = world[yCoord][xCoord].rival.cost[y][x];
        world[yCoord][xCoord].map[y][x] = RIVAL;
    }    

    if (numTrainers > 1) {
        x = rand() % (78 - 3 + 1) + 3;
        y = rand() % (18 - 3 + 1) + 3;
        while (world[yCoord][xCoord].hiker.cost[y][x] == INF 
        || world[yCoord][xCoord].map[y][x] == CENTER 
        || world[yCoord][xCoord].map[y][x] == MART
        || world[yCoord][xCoord].map[y][x] == ROAD
        || world[yCoord][xCoord].map[y][x] == PLAYER
        || world[yCoord][xCoord].map[y][x] == RIVAL
        || y == 0 || y == ROWS || x == 0 || x == COLS) {
            x = rand() % (78 - 3 + 1) + 3;
            y = rand() % (18 - 3 + 1) + 3;
        }
        world[yCoord][xCoord].hiker.coords.x = x;
        world[yCoord][xCoord].hiker.coords.y = y;

        world[yCoord][xCoord].hiker.currSpace = world[yCoord][xCoord].map[y][x];
        world[yCoord][xCoord].hiker.currCost = world[yCoord][xCoord].hiker.cost[y][x];
        world[yCoord][xCoord].map[y][x] = HIKER;
    }

    if (numTrainers > 2) {
        for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
            int x = rand() % (78 - 3 + 1) + 3;
            int y = rand() % (18 - 3 + 1) + 3;
            while (world[yCoord][xCoord].map[y][x] == CENTER 
            || world[yCoord][xCoord].map[y][x] == MART
            || world[yCoord][xCoord].map[y][x] == ROAD
            || world[yCoord][xCoord].map[y][x] == PLAYER
            || world[yCoord][xCoord].map[y][x] == RIVAL
            || world[yCoord][xCoord].map[y][x] == HIKER
            || world[yCoord][xCoord].map[y][x] == PACER
            || world[yCoord][xCoord].map[y][x] == EXPLORER
            || world[yCoord][xCoord].map[y][x] == WANDERER
            || world[yCoord][xCoord].map[y][x] == SENTRY
            || world[yCoord][xCoord].generalCost[y][x] == INF
            || y == 0 || y == ROWS || x == 0 || x == COLS) {
                x = rand() % (78 - 3 + 1) + 3;
                y = rand() % (18 - 3 + 1) + 3;
            }
            world[yCoord][xCoord].npc[i].coords.x = x;
            world[yCoord][xCoord].npc[i].coords.y = y;

            world[yCoord][xCoord].npc[i].currSpace = world[yCoord][xCoord].map[y][x];
            world[yCoord][xCoord].npc[i].currCost = world[yCoord][xCoord].generalCost[y][x];
            world[yCoord][xCoord].map[y][x] = world[yCoord][xCoord].npc[i].npcType;
        }
    }
}

void displayMap(int xCoord, int yCoord, WINDOW *win) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            mvwprintw(win, i + 1, j, "%c", world[yCoord][xCoord].map[i][j]);
        }
    }
    mvwprintw(win, 22, 0, "Location: (%d, %d)  Command:                                                    ", xCoord - 200, yCoord - 200);
    wrefresh(win);
    refresh();
}

void pacerTravel(int xCoord, int yCoord, int index) {
        if (world[yCoord][xCoord].npc[index].direction == 0 || world[yCoord][xCoord].npc[index].direction == 1) {
            if (world[yCoord][xCoord].npc[index].direction == 0 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != INF
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != WANDERER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != SENTRY
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != HIKER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != RIVAL
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != EXPLORER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != PLAYER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != PACER)
            {
                world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
                world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
                world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
                world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = PACER;
            }
            else
            {
                world[yCoord][xCoord].npc[index].direction = 1;
            }
            
            if (world[yCoord][xCoord].npc[index].direction == 1 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != INF
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != WANDERER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != SENTRY
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != HIKER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != RIVAL
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != EXPLORER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != PLAYER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != PACER)
            {
                world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
                world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
                world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
                world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = PACER;
            }
            else 
            {
                world[yCoord][xCoord].npc[index].direction = 0;
            }
        }

        if (world[yCoord][xCoord].npc[index].direction == 2 || world[yCoord][xCoord].npc[index].direction == 3) {
            if (world[yCoord][xCoord].npc[index].direction == 2 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != INF
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != WANDERER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != SENTRY
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != HIKER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != RIVAL
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != EXPLORER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != PLAYER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != PACER)
            {
                world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
                world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
                world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
                world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = PACER;
            }
            else 
            {
                world[yCoord][xCoord].npc[index].direction = 3;
            }
                
            if (world[yCoord][xCoord].npc[index].direction == 3 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != INF
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != WANDERER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != SENTRY
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != HIKER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != RIVAL
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != EXPLORER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != PLAYER
            && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != PACER)
            {
                world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
                world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
                world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
                world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
                world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = PACER;
            }
            else 
            {
                world[yCoord][xCoord].npc[index].direction = 2;
            }
        }
}

void wandererTravel(int xCoord, int yCoord, int index) {
        if (world[yCoord][xCoord].npc[index].direction == 0 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 1 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 2 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 3 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 4 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 5 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 6 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 7 && 
        world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] == world[yCoord][xCoord].npc[index].homeTerrain)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = WANDERER;
        }
        else
        {
            world[yCoord][xCoord].npc[index].direction = rand() % 8;
        }
}

void explorerTravel(int xCoord, int yCoord, int index) {
        if (world[yCoord][xCoord].npc[index].direction == 0 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 1 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x + 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 2 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 3 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x - 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 4 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x + 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 5 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x + 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x + 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 6 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y + 1][world[yCoord][xCoord].npc[index].coords.x - 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y + 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else if (world[yCoord][xCoord].npc[index].direction == 7 && world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != INF
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != WANDERER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != SENTRY
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != HIKER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != RIVAL
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != EXPLORER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != PLAYER
        && world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1] != PACER)
        {
            world[yCoord][xCoord].npc[index].nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y - 1][world[yCoord][xCoord].npc[index].coords.x - 1];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = world[yCoord][xCoord].npc[index].currSpace;
            world[yCoord][xCoord].npc[index].coords.y = world[yCoord][xCoord].npc[index].coords.y - 1;
            world[yCoord][xCoord].npc[index].coords.x = world[yCoord][xCoord].npc[index].coords.x - 1;
            world[yCoord][xCoord].npc[index].currSpace = world[yCoord][xCoord].npc[index].nextSpace;
            world[yCoord][xCoord].npc[index].currCost = world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].npc[index].coords.y][world[yCoord][xCoord].npc[index].coords.x] = EXPLORER;
        }
        else 
        {
            world[yCoord][xCoord].npc[index].direction = rand() % 8;
        }
}

void hikerTravel(int xCoord, int yCoord, int numTrainers) {
    if (numTrainers > 1) {    
        int y = world[yCoord][xCoord].hiker.coords.y;
        int x = world[yCoord][xCoord].hiker.coords.x;
        int shortestPath = world[yCoord][xCoord].hiker.pathCost[y][x];
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if ((i >= world[yCoord][xCoord].hiker.coords.y - 1 && i <= world[yCoord][xCoord].hiker.coords.y + 1)
                &&  (j >= world[yCoord][xCoord].hiker.coords.x - 1 && j <= world[yCoord][xCoord].hiker.coords.x + 1)) {
                    if (shortestPath > world[yCoord][xCoord].hiker.pathCost[i][j] 
                    && world[yCoord][xCoord].map[y][x] != PLAYER && world[yCoord][xCoord].map[y][x] != RIVAL 
                    && world[yCoord][xCoord].map[y][x] != SENTRY && world[yCoord][xCoord].map[y][x] != WANDERER 
                    && world[yCoord][xCoord].map[y][x] != EXPLORER && world[yCoord][xCoord].map[y][x] != PACER) {
                        shortestPath = world[yCoord][xCoord].hiker.pathCost[i][j];
                        y = i;
                        x = j;
                    }
                }
            }
        }

        if (world[yCoord][xCoord].map[y][x] != PLAYER && world[yCoord][xCoord].map[y][x] != RIVAL && world[yCoord][xCoord].map[y][x] != WANDERER 
            && world[yCoord][xCoord].map[y][x] != EXPLORER && world[yCoord][xCoord].map[y][x] != PACER) {
            world[yCoord][xCoord].hiker.nextSpace = world[yCoord][xCoord].map[y][x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = world[yCoord][xCoord].hiker.currSpace;
            world[yCoord][xCoord].hiker.coords.y = y;
            world[yCoord][xCoord].hiker.coords.x = x;
            if (world[yCoord][xCoord].hiker.nextSpace != 'h') {
                world[yCoord][xCoord].hiker.currSpace = world[yCoord][xCoord].hiker.nextSpace;
            }
            world[yCoord][xCoord].hiker.currCost = world[yCoord][xCoord].hiker.cost[y][x];
            world[yCoord][xCoord].map[y][x] = HIKER;
        }
    }
}

void rivalTravel(int xCoord, int yCoord, int numTrainers) {
    if (numTrainers > 0) {    
        int y = world[yCoord][xCoord].rival.coords.y;
        int x = world[yCoord][xCoord].rival.coords.x;
        int shortestPath = world[yCoord][xCoord].rival.pathCost[y][x];
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if ((i >= world[yCoord][xCoord].rival.coords.y - 1 && i <= world[yCoord][xCoord].rival.coords.y + 1)
                &&  (j >= world[yCoord][xCoord].rival.coords.x - 1 && j <= world[yCoord][xCoord].rival.coords.x + 1)) {
                    if (shortestPath > world[yCoord][xCoord].rival.pathCost[i][j] 
                    && world[yCoord][xCoord].map[y][x] != PLAYER && world[yCoord][xCoord].map[y][x] != HIKER 
                    && world[yCoord][xCoord].map[y][x] != SENTRY && world[yCoord][xCoord].map[y][x] != WANDERER 
                    && world[yCoord][xCoord].map[y][x] != EXPLORER && world[yCoord][xCoord].map[y][x] != PACER) {
                        shortestPath = world[yCoord][xCoord].rival.pathCost[i][j];
                        y = i;
                        x = j;
                    }
                }
            }
        }

        if (world[yCoord][xCoord].map[y][x] != PLAYER && world[yCoord][xCoord].map[y][x] != HIKER && world[yCoord][xCoord].map[y][x] != WANDERER 
            && world[yCoord][xCoord].map[y][x] != EXPLORER && world[yCoord][xCoord].map[y][x] != PACER) {
            world[yCoord][xCoord].rival.nextSpace = world[yCoord][xCoord].map[y][x];
            world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = world[yCoord][xCoord].rival.currSpace;
            world[yCoord][xCoord].rival.coords.y = y;
            world[yCoord][xCoord].rival.coords.x = x;
            if (world[yCoord][xCoord].rival.nextSpace != 'r') {
                world[yCoord][xCoord].rival.currSpace = world[yCoord][xCoord].rival.nextSpace;
            }
            world[yCoord][xCoord].rival.currCost = world[yCoord][xCoord].rival.cost[y][x];
            world[yCoord][xCoord].map[y][x] = RIVAL;
        }
    }
}

void hiker_defeatedTravel(int xCoord, int yCoord) {
    if (world[yCoord][xCoord].hiker.direction == 0 && world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x] != PACER)
    {
        world[yCoord][xCoord].hiker.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y + 1][world[yCoord][xCoord].hiker.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = world[yCoord][xCoord].hiker.currSpace;
        world[yCoord][xCoord].hiker.coords.y = world[yCoord][xCoord].hiker.coords.y + 1;
        world[yCoord][xCoord].hiker.currSpace = world[yCoord][xCoord].hiker.nextSpace;
        world[yCoord][xCoord].hiker.currCost = world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = HIKER;
    }
    else if (world[yCoord][xCoord].hiker.direction == 1 && world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1] != PACER)
    {
        world[yCoord][xCoord].hiker.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x + 1];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = world[yCoord][xCoord].hiker.currSpace;
        world[yCoord][xCoord].hiker.coords.x = world[yCoord][xCoord].hiker.coords.x + 1;
        world[yCoord][xCoord].hiker.currSpace = world[yCoord][xCoord].hiker.nextSpace;
        world[yCoord][xCoord].hiker.currCost = world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = HIKER;
    }
    else if (world[yCoord][xCoord].hiker.direction == 2 && world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x] != PACER)
    {
        world[yCoord][xCoord].hiker.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y - 1][world[yCoord][xCoord].hiker.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = world[yCoord][xCoord].hiker.currSpace;
        world[yCoord][xCoord].hiker.coords.y = world[yCoord][xCoord].hiker.coords.y - 1;
        world[yCoord][xCoord].hiker.currSpace = world[yCoord][xCoord].hiker.nextSpace;
        world[yCoord][xCoord].hiker.currCost = world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = HIKER;
    }
    else if (world[yCoord][xCoord].hiker.direction == 3 && world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1] != PACER)
    {
        world[yCoord][xCoord].hiker.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x - 1];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = world[yCoord][xCoord].hiker.currSpace;
        world[yCoord][xCoord].hiker.coords.x = world[yCoord][xCoord].hiker.coords.x - 1;
        world[yCoord][xCoord].hiker.currSpace = world[yCoord][xCoord].hiker.nextSpace;
        world[yCoord][xCoord].hiker.currCost = world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] = HIKER;
    }
    else 
    {
        world[yCoord][xCoord].hiker.direction = rand() % 4;
    }
}

void rival_defeatedTravel(int xCoord, int yCoord) {  
    if (world[yCoord][xCoord].rival.direction == 0 && world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x] != PACER)
    {
        world[yCoord][xCoord].rival.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y + 1][world[yCoord][xCoord].rival.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = world[yCoord][xCoord].rival.currSpace;
        world[yCoord][xCoord].rival.coords.y = world[yCoord][xCoord].rival.coords.y + 1;
        world[yCoord][xCoord].rival.currSpace = world[yCoord][xCoord].rival.nextSpace;
        world[yCoord][xCoord].rival.currCost = world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = RIVAL;
    }
    else if (world[yCoord][xCoord].rival.direction == 1 && world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1] != PACER)
    {
        world[yCoord][xCoord].rival.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x + 1];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = world[yCoord][xCoord].rival.currSpace;
        world[yCoord][xCoord].rival.coords.x = world[yCoord][xCoord].rival.coords.x + 1;
        world[yCoord][xCoord].rival.currSpace = world[yCoord][xCoord].rival.nextSpace;
        world[yCoord][xCoord].rival.currCost = world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = RIVAL;
    }
    else if (world[yCoord][xCoord].rival.direction == 2 && world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x] != PACER)
    {
        world[yCoord][xCoord].rival.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y - 1][world[yCoord][xCoord].rival.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = world[yCoord][xCoord].rival.currSpace;
        world[yCoord][xCoord].rival.coords.y = world[yCoord][xCoord].rival.coords.y - 1;
        world[yCoord][xCoord].rival.currSpace = world[yCoord][xCoord].rival.nextSpace;
        world[yCoord][xCoord].rival.currCost = world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = RIVAL;
    }
    else if (world[yCoord][xCoord].rival.direction == 3 && world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != INF
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != WANDERER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != SENTRY
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != HIKER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != RIVAL
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != EXPLORER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != PLAYER
    && world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1] != PACER)
    {
        world[yCoord][xCoord].rival.nextSpace = world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x - 1];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = world[yCoord][xCoord].rival.currSpace;
        world[yCoord][xCoord].rival.coords.x = world[yCoord][xCoord].rival.coords.x - 1;
        world[yCoord][xCoord].rival.currSpace = world[yCoord][xCoord].rival.nextSpace;
        world[yCoord][xCoord].rival.currCost = world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x];
        world[yCoord][xCoord].map[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] = RIVAL;
    }
    else 
    {
        world[yCoord][xCoord].rival.direction = rand() % 4;
    }
}

double calculateDamage(int id, int lvl, int attack, int defense, int speed, string move, vector<string> attackingType) {
    vector<int> attackingId;
    double random = ((double)(rand() % 16) + 85.0) / 100.0;
    int criticalChance = rand() % 256;
    double critical = 1;
    double numerator;
    double denominator = 50;
    double remainder;
    double stab = 1;
    double type = 1;
    double damage;
    int accuracy;
    int power;
    int typeId;
    int i;

    for (i = 0; i < pokemonData.move.size(); i++) {
        if (move == pokemonData.move[i].identifier) {
            typeId = pokemonData.move[i].type_id;
            accuracy = pokemonData.move[i].accuracy;
            power = pokemonData.move[i].power;
        }
    }

    if (accuracy < (rand() % 100)) {
        damage = 0;
        return damage;
    }

    if (power == 0) {
        damage = 1;
        return damage;
    }

    for (i = 0; i < pokemonData.poke_type.size(); i++) {
        if (id == pokemonData.poke_type[i].pokemon_id) {
            attackingId.push_back(pokemonData.poke_type[i].type_id);
        }
    }

    for (i = 0; i < attackingId.size(); i++) {
        if (typeId == attackingId[i]) {
            stab = 1.5;
        }
    }

    if (criticalChance < (speed / 2)) {
        critical = 1.5;
    }

    numerator = (((2 * lvl) / 5) + 2) * power * (attack / defense);
    remainder = critical * random * stab * type;
    damage = ((numerator / denominator) + 2) * remainder;

    if (damage <= 0) {
        damage = 1;
        return damage;
    }
    return damage;
}

void replenishItems() {
    if (pc_team.items.revives < 5) {
        pc_team.items.revives = 5;
    }
    if (pc_team.items.potions < 5) {
        pc_team.items.potions = 5;
    }
    if (pc_team.items.pokeballs < 5) {
        pc_team.items.pokeballs = 5;
    }
}

void replenishTeam() {
    for (int i = 0; i < pc_team.poke.size(); i++) {
        pc_team.poke[i].currentHP = pc_team.poke[i].stats[0];
    }
}

vector<string> getTypings(int pokemonId) {
    vector<int> typeId;
    vector<string> typeName;
    int i, j;
    for (i = 0; i < pokemonData.poke_type.size(); i++) {
        if (pokemonId == pokemonData.poke_type[i].pokemon_id) {
            typeId.push_back(pokemonData.poke_type[i].type_id);
        }
    }
    for (i = 0; i < typeId.size(); i++) {
        for (j = 0; j < pokemonData.type_name.size(); j++) {
            if ((typeId[i] == pokemonData.type_name[j].type_id) && (pokemonData.type_name[j].local_language_id == 9)) {
                typeName.push_back(pokemonData.type_name[j].name);
            }
        }
    }
    return typeName;
}

bool isShiny() {
    int shiny = rand() % 8192;
    bool isShiny;
    if (shiny == 0) {
        isShiny = true;
    }
    else {
        isShiny = false;
    }
    return isShiny;
}

vector<string> getMoves(int speciesId, int lvl) {
    vector<string> learnable;
    vector<string> knows;
    int i;
    int numMoves;
    for (i = 0; i < pokemonData.poke_move.size(); i++) {
        if ((pokemonData.poke_move[i].pokemon_id == speciesId) && (pokemonData.poke_move[i].pokemon_move_method_id == 1)) {
            if (pokemonData.poke_move[i].level <= lvl) {
                learnable.push_back(pokemonData.move[pokemonData.poke_move[i].move_id - 1].identifier.c_str());
            }
        }
    }
    sort(learnable.begin(), learnable.end());
    auto iterator = unique(learnable.begin(), learnable.end());
    learnable.erase(iterator, learnable.end());

    if (learnable.size() < 2) {
        learnable.push_back(pokemonData.move[164].identifier.c_str());
    }
    numMoves = learnable.size();

    int randIndex1 = rand() % numMoves;
    int randIndex2 = rand() % numMoves;

    while (randIndex2 == randIndex1) {
        randIndex2 = rand() % numMoves;
    }

    knows.push_back(learnable[randIndex1]);
    knows.push_back(learnable[randIndex2]);

    return knows;
}

vector<int> lvlUpPokemon(const vector<int> iv, const vector<int> baseStats, const int pokemonlvl) {
    vector<int> pokemonStats;
    int numerator;
    int denominator = 100;
    int hp, otherstat;
    int i;

    for (i = 0; i < 6; i++) {
        if (i == 0) {
            numerator = ((baseStats[i] + iv[i]) * 2) * pokemonlvl;
            hp = (numerator / denominator) + pokemonlvl + 10;
            pokemonStats.push_back(hp);
        }
        else {
            numerator = ((baseStats[i] + iv[i]) * 2) * pokemonlvl;
            otherstat = (numerator / denominator) + 5;
            pokemonStats.push_back(otherstat);
        }
    }

    return pokemonStats;
}

vector<int> getStats(int pokemonId, int lvl, vector<int> iv) {
    vector<int> baseStats;
    vector<int> pokemonStats;
    int i, j;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < pokemonData.poke_stat.size(); j++) {
            if (pokemonData.poke_stat[j].pokemon_id == pokemonId && pokemonData.poke_stat[j].stat_id == (i + 1)) {
                baseStats.push_back(pokemonData.poke_stat[j].base_stat);
            }
        }
    }
    pokemonStats = lvlUpPokemon(iv, baseStats, lvl);

    return pokemonStats;
}

vector<int> getIV() {
    vector<int> iv;
    for (int i = 0; i < 6; i++) {
        iv.push_back((rand() % 16));
    }
    return iv;
}

int loadPokemonlvl(int xCoord, int yCoord) {
    int manDis = static_cast<int>(manhattanDistance(xCoord, yCoord));
    int wildPokemonlvl;

    if (manDis <= 200) {
        int minlvl = 1;
        int maxlvl = manDis / 2;

        if (maxlvl <= minlvl) {
            wildPokemonlvl = minlvl;
        }
        else {
            wildPokemonlvl = (rand() % maxlvl - minlvl + 1) + minlvl;
        }
    }
    else {
        int minlvl = (manDis - 200) / 2;
        int maxlvl = 100;
        wildPokemonlvl = (rand() % maxlvl - minlvl + 1) + minlvl;
    }

    return wildPokemonlvl;
}

void pokemonOption(WINDOW *battle) {
    mvwprintw(battle, 17, 1, "                                                                            ");
    mvwprintw(battle, 18, 1, "                                                                            ");
    mvwprintw(battle, 19, 1, "                                                                            ");
    
    int j = 0, k = 0;
    for (int i = 0; i < pc_team.poke.size(); i++) {
        if (j > 2) {
            mvwprintw(battle, 17 + k, 30, "%d. %s", i + 1, pc_team.poke[i].identifier.c_str());
            k++;
        }
        else {
            mvwprintw(battle, 17 + j, 2, "%d. %s", i + 1, pc_team.poke[i].identifier.c_str());
            j++;
        }
    }
}

void trainerBattle(int xCoord, int yCoord, int index, WINDOW *battle, WINDOW *win) {
    char battleInput;
    int k, m, i;
    
    // Initiate Screen
    box(battle, 0, 0);
    for (i = 1; i < COLS - 1; i++) {
        mvwprintw(battle, 16, i, "x");
    }
    for (i = 1; i < ROWS - 4; i++) {
        mvwprintw(battle, i, 40, "x");
    }
    mvwprintw(battle, 1, 2, "You:");

    // Battle Starts here
    if (index == -2) {
        bool battleEnd = false;
        char battleInput;
        int pcPriority = 0;
        int wildPriority = 0;
        int j = 0;

        mvwprintw(battle, 1, 42, "Rival Battle:");
        wrefresh(battle);

        while (battleEnd == false) {
            bool playerWent = false;

            mvwprintw(battle, 3, 2, "                                      ");
            mvwprintw(battle, 3, 2, "Current Pokemon: %s", pc_team.poke[0].identifier.c_str());
            mvwprintw(battle, 3, 42, "                                    ");
            mvwprintw(battle, 3, 42, "Opponent Pokemon: %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str());

            mvwprintw(battle, 4, 2, "                                      ");
            mvwprintw(battle, 4, 2, "Lvl: %d", pc_team.poke[0].lvl);
            mvwprintw(battle, 4, 42, "Lvl: %d", world[yCoord][xCoord].rival.poke[0].lvl);

            mvwprintw(battle, 5, 2, "                                      ");
            mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
            mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);

            mvwprintw(battle, 6, 2, "                                      ");
            mvwprintw(battle, 6, 2, "Speed: %d", pc_team.poke[0].stats[5]);
            mvwprintw(battle, 6, 42, "Speed: %d", world[yCoord][xCoord].rival.poke[0].stats[5]);

            mvwprintw(battle, 8, 42, "Number of Pokemon: %d", world[yCoord][xCoord].rival.poke.size());

            mvwprintw(battle, 17, 1, "                                                                            ");
            mvwprintw(battle, 18, 1, "                                                                            ");
            mvwprintw(battle, 19, 1, "                                                                            ");
            mvwprintw(battle, 18, 15, "h. Fight      j. Bag      k. Pokemon      l. Run");
            wrefresh(battle);
            battleInput = wgetch(battle);

            if (battleInput == 'h') {
                // Attack
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");
                mvwprintw(battle, 18, 2, "h. %s      j. %s      k. Go back", pc_team.poke[0].moves[0].c_str(), pc_team.poke[0].moves[1].c_str());
                wrefresh(battle);

                do {
                    battleInput = wgetch(battle);
                    if (battleInput == 'h') {
                        // Attack with move 1
                        int move = rand() % 2;
                        int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], world[yCoord][xCoord].rival.poke[0].stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[0], pc_team.poke[0].typing);
                        int oppDamage = calculateDamage(world[yCoord][xCoord].rival.poke[0].id, world[yCoord][xCoord].rival.poke[0].lvl, world[yCoord][xCoord].rival.poke[0].stats[1], pc_team.poke[0].stats[2], world[yCoord][xCoord].rival.poke[0].stats[5], world[yCoord][xCoord].rival.poke[0].moves[move], world[yCoord][xCoord].rival.poke[0].typing);

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (pc_team.poke[0].moves[0] == pokemonData.move[i].identifier) {
                                pcPriority = pokemonData.move[i].priority;
                            }
                        }

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (world[yCoord][xCoord].rival.poke[0].moves[move] == pokemonData.move[i].identifier) {
                                wildPriority = pokemonData.move[i].priority;
                            }
                        }

                        if (pcPriority > wildPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                            if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                world[yCoord][xCoord].rival.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (world[yCoord][xCoord].rival.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (wildPriority > pcPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                    world[yCoord][xCoord].rival.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pcPriority == wildPriority) {
                            if (pc_team.poke[0].stats[5] >= world[yCoord][xCoord].rival.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                    world[yCoord][xCoord].rival.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (world[yCoord][xCoord].rival.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    pc_team.poke[0].currentHP -= oppDamage;
                                    if (pc_team.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                        pc_team.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                            else if (pc_team.poke[0].stats[5] < world[yCoord][xCoord].rival.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (pc_team.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                                    if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                        world[yCoord][xCoord].rival.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                        }

                        if (world[yCoord][xCoord].rival.poke[0].ko == true) {
                            if (world[yCoord][xCoord].rival.poke.size() == 1) {
                                battleEnd = true;
                            }
                            else if (world[yCoord][xCoord].rival.poke.size() > 1) {
                                swap(world[yCoord][xCoord].rival.poke[0], world[yCoord][xCoord].rival.poke[world[yCoord][xCoord].rival.poke.size() - 1]);
                                world[yCoord][xCoord].rival.poke.pop_back();
                            }
                        }

                        if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);

                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }
                        else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                            int one = 0, two = 0, three = 0, four = 0, five = 0;
                            int count = 0;
                            pokemonOption(battle);
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "Who will I switch in?");
                            wrefresh(battle);
                            wrefresh(win);

                            for (int i = 0; i < pc_team.poke.size(); i++) {
                                if (pc_team.poke[i].ko == false) {
                                    count++;
                                }
                            }

                            if (count == 0) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(2000000);

                                wclear(battle);
                                wrefresh(battle);
                                delwin(battle);
                                wclear(win);
                                wrefresh(win);
                                delwin(win);
                                endwin();
                                refresh();

                                exit(0);
                            }

                            do {
                                battleInput = wgetch(battle);
                                if (battleInput == '2') {
                                    if (pc_team.poke.size() >= 2) {
                                        if (pc_team.poke[1].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[1]);
                                            break;
                                        }
                                        else {
                                            one = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '3') {
                                    if (pc_team.poke.size() >= 3) {
                                        if (pc_team.poke[2].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[2]);
                                            break;
                                        }
                                        else {
                                            two = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '4') {
                                    if (pc_team.poke.size() >= 4) {
                                        if (pc_team.poke[3].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[3]);
                                            break;
                                        }
                                        else {
                                            three = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '5') {
                                    if (pc_team.poke.size() >= 5) {
                                        if (pc_team.poke[4].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[4]);
                                            break;
                                        }
                                        else {
                                            four = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '6') {
                                    if (pc_team.poke.size() >= 6) {
                                        if (pc_team.poke[5].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[5]);
                                            break;
                                        }
                                        else {
                                            five = 1;
                                        }
                                    }
                                }
                            } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                        }

                        playerWent = true;
                        break;
                    }
                    else if (battleInput == 'j') {
                        // Attack with move 2
                        int move = rand() % 2;
                        int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], world[yCoord][xCoord].rival.poke[0].stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[1], pc_team.poke[0].typing);
                        int oppDamage = calculateDamage(world[yCoord][xCoord].rival.poke[0].id, world[yCoord][xCoord].rival.poke[0].lvl, world[yCoord][xCoord].rival.poke[0].stats[1], pc_team.poke[0].stats[2], world[yCoord][xCoord].rival.poke[0].stats[5], world[yCoord][xCoord].rival.poke[0].moves[move], world[yCoord][xCoord].rival.poke[0].typing);

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (pc_team.poke[0].moves[1] == pokemonData.move[i].identifier) {
                                pcPriority = pokemonData.move[i].priority;
                            }
                        }

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (world[yCoord][xCoord].rival.poke[0].moves[move] == pokemonData.move[i].identifier) {
                                wildPriority = pokemonData.move[i].priority;
                            }
                        }

                        if (pcPriority > wildPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                            if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                world[yCoord][xCoord].rival.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (world[yCoord][xCoord].rival.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (wildPriority > pcPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                    world[yCoord][xCoord].rival.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pcPriority == wildPriority) {
                            if (pc_team.poke[0].stats[5] >= world[yCoord][xCoord].rival.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                    world[yCoord][xCoord].rival.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (world[yCoord][xCoord].rival.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    pc_team.poke[0].currentHP -= oppDamage;
                                    if (pc_team.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                        pc_team.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                            else if (pc_team.poke[0].stats[5] < world[yCoord][xCoord].rival.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].rival.poke[0].identifier.c_str(), world[yCoord][xCoord].rival.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (pc_team.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    world[yCoord][xCoord].rival.poke[0].currentHP -= yourDamage;
                                    if (world[yCoord][xCoord].rival.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                        world[yCoord][xCoord].rival.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].rival.poke[0].currentHP, world[yCoord][xCoord].rival.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                        }

                        if (world[yCoord][xCoord].rival.poke[0].ko == true) {
                            if (world[yCoord][xCoord].rival.poke.size() == 1) {
                                battleEnd = true;
                            }
                            else if (world[yCoord][xCoord].rival.poke.size() > 1) {
                                swap(world[yCoord][xCoord].rival.poke[0], world[yCoord][xCoord].rival.poke[world[yCoord][xCoord].rival.poke.size() - 1]);
                                world[yCoord][xCoord].rival.poke.pop_back();
                            }
                        }

                        if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);
                            
                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }
                        else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                            int one = 0, two = 0, three = 0, four = 0, five = 0;
                            int count = 0;
                            pokemonOption(battle);
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "Who will I switch in?");
                            wrefresh(battle);
                            wrefresh(win);

                            for (int i = 0; i < pc_team.poke.size(); i++) {
                                if (pc_team.poke[i].ko == false) {
                                    count++;
                                }
                            }

                            if (count == 0) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(2000000);

                                wclear(battle);
                                wrefresh(battle);
                                delwin(battle);
                                wclear(win);
                                wrefresh(win);
                                delwin(win);
                                endwin();
                                refresh();

                                exit(0);
                            }

                            do {
                                battleInput = wgetch(battle);
                                if (battleInput == '2') {
                                    if (pc_team.poke.size() >= 2) {
                                        if (pc_team.poke[1].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[1]);
                                            break;
                                        }
                                        else {
                                            one = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '3') {
                                    if (pc_team.poke.size() >= 3) {
                                        if (pc_team.poke[2].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[2]);
                                            break;
                                        }
                                        else {
                                            two = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '4') {
                                    if (pc_team.poke.size() >= 4) {
                                        if (pc_team.poke[3].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[3]);
                                            break;
                                        }
                                        else {
                                            three = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '5') {
                                    if (pc_team.poke.size() >= 5) {
                                        if (pc_team.poke[4].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[4]);
                                            break;
                                        }
                                        else {
                                            four = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '6') {
                                    if (pc_team.poke.size() >= 6) {
                                        if (pc_team.poke[5].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[5]);
                                            break;
                                        }
                                        else {
                                            five = 1;
                                        }
                                    }
                                }
                            } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                        }

                        playerWent = true;
                        break;
                    }
                    else if (battleInput == 'k') {
                        mvwprintw(battle, 17, 1, "                                                                            ");
                        mvwprintw(battle, 18, 1, "                                                                            ");
                        mvwprintw(battle, 19, 1, "                                                                            ");
                        mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                        wrefresh(battle);
                        break;
                    }
                } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k');
            }
            else if (battleInput == 'j') {
                // Bag
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");
                mvwprintw(battle, 18, 2, "h. Pokeball (%d)     j. Potion (%d)     k. Revive (%d)      l. Go back", pc_team.items.pokeballs, pc_team.items.potions, pc_team.items.revives);
                wrefresh(battle);

                do {
                    battleInput = wgetch(battle);
                    if (battleInput == 'h') {
                        // Use Pokeball
                        break;
                    }
                    else if (battleInput == 'j') {
                        // Use Potion
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Use Potion on who?");
                        wrefresh(battle);
                        wrefresh(win);
                        
                        do {
                            battleInput = wgetch(battle);
                            int deficit;
                            if (battleInput == '1') {
                                if (pc_team.poke[0].ko == false && pc_team.poke.size() >= 1 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[0].stats[0] - pc_team.poke[0].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[0].currentHP = pc_team.poke[0].stats[0];
                                        pc_team.items.potions--;
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                        wrefresh(battle);
                                    }
                                    else {
                                        pc_team.poke[0].currentHP += 20;
                                        pc_team.items.potions--;
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                        wrefresh(battle);
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '2') {
                                if (pc_team.poke[1].ko == false && pc_team.poke.size() >= 2 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[1].stats[0] - pc_team.poke[1].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[1].currentHP = pc_team.poke[1].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[1].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke[2].ko == false && pc_team.poke.size() >= 3 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[2].stats[0] - pc_team.poke[2].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[2].currentHP = pc_team.poke[2].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[2].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke[3].ko == false && pc_team.poke.size() >= 4 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[3].stats[0] - pc_team.poke[3].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[3].currentHP = pc_team.poke[3].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[3].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke[4].ko == false && pc_team.poke.size() >= 5 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[4].stats[0] - pc_team.poke[4].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[4].currentHP = pc_team.poke[4].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[4].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke[5].ko == false && pc_team.poke.size() >= 6 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[5].stats[0] - pc_team.poke[5].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[5].currentHP = pc_team.poke[5].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[5].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                        } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                    }
                    else if (battleInput == 'k') {
                        // Use Revive
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Use Revive on who?");
                        wrefresh(battle);
                        wrefresh(win);
                        
                        do {
                            battleInput = wgetch(battle);
                            if (battleInput == '1') {
                                if (pc_team.poke[0].ko == true && pc_team.poke.size() >= 1 && pc_team.items.revives > 0) {
                                    pc_team.poke[0].currentHP = pc_team.poke[0].stats[0] / 2;
                                    pc_team.poke[0].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '2') {
                                if (pc_team.poke[1].ko == true && pc_team.poke.size() >= 2 && pc_team.items.revives > 0) {
                                    pc_team.poke[1].currentHP = pc_team.poke[1].stats[0] / 2;
                                    pc_team.poke[1].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke[2].ko == true && pc_team.poke.size() >= 3 && pc_team.items.revives > 0) {
                                    pc_team.poke[2].currentHP = pc_team.poke[2].stats[0] / 2;
                                    pc_team.poke[2].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke[3].ko == true && pc_team.poke.size() >= 4 && pc_team.items.revives > 0) {
                                    pc_team.poke[3].currentHP = pc_team.poke[3].stats[0] / 2;
                                    pc_team.poke[3].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke[4].ko == true && pc_team.poke.size() >= 5 && pc_team.items.revives > 0) {
                                    pc_team.poke[4].currentHP = pc_team.poke[4].stats[0] / 2;
                                    pc_team.poke[4].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke[5].ko == true && pc_team.poke.size() >= 6 && pc_team.items.revives > 0) {
                                    pc_team.poke[5].currentHP = pc_team.poke[5].stats[0] / 2;
                                    pc_team.poke[5].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            //break;
                        } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                    }
                    else if (battleInput == 'l') {
                        mvwprintw(battle, 17, 1, "                                                                            ");
                        mvwprintw(battle, 18, 1, "                                                                            ");
                        mvwprintw(battle, 19, 1, "                                                                            ");
                        mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                        wrefresh(battle);
                        break;
                    }
                    break;
                } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k' || battleInput != 'l');
            }
            else if (battleInput == 'k') {
                // Pokemon
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");

                pokemonOption(battle);
                wrefresh(battle);
                
                do {
                    battleInput = wgetch(battle);
                    if (battleInput == '1') {
                        break;
                    }
                    else if (battleInput == '2') {
                        if (pc_team.poke.size() >= 2) {
                            if (pc_team.poke[1].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[1]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '3') {
                        if (pc_team.poke.size() >= 3) {
                            if (pc_team.poke[2].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[2]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '4') {
                        if (pc_team.poke.size() >= 4) {
                            if (pc_team.poke[3].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[3]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '5') {
                        if (pc_team.poke.size() >= 5) {
                            if (pc_team.poke[4].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[4]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '6') {
                        if (pc_team.poke.size() >= 6) {
                            if (pc_team.poke[5].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[5]);
                                break;
                            }
                        }
                    }
                } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
            }
            else if (battleInput == 'l') {
                // Run
                //battleEnd = true;
            }
        }

        wrefresh(battle);
        refresh();

    }
    else if (index == -1) {
        bool battleEnd = false;
        char battleInput;
        int pcPriority = 0;
        int wildPriority = 0;
        int j = 0;

        mvwprintw(battle, 1, 42, "Hiker Battle:");
        wrefresh(battle);

        while (battleEnd == false) {
            bool playerWent = false;

            mvwprintw(battle, 3, 2, "                                      ");
            mvwprintw(battle, 3, 2, "Current Pokemon: %s", pc_team.poke[0].identifier.c_str());
            mvwprintw(battle, 3, 42, "                                    ");
            mvwprintw(battle, 3, 42, "Opponent Pokemon: %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str());

            mvwprintw(battle, 4, 2, "                                      ");
            mvwprintw(battle, 4, 2, "Lvl: %d", pc_team.poke[0].lvl);
            mvwprintw(battle, 4, 42, "Lvl: %d", world[yCoord][xCoord].hiker.poke[0].lvl);

            mvwprintw(battle, 5, 2, "                                      ");
            mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
            mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);

            mvwprintw(battle, 6, 2, "                                      ");
            mvwprintw(battle, 6, 2, "Speed: %d", pc_team.poke[0].stats[5]);
            mvwprintw(battle, 6, 42, "Speed: %d", world[yCoord][xCoord].hiker.poke[0].stats[5]);

            mvwprintw(battle, 8, 42, "Number of Pokemon: %d", world[yCoord][xCoord].hiker.poke.size());

            mvwprintw(battle, 17, 1, "                                                                            ");
            mvwprintw(battle, 18, 1, "                                                                            ");
            mvwprintw(battle, 19, 1, "                                                                            ");
            mvwprintw(battle, 18, 15, "h. Fight      j. Bag      k. Pokemon      l. Run");
            wrefresh(battle);
            battleInput = wgetch(battle);

            if (battleInput == 'h') {
                // Attack
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");
                mvwprintw(battle, 18, 2, "h. %s      j. %s      k. Go back", pc_team.poke[0].moves[0].c_str(), pc_team.poke[0].moves[1].c_str());
                wrefresh(battle);

                do {
                    battleInput = wgetch(battle);
                    if (battleInput == 'h') {
                        // Attack with move 1
                        int move = rand() % 2;
                        int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], world[yCoord][xCoord].hiker.poke[0].stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[0], pc_team.poke[0].typing);
                        int oppDamage = calculateDamage(world[yCoord][xCoord].hiker.poke[0].id, world[yCoord][xCoord].hiker.poke[0].lvl, world[yCoord][xCoord].hiker.poke[0].stats[1], pc_team.poke[0].stats[2], world[yCoord][xCoord].hiker.poke[0].stats[5], world[yCoord][xCoord].hiker.poke[0].moves[move], world[yCoord][xCoord].hiker.poke[0].typing);

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (pc_team.poke[0].moves[0] == pokemonData.move[i].identifier) {
                                pcPriority = pokemonData.move[i].priority;
                            }
                        }

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (world[yCoord][xCoord].hiker.poke[0].moves[move] == pokemonData.move[i].identifier) {
                                wildPriority = pokemonData.move[i].priority;
                            }
                        }

                        if (pcPriority > wildPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                            if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                world[yCoord][xCoord].hiker.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (world[yCoord][xCoord].hiker.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (wildPriority > pcPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                    world[yCoord][xCoord].hiker.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pcPriority == wildPriority) {
                            if (pc_team.poke[0].stats[5] >= world[yCoord][xCoord].hiker.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                    world[yCoord][xCoord].hiker.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (world[yCoord][xCoord].hiker.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    pc_team.poke[0].currentHP -= oppDamage;
                                    if (pc_team.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                        pc_team.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                            else if (pc_team.poke[0].stats[5] < world[yCoord][xCoord].hiker.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (pc_team.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                                    if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                        world[yCoord][xCoord].hiker.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                        }

                        if (world[yCoord][xCoord].hiker.poke[0].ko == true) {
                            if (world[yCoord][xCoord].hiker.poke.size() == 1) {
                                battleEnd = true;
                            }
                            else if (world[yCoord][xCoord].hiker.poke.size() > 1) {
                                swap(world[yCoord][xCoord].hiker.poke[0], world[yCoord][xCoord].hiker.poke[world[yCoord][xCoord].hiker.poke.size() - 1]);
                                world[yCoord][xCoord].hiker.poke.pop_back();
                            }
                        }

                        if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);

                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }
                        else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                            int one = 0, two = 0, three = 0, four = 0, five = 0;
                            int count = 0;
                            pokemonOption(battle);
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "Who will I switch in?");
                            wrefresh(battle);
                            wrefresh(win);

                            for (int i = 0; i < pc_team.poke.size(); i++) {
                                if (pc_team.poke[i].ko == false) {
                                    count++;
                                }
                            }

                            if (count == 0) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(2000000);

                                wclear(battle);
                                wrefresh(battle);
                                delwin(battle);
                                wclear(win);
                                wrefresh(win);
                                delwin(win);
                                endwin();
                                refresh();

                                exit(0);
                            }

                            do {
                                battleInput = wgetch(battle);
                                if (battleInput == '2') {
                                    if (pc_team.poke.size() >= 2) {
                                        if (pc_team.poke[1].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[1]);
                                            break;
                                        }
                                        else {
                                            one = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '3') {
                                    if (pc_team.poke.size() >= 3) {
                                        if (pc_team.poke[2].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[2]);
                                            break;
                                        }
                                        else {
                                            two = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '4') {
                                    if (pc_team.poke.size() >= 4) {
                                        if (pc_team.poke[3].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[3]);
                                            break;
                                        }
                                        else {
                                            three = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '5') {
                                    if (pc_team.poke.size() >= 5) {
                                        if (pc_team.poke[4].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[4]);
                                            break;
                                        }
                                        else {
                                            four = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '6') {
                                    if (pc_team.poke.size() >= 6) {
                                        if (pc_team.poke[5].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[5]);
                                            break;
                                        }
                                        else {
                                            five = 1;
                                        }
                                    }
                                }
                            } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                        }

                        playerWent = true;
                        break;
                    }
                    else if (battleInput == 'j') {
                        // Attack with move 2
                        int move = rand() % 2;
                        int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], world[yCoord][xCoord].hiker.poke[0].stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[1], pc_team.poke[0].typing);
                        int oppDamage = calculateDamage(world[yCoord][xCoord].hiker.poke[0].id, world[yCoord][xCoord].hiker.poke[0].lvl, world[yCoord][xCoord].hiker.poke[0].stats[1], pc_team.poke[0].stats[2], world[yCoord][xCoord].hiker.poke[0].stats[5], world[yCoord][xCoord].hiker.poke[0].moves[move], world[yCoord][xCoord].hiker.poke[0].typing);

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (pc_team.poke[0].moves[1] == pokemonData.move[i].identifier) {
                                pcPriority = pokemonData.move[i].priority;
                            }
                        }

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (world[yCoord][xCoord].hiker.poke[0].moves[move] == pokemonData.move[i].identifier) {
                                wildPriority = pokemonData.move[i].priority;
                            }
                        }

                        if (pcPriority > wildPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                            if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                world[yCoord][xCoord].hiker.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (world[yCoord][xCoord].hiker.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (wildPriority > pcPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                    world[yCoord][xCoord].hiker.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pcPriority == wildPriority) {
                            if (pc_team.poke[0].stats[5] >= world[yCoord][xCoord].hiker.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                    world[yCoord][xCoord].hiker.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (world[yCoord][xCoord].hiker.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    pc_team.poke[0].currentHP -= oppDamage;
                                    if (pc_team.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                        pc_team.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                            else if (pc_team.poke[0].stats[5] < world[yCoord][xCoord].hiker.poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].hiker.poke[0].identifier.c_str(), world[yCoord][xCoord].hiker.poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (pc_team.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    world[yCoord][xCoord].hiker.poke[0].currentHP -= yourDamage;
                                    if (world[yCoord][xCoord].hiker.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                        world[yCoord][xCoord].hiker.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].hiker.poke[0].currentHP, world[yCoord][xCoord].hiker.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                        }

                        if (world[yCoord][xCoord].hiker.poke[0].ko == true) {
                            if (world[yCoord][xCoord].hiker.poke.size() == 1) {
                                battleEnd = true;
                            }
                            else if (world[yCoord][xCoord].hiker.poke.size() > 1) {
                                swap(world[yCoord][xCoord].hiker.poke[0], world[yCoord][xCoord].hiker.poke[world[yCoord][xCoord].hiker.poke.size() - 1]);
                                world[yCoord][xCoord].hiker.poke.pop_back();
                            }
                        }

                        if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);
                            
                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }
                        else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                            int one = 0, two = 0, three = 0, four = 0, five = 0;
                            int count = 0;
                            pokemonOption(battle);
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "Who will I switch in?");
                            wrefresh(battle);
                            wrefresh(win);

                            for (int i = 0; i < pc_team.poke.size(); i++) {
                                if (pc_team.poke[i].ko == false) {
                                    count++;
                                }
                            }

                            if (count == 0) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(2000000);

                                wclear(battle);
                                wrefresh(battle);
                                delwin(battle);
                                wclear(win);
                                wrefresh(win);
                                delwin(win);
                                endwin();
                                refresh();

                                exit(0);
                            }

                            do {
                                battleInput = wgetch(battle);
                                if (battleInput == '2') {
                                    if (pc_team.poke.size() >= 2) {
                                        if (pc_team.poke[1].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[1]);
                                            break;
                                        }
                                        else {
                                            one = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '3') {
                                    if (pc_team.poke.size() >= 3) {
                                        if (pc_team.poke[2].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[2]);
                                            break;
                                        }
                                        else {
                                            two = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '4') {
                                    if (pc_team.poke.size() >= 4) {
                                        if (pc_team.poke[3].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[3]);
                                            break;
                                        }
                                        else {
                                            three = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '5') {
                                    if (pc_team.poke.size() >= 5) {
                                        if (pc_team.poke[4].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[4]);
                                            break;
                                        }
                                        else {
                                            four = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '6') {
                                    if (pc_team.poke.size() >= 6) {
                                        if (pc_team.poke[5].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[5]);
                                            break;
                                        }
                                        else {
                                            five = 1;
                                        }
                                    }
                                }
                            } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                        }

                        playerWent = true;
                        break;
                    }
                    else if (battleInput == 'k') {
                        mvwprintw(battle, 17, 1, "                                                                            ");
                        mvwprintw(battle, 18, 1, "                                                                            ");
                        mvwprintw(battle, 19, 1, "                                                                            ");
                        mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                        wrefresh(battle);
                        break;
                    }
                } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k');
            }
            else if (battleInput == 'j') {
                // Bag
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");
                mvwprintw(battle, 18, 2, "h. Pokeball (%d)     j. Potion (%d)     k. Revive (%d)      l. Go back", pc_team.items.pokeballs, pc_team.items.potions, pc_team.items.revives);
                wrefresh(battle);

                do {
                    battleInput = wgetch(battle);
                    if (battleInput == 'h') {
                        // Use Pokeball
                        break;
                    }
                    else if (battleInput == 'j') {
                        // Use Potion
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Use Potion on who?");
                        wrefresh(battle);
                        wrefresh(win);
                        
                        do {
                            battleInput = wgetch(battle);
                            int deficit;
                            if (battleInput == '1') {
                                if (pc_team.poke[0].ko == false && pc_team.poke.size() >= 1 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[0].stats[0] - pc_team.poke[0].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[0].currentHP = pc_team.poke[0].stats[0];
                                        pc_team.items.potions--;
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                        wrefresh(battle);
                                    }
                                    else {
                                        pc_team.poke[0].currentHP += 20;
                                        pc_team.items.potions--;
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                        wrefresh(battle);
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '2') {
                                if (pc_team.poke[1].ko == false && pc_team.poke.size() >= 2 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[1].stats[0] - pc_team.poke[1].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[1].currentHP = pc_team.poke[1].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[1].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke[2].ko == false && pc_team.poke.size() >= 3 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[2].stats[0] - pc_team.poke[2].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[2].currentHP = pc_team.poke[2].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[2].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke[3].ko == false && pc_team.poke.size() >= 4 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[3].stats[0] - pc_team.poke[3].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[3].currentHP = pc_team.poke[3].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[3].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke[4].ko == false && pc_team.poke.size() >= 5 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[4].stats[0] - pc_team.poke[4].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[4].currentHP = pc_team.poke[4].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[4].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke[5].ko == false && pc_team.poke.size() >= 6 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[5].stats[0] - pc_team.poke[5].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[5].currentHP = pc_team.poke[5].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[5].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                        } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                    }
                    else if (battleInput == 'k') {
                        // Use Revive
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Use Revive on who?");
                        wrefresh(battle);
                        wrefresh(win);
                        
                        do {
                            battleInput = wgetch(battle);
                            if (battleInput == '1') {
                                if (pc_team.poke[0].ko == true && pc_team.poke.size() >= 1 && pc_team.items.revives > 0) {
                                    pc_team.poke[0].currentHP = pc_team.poke[0].stats[0] / 2;
                                    pc_team.poke[0].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '2') {
                                if (pc_team.poke[1].ko == true && pc_team.poke.size() >= 2 && pc_team.items.revives > 0) {
                                    pc_team.poke[1].currentHP = pc_team.poke[1].stats[0] / 2;
                                    pc_team.poke[1].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke[2].ko == true && pc_team.poke.size() >= 3 && pc_team.items.revives > 0) {
                                    pc_team.poke[2].currentHP = pc_team.poke[2].stats[0] / 2;
                                    pc_team.poke[2].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke[3].ko == true && pc_team.poke.size() >= 4 && pc_team.items.revives > 0) {
                                    pc_team.poke[3].currentHP = pc_team.poke[3].stats[0] / 2;
                                    pc_team.poke[3].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke[4].ko == true && pc_team.poke.size() >= 5 && pc_team.items.revives > 0) {
                                    pc_team.poke[4].currentHP = pc_team.poke[4].stats[0] / 2;
                                    pc_team.poke[4].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke[5].ko == true && pc_team.poke.size() >= 6 && pc_team.items.revives > 0) {
                                    pc_team.poke[5].currentHP = pc_team.poke[5].stats[0] / 2;
                                    pc_team.poke[5].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            //break;
                        } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                    }
                    else if (battleInput == 'l') {
                        mvwprintw(battle, 17, 1, "                                                                            ");
                        mvwprintw(battle, 18, 1, "                                                                            ");
                        mvwprintw(battle, 19, 1, "                                                                            ");
                        mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                        wrefresh(battle);
                        break;
                    }
                    break;
                } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k' || battleInput != 'l');
            }
            else if (battleInput == 'k') {
                // Pokemon
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");

                pokemonOption(battle);
                wrefresh(battle);
                
                do {
                    battleInput = wgetch(battle);
                    if (battleInput == '1') {
                        break;
                    }
                    else if (battleInput == '2') {
                        if (pc_team.poke.size() >= 2) {
                            if (pc_team.poke[1].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[1]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '3') {
                        if (pc_team.poke.size() >= 3) {
                            if (pc_team.poke[2].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[2]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '4') {
                        if (pc_team.poke.size() >= 4) {
                            if (pc_team.poke[3].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[3]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '5') {
                        if (pc_team.poke.size() >= 5) {
                            if (pc_team.poke[4].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[4]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '6') {
                        if (pc_team.poke.size() >= 6) {
                            if (pc_team.poke[5].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[5]);
                                break;
                            }
                        }
                    }
                } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
            }
            else if (battleInput == 'l') {
                // Run
                //battleEnd = true;
            }
        }

        wrefresh(battle);
        refresh();
    }
    else if (index >= 0) {
        bool battleEnd = false;
        char battleInput;
        int pcPriority = 0;
        int wildPriority = 0;
        int j = 0;

        mvwprintw(battle, 1, 42, "Trainer Battle:");
        wrefresh(battle);

        while (battleEnd == false) {
            bool playerWent = false;

            mvwprintw(battle, 3, 2, "                                      ");
            mvwprintw(battle, 3, 2, "Current Pokemon: %s", pc_team.poke[0].identifier.c_str());
            mvwprintw(battle, 3, 42, "                                    ");
            mvwprintw(battle, 3, 42, "Opponent Pokemon: %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str());

            mvwprintw(battle, 4, 2, "                                      ");
            mvwprintw(battle, 4, 2, "Lvl: %d", pc_team.poke[0].lvl);
            mvwprintw(battle, 4, 42, "Lvl: %d", world[yCoord][xCoord].npc[index].poke[0].lvl);

            mvwprintw(battle, 5, 2, "                                      ");
            mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
            mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);

            mvwprintw(battle, 6, 2, "                                      ");
            mvwprintw(battle, 6, 2, "Speed: %d", pc_team.poke[0].stats[5]);
            mvwprintw(battle, 6, 42, "Speed: %d", world[yCoord][xCoord].npc[index].poke[0].stats[5]);

            mvwprintw(battle, 8, 42, "Number of Pokemon: %d", world[yCoord][xCoord].npc[index].poke.size());

            mvwprintw(battle, 17, 1, "                                                                            ");
            mvwprintw(battle, 18, 1, "                                                                            ");
            mvwprintw(battle, 19, 1, "                                                                            ");
            mvwprintw(battle, 18, 15, "h. Fight      j. Bag      k. Pokemon      l. Run");
            wrefresh(battle);
            battleInput = wgetch(battle);

            if (battleInput == 'h') {
                // Attack
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");
                mvwprintw(battle, 18, 2, "h. %s      j. %s      k. Go back", pc_team.poke[0].moves[0].c_str(), pc_team.poke[0].moves[1].c_str());
                wrefresh(battle);

                do {
                    battleInput = wgetch(battle);
                    if (battleInput == 'h') {
                        // Attack with move 1
                        int move = rand() % 2;
                        int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], world[yCoord][xCoord].npc[index].poke[0].stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[0], pc_team.poke[0].typing);
                        int oppDamage = calculateDamage(world[yCoord][xCoord].npc[index].poke[0].id, world[yCoord][xCoord].npc[index].poke[0].lvl, world[yCoord][xCoord].npc[index].poke[0].stats[1], pc_team.poke[0].stats[2], world[yCoord][xCoord].npc[index].poke[0].stats[5], world[yCoord][xCoord].npc[index].poke[0].moves[move], world[yCoord][xCoord].npc[index].poke[0].typing);

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (pc_team.poke[0].moves[0] == pokemonData.move[i].identifier) {
                                pcPriority = pokemonData.move[i].priority;
                            }
                        }

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (world[yCoord][xCoord].npc[index].poke[0].moves[move] == pokemonData.move[i].identifier) {
                                wildPriority = pokemonData.move[i].priority;
                            }
                        }

                        if (pcPriority > wildPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                            if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                world[yCoord][xCoord].npc[index].poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (world[yCoord][xCoord].npc[index].poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (wildPriority > pcPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                    world[yCoord][xCoord].npc[index].poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pcPriority == wildPriority) {
                            if (pc_team.poke[0].stats[5] >= world[yCoord][xCoord].npc[index].poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                    world[yCoord][xCoord].npc[index].poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (world[yCoord][xCoord].npc[index].poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    pc_team.poke[0].currentHP -= oppDamage;
                                    if (pc_team.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                        pc_team.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                            else if (pc_team.poke[0].stats[5] < world[yCoord][xCoord].npc[index].poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (pc_team.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                                    if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                        world[yCoord][xCoord].npc[index].poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                        }

                        if (world[yCoord][xCoord].npc[index].poke[0].ko == true) {
                            if (world[yCoord][xCoord].npc[index].poke.size() == 1) {
                                battleEnd = true;
                            }
                            else if (world[yCoord][xCoord].npc[index].poke.size() > 1) {
                                swap(world[yCoord][xCoord].npc[index].poke[0], world[yCoord][xCoord].npc[index].poke[world[yCoord][xCoord].npc[index].poke.size() - 1]);
                                world[yCoord][xCoord].npc[index].poke.pop_back();
                            }
                        }

                        if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);

                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }
                        else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                            int one = 0, two = 0, three = 0, four = 0, five = 0;
                            int count = 0;
                            pokemonOption(battle);
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "Who will I switch in?");
                            wrefresh(battle);
                            wrefresh(win);

                            for (int i = 0; i < pc_team.poke.size(); i++) {
                                if (pc_team.poke[i].ko == false) {
                                    count++;
                                }
                            }

                            if (count == 0) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(2000000);

                                wclear(battle);
                                wrefresh(battle);
                                delwin(battle);
                                wclear(win);
                                wrefresh(win);
                                delwin(win);
                                endwin();
                                refresh();

                                exit(0);
                            }

                            do {
                                battleInput = wgetch(battle);
                                if (battleInput == '2') {
                                    if (pc_team.poke.size() >= 2) {
                                        if (pc_team.poke[1].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[1]);
                                            break;
                                        }
                                        else {
                                            one = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '3') {
                                    if (pc_team.poke.size() >= 3) {
                                        if (pc_team.poke[2].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[2]);
                                            break;
                                        }
                                        else {
                                            two = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '4') {
                                    if (pc_team.poke.size() >= 4) {
                                        if (pc_team.poke[3].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[3]);
                                            break;
                                        }
                                        else {
                                            three = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '5') {
                                    if (pc_team.poke.size() >= 5) {
                                        if (pc_team.poke[4].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[4]);
                                            break;
                                        }
                                        else {
                                            four = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '6') {
                                    if (pc_team.poke.size() >= 6) {
                                        if (pc_team.poke[5].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[5]);
                                            break;
                                        }
                                        else {
                                            five = 1;
                                        }
                                    }
                                }
                            } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                        }

                        playerWent = true;
                        break;
                    }
                    else if (battleInput == 'j') {
                        // Attack with move 2
                        int move = rand() % 2;
                        int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], world[yCoord][xCoord].npc[index].poke[0].stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[1], pc_team.poke[0].typing);
                        int oppDamage = calculateDamage(world[yCoord][xCoord].npc[index].poke[0].id, world[yCoord][xCoord].npc[index].poke[0].lvl, world[yCoord][xCoord].npc[index].poke[0].stats[1], pc_team.poke[0].stats[2], world[yCoord][xCoord].npc[index].poke[0].stats[5], world[yCoord][xCoord].npc[index].poke[0].moves[move], world[yCoord][xCoord].npc[index].poke[0].typing);

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (pc_team.poke[0].moves[1] == pokemonData.move[i].identifier) {
                                pcPriority = pokemonData.move[i].priority;
                            }
                        }

                        for (i = 0; i < pokemonData.move.size(); i++) {
                            if (world[yCoord][xCoord].npc[index].poke[0].moves[move] == pokemonData.move[i].identifier) {
                                wildPriority = pokemonData.move[i].priority;
                            }
                        }

                        if (pcPriority > wildPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                            if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                world[yCoord][xCoord].npc[index].poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (world[yCoord][xCoord].npc[index].poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (wildPriority > pcPriority) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                    world[yCoord][xCoord].npc[index].poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pcPriority == wildPriority) {
                            if (pc_team.poke[0].stats[5] >= world[yCoord][xCoord].npc[index].poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                                if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                    world[yCoord][xCoord].npc[index].poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (world[yCoord][xCoord].npc[index].poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    pc_team.poke[0].currentHP -= oppDamage;
                                    if (pc_team.poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                        pc_team.poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                            else if (pc_team.poke[0].stats[5] < world[yCoord][xCoord].npc[index].poke[0].stats[5]) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", world[yCoord][xCoord].npc[index].poke[0].identifier.c_str(), world[yCoord][xCoord].npc[index].poke[0].moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);

                                if (pc_team.poke[0].ko == false) {
                                    mvwprintw(win, 0, 0, "                                                                                ");
                                    mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                    
                                    world[yCoord][xCoord].npc[index].poke[0].currentHP -= yourDamage;
                                    if (world[yCoord][xCoord].npc[index].poke[0].currentHP <= 0) {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", 0, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                        world[yCoord][xCoord].npc[index].poke[0].ko = true;
                                    }
                                    else {
                                        mvwprintw(battle, 5, 42, "                                 ");
                                        mvwprintw(battle, 5, 42, "HP: %d/%d", world[yCoord][xCoord].npc[index].poke[0].currentHP, world[yCoord][xCoord].npc[index].poke[0].stats[0]);
                                    }
                                    wrefresh(battle);
                                    wrefresh(win);
                                    usleep(1000000);
                                }
                            }
                        }

                        if (world[yCoord][xCoord].npc[index].poke[0].ko == true) {
                            if (world[yCoord][xCoord].npc[index].poke.size() == 1) {
                                battleEnd = true;
                            }
                            else if (world[yCoord][xCoord].npc[index].poke.size() > 1) {
                                swap(world[yCoord][xCoord].npc[index].poke[0], world[yCoord][xCoord].npc[index].poke[world[yCoord][xCoord].npc[index].poke.size() - 1]);
                                world[yCoord][xCoord].npc[index].poke.pop_back();
                            }
                        }

                        if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);
                            
                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }
                        else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                            int one = 0, two = 0, three = 0, four = 0, five = 0;
                            int count = 0;
                            pokemonOption(battle);
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "Who will I switch in?");
                            wrefresh(battle);
                            wrefresh(win);

                            for (int i = 0; i < pc_team.poke.size(); i++) {
                                if (pc_team.poke[i].ko == false) {
                                    count++;
                                }
                            }

                            if (count == 0) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(2000000);

                                wclear(battle);
                                wrefresh(battle);
                                delwin(battle);
                                wclear(win);
                                wrefresh(win);
                                delwin(win);
                                endwin();
                                refresh();

                                exit(0);
                            }

                            do {
                                battleInput = wgetch(battle);
                                if (battleInput == '2') {
                                    if (pc_team.poke.size() >= 2) {
                                        if (pc_team.poke[1].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[1]);
                                            break;
                                        }
                                        else {
                                            one = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '3') {
                                    if (pc_team.poke.size() >= 3) {
                                        if (pc_team.poke[2].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[2]);
                                            break;
                                        }
                                        else {
                                            two = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '4') {
                                    if (pc_team.poke.size() >= 4) {
                                        if (pc_team.poke[3].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[3]);
                                            break;
                                        }
                                        else {
                                            three = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '5') {
                                    if (pc_team.poke.size() >= 5) {
                                        if (pc_team.poke[4].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[4]);
                                            break;
                                        }
                                        else {
                                            four = 1;
                                        }
                                    }
                                }
                                else if (battleInput == '6') {
                                    if (pc_team.poke.size() >= 6) {
                                        if (pc_team.poke[5].ko == false) {
                                            swap(pc_team.poke[0], pc_team.poke[5]);
                                            break;
                                        }
                                        else {
                                            five = 1;
                                        }
                                    }
                                }
                            } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                        }

                        playerWent = true;
                        break;
                    }
                    else if (battleInput == 'k') {
                        mvwprintw(battle, 17, 1, "                                                                            ");
                        mvwprintw(battle, 18, 1, "                                                                            ");
                        mvwprintw(battle, 19, 1, "                                                                            ");
                        mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                        wrefresh(battle);
                        break;
                    }
                } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k');
            }
            else if (battleInput == 'j') {
                // Bag
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");
                mvwprintw(battle, 18, 2, "h. Pokeball (%d)     j. Potion (%d)     k. Revive (%d)      l. Go back", pc_team.items.pokeballs, pc_team.items.potions, pc_team.items.revives);
                wrefresh(battle);

                do {
                    battleInput = wgetch(battle);
                    if (battleInput == 'h') {
                        // Use Pokeball
                        break;
                    }
                    else if (battleInput == 'j') {
                        // Use Potion
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Use Potion on who?");
                        wrefresh(battle);
                        wrefresh(win);
                        
                        do {
                            battleInput = wgetch(battle);
                            int deficit;
                            if (battleInput == '1') {
                                if (pc_team.poke[0].ko == false && pc_team.poke.size() >= 1 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[0].stats[0] - pc_team.poke[0].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[0].currentHP = pc_team.poke[0].stats[0];
                                        pc_team.items.potions--;
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                        wrefresh(battle);
                                    }
                                    else {
                                        pc_team.poke[0].currentHP += 20;
                                        pc_team.items.potions--;
                                        mvwprintw(battle, 5, 2, "                                      ");
                                        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                        wrefresh(battle);
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '2') {
                                if (pc_team.poke[1].ko == false && pc_team.poke.size() >= 2 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[1].stats[0] - pc_team.poke[1].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[1].currentHP = pc_team.poke[1].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[1].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke[2].ko == false && pc_team.poke.size() >= 3 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[2].stats[0] - pc_team.poke[2].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[2].currentHP = pc_team.poke[2].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[2].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke[3].ko == false && pc_team.poke.size() >= 4 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[3].stats[0] - pc_team.poke[3].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[3].currentHP = pc_team.poke[3].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[3].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke[4].ko == false && pc_team.poke.size() >= 5 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[4].stats[0] - pc_team.poke[4].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[4].currentHP = pc_team.poke[4].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[4].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke[5].ko == false && pc_team.poke.size() >= 6 && pc_team.items.potions > 0) {
                                    deficit = pc_team.poke[5].stats[0] - pc_team.poke[5].currentHP;
                                    if (deficit <= 20) {
                                        pc_team.poke[5].currentHP = pc_team.poke[5].stats[0];
                                        pc_team.items.potions--;
                                    }
                                    else {
                                        pc_team.poke[5].currentHP += 20;
                                        pc_team.items.potions--;
                                    }
                                }
                                break;
                            }
                        } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                    }
                    else if (battleInput == 'k') {
                        // Use Revive
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Use Revive on who?");
                        wrefresh(battle);
                        wrefresh(win);
                        
                        do {
                            battleInput = wgetch(battle);
                            if (battleInput == '1') {
                                if (pc_team.poke[0].ko == true && pc_team.poke.size() >= 1 && pc_team.items.revives > 0) {
                                    pc_team.poke[0].currentHP = pc_team.poke[0].stats[0] / 2;
                                    pc_team.poke[0].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '2') {
                                if (pc_team.poke[1].ko == true && pc_team.poke.size() >= 2 && pc_team.items.revives > 0) {
                                    pc_team.poke[1].currentHP = pc_team.poke[1].stats[0] / 2;
                                    pc_team.poke[1].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke[2].ko == true && pc_team.poke.size() >= 3 && pc_team.items.revives > 0) {
                                    pc_team.poke[2].currentHP = pc_team.poke[2].stats[0] / 2;
                                    pc_team.poke[2].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke[3].ko == true && pc_team.poke.size() >= 4 && pc_team.items.revives > 0) {
                                    pc_team.poke[3].currentHP = pc_team.poke[3].stats[0] / 2;
                                    pc_team.poke[3].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke[4].ko == true && pc_team.poke.size() >= 5 && pc_team.items.revives > 0) {
                                    pc_team.poke[4].currentHP = pc_team.poke[4].stats[0] / 2;
                                    pc_team.poke[4].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke[5].ko == true && pc_team.poke.size() >= 6 && pc_team.items.revives > 0) {
                                    pc_team.poke[5].currentHP = pc_team.poke[5].stats[0] / 2;
                                    pc_team.poke[5].ko == false;
                                    pc_team.items.revives--;
                                }
                                break;
                            }
                            //break;
                        } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                    }
                    else if (battleInput == 'l') {
                        mvwprintw(battle, 17, 1, "                                                                            ");
                        mvwprintw(battle, 18, 1, "                                                                            ");
                        mvwprintw(battle, 19, 1, "                                                                            ");
                        mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                        wrefresh(battle);
                        break;
                    }
                    break;
                } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k' || battleInput != 'l');
            }
            else if (battleInput == 'k') {
                // Pokemon
                mvwprintw(battle, 17, 1, "                                                                            ");
                mvwprintw(battle, 18, 1, "                                                                            ");
                mvwprintw(battle, 19, 1, "                                                                            ");

                pokemonOption(battle);
                wrefresh(battle);
                
                do {
                    battleInput = wgetch(battle);
                    if (battleInput == '1') {
                        break;
                    }
                    else if (battleInput == '2') {
                        if (pc_team.poke.size() >= 2) {
                            if (pc_team.poke[1].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[1]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '3') {
                        if (pc_team.poke.size() >= 3) {
                            if (pc_team.poke[2].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[2]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '4') {
                        if (pc_team.poke.size() >= 4) {
                            if (pc_team.poke[3].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[3]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '5') {
                        if (pc_team.poke.size() >= 5) {
                            if (pc_team.poke[4].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[4]);
                                break;
                            }
                        }
                    }
                    else if (battleInput == '6') {
                        if (pc_team.poke.size() >= 6) {
                            if (pc_team.poke[5].ko == false) {
                                swap(pc_team.poke[0], pc_team.poke[5]);
                                break;
                            }
                        }
                    }
                } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
            }
            else if (battleInput == 'l') {
                // Run
                //battleEnd = true;
            }
        }

        wrefresh(battle);
        refresh();
    }
    wrefresh(battle);
    refresh();

}

void openMart(int xCoord, int yCoord, WINDOW *mart) {
    char martInput;

    box(mart, 0, 0);
    mvwprintw(mart, 8, 9, "*All Items Replenished! Press '<' to exit*");
    replenishItems();
    wrefresh(mart);
    refresh();

    while (martInput != '<') {
        martInput = getch();
    }
}

void openCenter(int xCoord, int yCoord, WINDOW *center) {
    char centerInput;

    box(center, 0, 0);
    mvwprintw(center, 8, 8, "*All Pokemon Healed! Press '<' to exit*");
    replenishTeam();
    wrefresh(center);
    refresh();

    while (centerInput != '<') {
        centerInput = getch();
    }
}

void openFly(int xCoord, int yCoord, WINDOW *fly) {
    box(fly, 0, 0);
    mvwprintw(fly, 1, 1, "Where would you like to go?");
    wrefresh(fly);
    refresh();

    wscanw(fly, "%d %d", &usrX, &usrY);
}

void initiateCaughtPokemon() {

}

void initiatePlayerTeam() {
    pc_team.poke[0].typing = getTypings(pc_team.poke[0].id);
    pc_team.poke[0].shiny = isShiny();
    pc_team.poke[0].moves = getMoves(pc_team.poke[0].species_id, pc_team.poke[0].lvl);
    pc_team.poke[0].ivs = getIV();
    pc_team.poke[0].stats = getStats(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].ivs);
    pc_team.poke[0].currentHP = pc_team.poke[0].stats[0];
    pc_team.poke[0].ko = false;

    for (int i = 0; i < pokemonData.poke_spec.size(); i++) {
        if (pc_team.poke[0].id == pokemonData.poke_spec[i].id) {
            pc_team.poke[0].growth_id = pokemonData.poke_spec[i].growth_rate_id;
        }
    }
    for (int i = 0; i < pokemonData.exp.size(); i++) {
        if (pc_team.poke[0].lvl == pokemonData.exp[i].level && pc_team.poke[0].growth_id == pokemonData.exp[i].growth_rate_id) {
            pc_team.poke[0].currentEXP = pokemonData.exp[i].experience;
        }
    }
}

void viewTeam(WINDOW *team) {
    char teamInput;
    int i, j, k;

    box(team, 0, 0);
    mvwprintw(team, 1, 2, "This is your starting team, good luck!");
    mvwprintw(team, 2, 2, "You picked a lvl %d %s (ID: %d, Species ID: %d)", pc_team.poke[0].lvl, pc_team.poke[0].identifier.c_str(), pc_team.poke[0].id, pc_team.poke[0].species_id);
    if (pc_team.poke[0].typing.size() > 1) {
        mvwprintw(team, 4, 2, "%s is a %s/%s type.    Shiny: %d", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].typing[0].c_str(), pc_team.poke[0].typing[1].c_str(), pc_team.poke[0].shiny);
    }
    else {
        mvwprintw(team, 4, 2, "%s is a %s type.    Shiny: %d", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].typing[0].c_str(), pc_team.poke[0].shiny);
    }
    mvwprintw(team, 5, 2, "Known Moves: %s, %s", pc_team.poke[0].moves[0].c_str(), pc_team.poke[0].moves[1].c_str());
    mvwprintw(team, 6, 2, "Stats:");
    for (i = 0; i < 6; i++) {
        mvwprintw(team, 7 + i, 2, "%s: %d", pokemonData.stat[i].identifier.c_str(), pc_team.poke[0].stats[i]);
    }

    wrefresh(team);
    refresh();

    while (teamInput != '\n') {
        teamInput = getch();
    }
}

void pickStarter(WINDOW *starter) {
    char starterInput;
    int currentChoice = 0;
    int randIndex[3];
    int i, j, k;

    box(starter, 0, 0);
    mvwprintw(starter, 1, 11, "Starter Selection");
    mvwprintw(starter, 3, 11, "Your choices are:");
    mvwprintw(starter, 2, 5, "('w' for up / 's' for down)");
    mvwprintw(starter, 15, 11, "('enter' to pick)");

    j = 5;
    for (i = 0; i < 3; i++) {
        randIndex[i] = (rand() % 1092);
        mvwprintw(starter, j, 13, "%s", pokemonData.poke[randIndex[i]].identifier.c_str());
        j += 4;
    }
    j = 0;
    mvwprintw(starter, 5, 11, "X"); // Can be 5, 9, or 13
    wrefresh(starter);
    refresh();

    while (starterInput != '\n') {
        starterInput = getch();

        if (starterInput == 'w') {
            if (currentChoice > 0) {
                currentChoice--;
            }
        }
        else if (starterInput == 's') {
            if (currentChoice < 2) {
                currentChoice++;
            }
        }

        mvwprintw(starter, 5, 11, " ");
        mvwprintw(starter, 9, 11, " ");
        mvwprintw(starter, 13, 11, " ");

        switch(currentChoice) {
            case 0:
                mvwprintw(starter, 5, 11, "X");
                break;
            case 1:
                mvwprintw(starter, 9, 11, "X");
                break;
            case 2:
                mvwprintw(starter, 13, 11, "X");
                break;
        }

        wrefresh(starter);
    }
    wrefresh(starter);
    refresh();

    pokemon p;
    p.id = pokemonData.poke[randIndex[currentChoice]].id;
    p.identifier = pokemonData.poke[randIndex[currentChoice]].identifier;
    p.species_id = pokemonData.poke[randIndex[currentChoice]].species_id;
    p.height = pokemonData.poke[randIndex[currentChoice]].height;
    p.weight = pokemonData.poke[randIndex[currentChoice]].weight;
    p.base_experience = pokemonData.poke[randIndex[currentChoice]].base_experience;
    p.order = pokemonData.poke[randIndex[currentChoice]].order;
    p.is_default = pokemonData.poke[randIndex[currentChoice]].is_default;

    pc_team.poke.push_back(p);
    pc_team.poke[0].index = randIndex[currentChoice];
    pc_team.poke[0].lvl = 1;
}

void pokemonBattle(int xCoord, int yCoord, WINDOW *battle, WINDOW *win) {
    int i;
    pokemon p;
    p.index = (rand() % 1092);
    p.id = pokemonData.poke[p.index].id;
    p.identifier = pokemonData.poke[p.index].identifier;
    p.species_id = pokemonData.poke[p.index].species_id;
    p.height = pokemonData.poke[p.index].height;
    p.weight = pokemonData.poke[p.index].weight;
    p.base_experience = pokemonData.poke[p.index].base_experience;
    p.order = pokemonData.poke[p.index].order;
    p.is_default = pokemonData.poke[p.index].is_default;
    p.lvl = loadPokemonlvl(xCoord, yCoord);
    p.typing = getTypings(p.id);
    p.shiny = isShiny();
    p.moves = getMoves(p.species_id, p.lvl);
    p.ivs = getIV();
    p.stats = getStats(p.id, p.lvl, p.ivs);
    p.currentHP = p.stats[0];
    p.ko = false;

    for (i = 0; i < pokemonData.poke_spec.size(); i++) {
        if (p.id == pokemonData.poke_spec[i].id) {
            p.growth_id = pokemonData.poke_spec[i].growth_rate_id;
        }
    }
    for (i = 0; i < pokemonData.exp.size(); i++) {
        if (p.lvl == pokemonData.exp[i].level && p.growth_id == pokemonData.exp[i].growth_rate_id) {
            p.currentEXP = pokemonData.exp[i].experience;
        }
    }

    bool battleEnd = false;
    char battleInput;
    int pcPriority = 0;
    int wildPriority = 0;
    int j = 0;

    box(battle, 0, 0);
    for (i = 1; i < COLS - 1; i++) {
        mvwprintw(battle, 16, i, "x");
    }
    for (i = 1; i < ROWS - 4; i++) {
        mvwprintw(battle, i, 40, "x");
    }
    mvwprintw(battle, 1, 2, "You:");
    mvwprintw(battle, 1, 42, "Wild Pokemon:");

    wrefresh(battle);

    while (battleEnd == false) {
        bool playerWent = false;

        mvwprintw(battle, 3, 2, "                                      ");
        mvwprintw(battle, 3, 2, "Current Pokemon: %s", pc_team.poke[0].identifier.c_str());
        mvwprintw(battle, 3, 42, "Opponent Pokemon: %s", p.identifier.c_str());

        mvwprintw(battle, 4, 2, "                                      ");
        mvwprintw(battle, 4, 2, "Lvl: %d", pc_team.poke[0].lvl);
        mvwprintw(battle, 4, 42, "Lvl: %d", p.lvl);

        mvwprintw(battle, 5, 2, "                                      ");
        mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
        mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);

        mvwprintw(battle, 6, 2, "                                      ");
        mvwprintw(battle, 6, 2, "Speed: %d", pc_team.poke[0].stats[5]);
        mvwprintw(battle, 6, 42, "Speed: %d", p.stats[5]);

        mvwprintw(battle, 17, 1, "                                                                            ");
        mvwprintw(battle, 18, 1, "                                                                            ");
        mvwprintw(battle, 19, 1, "                                                                            ");
        mvwprintw(battle, 18, 15, "h. Fight      j. Bag      k. Pokemon      l. Run");
        wrefresh(battle);
        battleInput = wgetch(battle);

        if (battleInput == 'h') {
            // Attack
            mvwprintw(battle, 17, 1, "                                                                            ");
            mvwprintw(battle, 18, 1, "                                                                            ");
            mvwprintw(battle, 19, 1, "                                                                            ");
            mvwprintw(battle, 18, 2, "h. %s      j. %s      k. Go back", pc_team.poke[0].moves[0].c_str(), pc_team.poke[0].moves[1].c_str());
            wrefresh(battle);

            do {
                battleInput = wgetch(battle);
                if (battleInput == 'h') {
                    // Attack with move 1
                    int move = rand() % 2;
                    int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], p.stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[0], pc_team.poke[0].typing);
                    int oppDamage = calculateDamage(p.id, p.lvl, p.stats[1], pc_team.poke[0].stats[2], p.stats[5], p.moves[move], p.typing);

                    for (i = 0; i < pokemonData.move.size(); i++) {
                        if (pc_team.poke[0].moves[0] == pokemonData.move[i].identifier) {
                            pcPriority = pokemonData.move[i].priority;
                        }
                    }

                    for (i = 0; i < pokemonData.move.size(); i++) {
                        if (p.moves[move] == pokemonData.move[i].identifier) {
                            wildPriority = pokemonData.move[i].priority;
                        }
                    }

                    if (pcPriority > wildPriority) {
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);
                        
                        p.currentHP -= yourDamage;
                        if (p.currentHP <= 0) {
                            mvwprintw(battle, 5, 42, "                                 ");
                            mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                            p.ko = true;
                        }
                        else {
                            mvwprintw(battle, 5, 42, "                                 ");
                            mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                        }
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);

                        if (p.ko == false) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                        }
                    }
                    else if (wildPriority > pcPriority) {
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);
                        
                        pc_team.poke[0].currentHP -= oppDamage;
                        if (pc_team.poke[0].currentHP <= 0) {
                            mvwprintw(battle, 5, 2, "                                      ");
                            mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                            pc_team.poke[0].ko = true;
                        }
                        else {
                            mvwprintw(battle, 5, 2, "                                      ");
                            mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                        }
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);

                        if (pc_team.poke[0].ko == false) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            p.currentHP -= yourDamage;
                            if (p.currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                                p.ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                        }
                    }
                    else if (pcPriority == wildPriority) {
                        if (pc_team.poke[0].stats[5] >= p.stats[5]) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            p.currentHP -= yourDamage;
                            if (p.currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                                p.ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (p.ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pc_team.poke[0].stats[5] < p.stats[5]) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[0].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                p.currentHP -= yourDamage;
                                if (p.currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                                    p.ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                    }

                    if (p.ko == true) {
                        battleEnd = true;
                    }

                    if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(2000000);

                        wclear(battle);
                        wrefresh(battle);
                        delwin(battle);
                        wclear(win);
                        wrefresh(win);
                        delwin(win);
                        endwin();
                        refresh();

                        exit(0);
                    }
                    else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                        int one = 0, two = 0, three = 0, four = 0, five = 0;
                        int count = 0;
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Who will I switch in?");
                        wrefresh(battle);
                        wrefresh(win);

                        for (int i = 0; i < pc_team.poke.size(); i++) {
                            if (pc_team.poke[i].ko == false) {
                                count++;
                            }
                        }

                        if (count == 0) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);

                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }

                        do {
                            battleInput = wgetch(battle);
                            if (battleInput == '2') {
                                if (pc_team.poke.size() >= 2) {
                                    if (pc_team.poke[1].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[1]);
                                        break;
                                    }
                                    else {
                                        one = 1;
                                    }
                                }
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke.size() >= 3) {
                                    if (pc_team.poke[2].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[2]);
                                        break;
                                    }
                                    else {
                                        two = 1;
                                    }
                                }
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke.size() >= 4) {
                                    if (pc_team.poke[3].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[3]);
                                        break;
                                    }
                                    else {
                                        three = 1;
                                    }
                                }
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke.size() >= 5) {
                                    if (pc_team.poke[4].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[4]);
                                        break;
                                    }
                                    else {
                                        four = 1;
                                    }
                                }
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke.size() >= 6) {
                                    if (pc_team.poke[5].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[5]);
                                        break;
                                    }
                                    else {
                                        five = 1;
                                    }
                                }
                            }
                        } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                    }

                    playerWent = true;
                    break;
                }
                else if (battleInput == 'j') {
                    // Attack with move 2
                    int move = rand() % 2;
                    int yourDamage = calculateDamage(pc_team.poke[0].id, pc_team.poke[0].lvl, pc_team.poke[0].stats[1], p.stats[2], pc_team.poke[0].stats[5], pc_team.poke[0].moves[1], pc_team.poke[0].typing);
                    int oppDamage = calculateDamage(p.id, p.lvl, p.stats[1], pc_team.poke[0].stats[2], p.stats[5], p.moves[move], p.typing);

                    for (i = 0; i < pokemonData.move.size(); i++) {
                        if (pc_team.poke[0].moves[1] == pokemonData.move[i].identifier) {
                            pcPriority = pokemonData.move[i].priority;
                        }
                    }

                    for (i = 0; i < pokemonData.move.size(); i++) {
                        if (p.moves[move] == pokemonData.move[i].identifier) {
                            wildPriority = pokemonData.move[i].priority;
                        }
                    }

                    if (pcPriority > wildPriority) {
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);
                        
                        p.currentHP -= yourDamage;
                        if (p.currentHP <= 0) {
                            mvwprintw(battle, 5, 42, "                                 ");
                            mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                            p.ko = true;
                        }
                        else {
                            mvwprintw(battle, 5, 42, "                                 ");
                            mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                        }
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);

                        if (p.ko == false) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                        }
                    }
                    else if (wildPriority > pcPriority) {
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);
                        
                        pc_team.poke[0].currentHP -= oppDamage;
                        if (pc_team.poke[0].currentHP <= 0) {
                            mvwprintw(battle, 5, 2, "                                      ");
                            mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                            pc_team.poke[0].ko = true;
                        }
                        else {
                            mvwprintw(battle, 5, 2, "                                      ");
                            mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                        }
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(1000000);

                        if (pc_team.poke[0].ko == false) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            p.currentHP -= yourDamage;
                            if (p.currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                                p.ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                        }
                    }
                    else if (pcPriority == wildPriority) {
                        if (pc_team.poke[0].stats[5] >= p.stats[5]) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            p.currentHP -= yourDamage;
                            if (p.currentHP <= 0) {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                                p.ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 42, "                                 ");
                                mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (p.ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                pc_team.poke[0].currentHP -= oppDamage;
                                if (pc_team.poke[0].currentHP <= 0) {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                    pc_team.poke[0].ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                        else if (pc_team.poke[0].stats[5] < p.stats[5]) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "%s used %s", p.identifier.c_str(), p.moves[move].c_str());
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);
                            
                            pc_team.poke[0].currentHP -= oppDamage;
                            if (pc_team.poke[0].currentHP <= 0) {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", 0, pc_team.poke[0].stats[0]);
                                pc_team.poke[0].ko = true;
                            }
                            else {
                                mvwprintw(battle, 5, 2, "                                      ");
                                mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                            }
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(1000000);

                            if (pc_team.poke[0].ko == false) {
                                mvwprintw(win, 0, 0, "                                                                                ");
                                mvwprintw(win, 0, 0, "%s used %s", pc_team.poke[0].identifier.c_str(), pc_team.poke[0].moves[1].c_str());
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                                
                                p.currentHP -= yourDamage;
                                if (p.currentHP <= 0) {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", 0, p.stats[0]);
                                    p.ko = true;
                                }
                                else {
                                    mvwprintw(battle, 5, 42, "                                 ");
                                    mvwprintw(battle, 5, 42, "HP: %d/%d", p.currentHP, p.stats[0]);
                                }
                                wrefresh(battle);
                                wrefresh(win);
                                usleep(1000000);
                            }
                        }
                    }

                    if (p.ko == true) {
                        battleEnd = true;
                    }

                    if (pc_team.poke[0].ko == true && pc_team.poke.size() == 1) {
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                        wrefresh(battle);
                        wrefresh(win);
                        usleep(2000000);
                        
                        wclear(battle);
                        wrefresh(battle);
                        delwin(battle);
                        wclear(win);
                        wrefresh(win);
                        delwin(win);
                        endwin();
                        refresh();

                        exit(0);
                    }
                    else if (pc_team.poke[0].ko == true && pc_team.poke.size() > 1) {
                        int one = 0, two = 0, three = 0, four = 0, five = 0;
                        int count = 0;
                        pokemonOption(battle);
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "Who will I switch in?");
                        wrefresh(battle);
                        wrefresh(win);

                        for (int i = 0; i < pc_team.poke.size(); i++) {
                            if (pc_team.poke[i].ko == false) {
                                count++;
                            }
                        }

                        if (count == 0) {
                            mvwprintw(win, 0, 0, "                                                                                ");
                            mvwprintw(win, 0, 0, "You are out of useable Pokemon, please play again!");
                            wrefresh(battle);
                            wrefresh(win);
                            usleep(2000000);

                            wclear(battle);
                            wrefresh(battle);
                            delwin(battle);
                            wclear(win);
                            wrefresh(win);
                            delwin(win);
                            endwin();
                            refresh();

                            exit(0);
                        }

                        do {
                            battleInput = wgetch(battle);
                            if (battleInput == '2') {
                                if (pc_team.poke.size() >= 2) {
                                    if (pc_team.poke[1].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[1]);
                                        break;
                                    }
                                    else {
                                        one = 1;
                                    }
                                }
                            }
                            else if (battleInput == '3') {
                                if (pc_team.poke.size() >= 3) {
                                    if (pc_team.poke[2].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[2]);
                                        break;
                                    }
                                    else {
                                        two = 1;
                                    }
                                }
                            }
                            else if (battleInput == '4') {
                                if (pc_team.poke.size() >= 4) {
                                    if (pc_team.poke[3].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[3]);
                                        break;
                                    }
                                    else {
                                        three = 1;
                                    }
                                }
                            }
                            else if (battleInput == '5') {
                                if (pc_team.poke.size() >= 5) {
                                    if (pc_team.poke[4].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[4]);
                                        break;
                                    }
                                    else {
                                        four = 1;
                                    }
                                }
                            }
                            else if (battleInput == '6') {
                                if (pc_team.poke.size() >= 6) {
                                    if (pc_team.poke[5].ko == false) {
                                        swap(pc_team.poke[0], pc_team.poke[5]);
                                        break;
                                    }
                                    else {
                                        five = 1;
                                    }
                                }
                            }
                        } while (one != 1 && two != 1 && three != 1 && four != 1 && five != 1);
                    }

                    playerWent = true;
                    break;
                }
                else if (battleInput == 'k') {
                    mvwprintw(battle, 17, 1, "                                                                            ");
                    mvwprintw(battle, 18, 1, "                                                                            ");
                    mvwprintw(battle, 19, 1, "                                                                            ");
                    mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                    wrefresh(battle);
                    break;
                }
            } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k');
        }
        else if (battleInput == 'j') {
            // Bag
            mvwprintw(battle, 17, 1, "                                                                            ");
            mvwprintw(battle, 18, 1, "                                                                            ");
            mvwprintw(battle, 19, 1, "                                                                            ");
            mvwprintw(battle, 18, 2, "h. Pokeball (%d)     j. Potion (%d)     k. Revive (%d)      l. Go back", pc_team.items.pokeballs, pc_team.items.potions, pc_team.items.revives);
            wrefresh(battle);

            do {
                battleInput = wgetch(battle);
                if (battleInput == 'h') {
                    // Use Pokeball
                    if (pc_team.poke.size() < 6) {
                        pc_team.poke.push_back(p);
                        pc_team.items.pokeballs--;
                        mvwprintw(win, 0, 0, "                                                                                ");
                        mvwprintw(win, 0, 0, "You caught %s!", p.identifier.c_str());
                        wrefresh(win);
                        usleep(1000000);
                        battleEnd = true;
                    }
                    break;
                }
                else if (battleInput == 'j') {
                    // Use Potion
                    pokemonOption(battle);
                    mvwprintw(win, 0, 0, "                                                                                ");
                    mvwprintw(win, 0, 0, "Use Potion on who?");
                    wrefresh(battle);
                    wrefresh(win);
                    
                    do {
                        battleInput = wgetch(battle);
                        int deficit;
                        if (battleInput == '1') {
                            if (pc_team.poke[0].ko == false && pc_team.poke.size() >= 1 && pc_team.items.potions > 0) {
                                deficit = pc_team.poke[0].stats[0] - pc_team.poke[0].currentHP;
                                if (deficit <= 20) {
                                    pc_team.poke[0].currentHP = pc_team.poke[0].stats[0];
                                    pc_team.items.potions--;
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    wrefresh(battle);
                                }
                                else {
                                    pc_team.poke[0].currentHP += 20;
                                    pc_team.items.potions--;
                                    mvwprintw(battle, 5, 2, "                                      ");
                                    mvwprintw(battle, 5, 2, "HP: %d/%d", pc_team.poke[0].currentHP, pc_team.poke[0].stats[0]);
                                    wrefresh(battle);
                                }
                            }
                            break;
                        }
                        else if (battleInput == '2') {
                            if (pc_team.poke[1].ko == false && pc_team.poke.size() >= 2 && pc_team.items.potions > 0) {
                                deficit = pc_team.poke[1].stats[0] - pc_team.poke[1].currentHP;
                                if (deficit <= 20) {
                                    pc_team.poke[1].currentHP = pc_team.poke[1].stats[0];
                                    pc_team.items.potions--;
                                }
                                else {
                                    pc_team.poke[1].currentHP += 20;
                                    pc_team.items.potions--;
                                }
                            }
                            break;
                        }
                        else if (battleInput == '3') {
                            if (pc_team.poke[2].ko == false && pc_team.poke.size() >= 3 && pc_team.items.potions > 0) {
                                deficit = pc_team.poke[2].stats[0] - pc_team.poke[2].currentHP;
                                if (deficit <= 20) {
                                    pc_team.poke[2].currentHP = pc_team.poke[2].stats[0];
                                    pc_team.items.potions--;
                                }
                                else {
                                    pc_team.poke[2].currentHP += 20;
                                    pc_team.items.potions--;
                                }
                            }
                            break;
                        }
                        else if (battleInput == '4') {
                            if (pc_team.poke[3].ko == false && pc_team.poke.size() >= 4 && pc_team.items.potions > 0) {
                                deficit = pc_team.poke[3].stats[0] - pc_team.poke[3].currentHP;
                                if (deficit <= 20) {
                                    pc_team.poke[3].currentHP = pc_team.poke[3].stats[0];
                                    pc_team.items.potions--;
                                }
                                else {
                                    pc_team.poke[3].currentHP += 20;
                                    pc_team.items.potions--;
                                }
                            }
                            break;
                        }
                        else if (battleInput == '5') {
                            if (pc_team.poke[4].ko == false && pc_team.poke.size() >= 5 && pc_team.items.potions > 0) {
                                deficit = pc_team.poke[4].stats[0] - pc_team.poke[4].currentHP;
                                if (deficit <= 20) {
                                    pc_team.poke[4].currentHP = pc_team.poke[4].stats[0];
                                    pc_team.items.potions--;
                                }
                                else {
                                    pc_team.poke[4].currentHP += 20;
                                    pc_team.items.potions--;
                                }
                            }
                            break;
                        }
                        else if (battleInput == '6') {
                            if (pc_team.poke[5].ko == false && pc_team.poke.size() >= 6 && pc_team.items.potions > 0) {
                                deficit = pc_team.poke[5].stats[0] - pc_team.poke[5].currentHP;
                                if (deficit <= 20) {
                                    pc_team.poke[5].currentHP = pc_team.poke[5].stats[0];
                                    pc_team.items.potions--;
                                }
                                else {
                                    pc_team.poke[5].currentHP += 20;
                                    pc_team.items.potions--;
                                }
                            }
                            break;
                        }
                    } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                }
                else if (battleInput == 'k') {
                    // Use Revive
                    pokemonOption(battle);
                    mvwprintw(win, 0, 0, "                                                                                ");
                    mvwprintw(win, 0, 0, "Use Revive on who?");
                    wrefresh(battle);
                    wrefresh(win);
                    
                    do {
                        battleInput = wgetch(battle);
                        if (battleInput == '1') {
                            if (pc_team.poke[0].ko == true && pc_team.poke.size() >= 1 && pc_team.items.revives > 0) {
                                pc_team.poke[0].currentHP = pc_team.poke[0].stats[0] / 2;
                                pc_team.poke[0].ko == false;
                                pc_team.items.revives--;
                            }
                            break;
                        }
                        else if (battleInput == '2') {
                            if (pc_team.poke[1].ko == true && pc_team.poke.size() >= 2 && pc_team.items.revives > 0) {
                                pc_team.poke[1].currentHP = pc_team.poke[1].stats[0] / 2;
                                pc_team.poke[1].ko == false;
                                pc_team.items.revives--;
                            }
                            break;
                        }
                        else if (battleInput == '3') {
                            if (pc_team.poke[2].ko == true && pc_team.poke.size() >= 3 && pc_team.items.revives > 0) {
                                pc_team.poke[2].currentHP = pc_team.poke[2].stats[0] / 2;
                                pc_team.poke[2].ko == false;
                                pc_team.items.revives--;
                            }
                            break;
                        }
                        else if (battleInput == '4') {
                            if (pc_team.poke[3].ko == true && pc_team.poke.size() >= 4 && pc_team.items.revives > 0) {
                                pc_team.poke[3].currentHP = pc_team.poke[3].stats[0] / 2;
                                pc_team.poke[3].ko == false;
                                pc_team.items.revives--;
                            }
                            break;
                        }
                        else if (battleInput == '5') {
                            if (pc_team.poke[4].ko == true && pc_team.poke.size() >= 5 && pc_team.items.revives > 0) {
                                pc_team.poke[4].currentHP = pc_team.poke[4].stats[0] / 2;
                                pc_team.poke[4].ko == false;
                                pc_team.items.revives--;
                            }
                            break;
                        }
                        else if (battleInput == '6') {
                            if (pc_team.poke[5].ko == true && pc_team.poke.size() >= 6 && pc_team.items.revives > 0) {
                                pc_team.poke[5].currentHP = pc_team.poke[5].stats[0] / 2;
                                pc_team.poke[5].ko == false;
                                pc_team.items.revives--;
                            }
                            break;
                        }
                        //break;
                    } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
                }
                else if (battleInput == 'l') {
                    mvwprintw(battle, 17, 1, "                                                                            ");
                    mvwprintw(battle, 18, 1, "                                                                            ");
                    mvwprintw(battle, 19, 1, "                                                                            ");
                    mvwprintw(battle, 18, 2, "h. Fight      j. Bag      k. Pokemon      l. Run");
                    wrefresh(battle);
                    break;
                }
                break;
            } while (battleInput != 'h' || battleInput != 'j' || battleInput != 'k' || battleInput != 'l');
        }
        else if (battleInput == 'k') {
            // Pokemon
            mvwprintw(battle, 17, 1, "                                                                            ");
            mvwprintw(battle, 18, 1, "                                                                            ");
            mvwprintw(battle, 19, 1, "                                                                            ");

            pokemonOption(battle);
            wrefresh(battle);
            
            do {
                battleInput = wgetch(battle);
                if (battleInput == '1') {
                    break;
                }
                else if (battleInput == '2') {
                    if (pc_team.poke.size() >= 2) {
                        if (pc_team.poke[1].ko == false) {
                            swap(pc_team.poke[0], pc_team.poke[1]);
                            break;
                        }
                    }
                }
                else if (battleInput == '3') {
                    if (pc_team.poke.size() >= 3) {
                        if (pc_team.poke[2].ko == false) {
                            swap(pc_team.poke[0], pc_team.poke[2]);
                            break;
                        }
                    }
                }
                else if (battleInput == '4') {
                    if (pc_team.poke.size() >= 4) {
                        if (pc_team.poke[3].ko == false) {
                            swap(pc_team.poke[0], pc_team.poke[3]);
                            break;
                        }
                    }
                }
                else if (battleInput == '5') {
                    if (pc_team.poke.size() >= 5) {
                        if (pc_team.poke[4].ko == false) {
                            swap(pc_team.poke[0], pc_team.poke[4]);
                            break;
                        }
                    }
                }
                else if (battleInput == '6') {
                    if (pc_team.poke.size() >= 6) {
                        if (pc_team.poke[5].ko == false) {
                            swap(pc_team.poke[0], pc_team.poke[5]);
                            break;
                        }
                    }
                }
            } while (battleInput != '1' || battleInput != '2' || battleInput != '3' || battleInput != '4' || battleInput != '5' || battleInput != '6');
        }
        else if (battleInput == 'l') {
            // Run
            battleEnd = true;
        }
    }

    wrefresh(battle);
    refresh();
}

void movePC(int xCoord, int yCoord, int newX, int newY, int numTrainers, WINDOW *win) {
    if (world[yCoord][xCoord].player.cost[newY][newX] != INF && world[yCoord][xCoord].map[newY][newX] != HIKER && world[yCoord][xCoord].map[newY][newX] != RIVAL
     && world[yCoord][xCoord].map[newY][newX] != PACER && world[yCoord][xCoord].map[newY][newX] != WANDERER && world[yCoord][xCoord].map[newY][newX] != EXPLORER
     && world[yCoord][xCoord].map[newY][newX] != SENTRY) {

        world[yCoord][xCoord].player.nextSpace = world[yCoord][xCoord].map[newY][newX];
        world[yCoord][xCoord].map[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x] = world[yCoord][xCoord].player.currSpace;
        world[yCoord][xCoord].player.coords.y = newY;
        world[yCoord][xCoord].player.coords.x = newX;
        if (world[yCoord][xCoord].player.nextSpace != '@') {
            world[yCoord][xCoord].player.currSpace = world[yCoord][xCoord].player.nextSpace;
        }
        world[yCoord][xCoord].player.currCost = world[yCoord][xCoord].player.cost[newY][newX];
        world[yCoord][xCoord].map[newY][newX] = PLAYER;

        if (world[yCoord][xCoord].hiker.defeated == 0) {
            dijkstra_pathing(world[yCoord][xCoord].player.coords.x, world[yCoord][xCoord].player.coords.y, xCoord, yCoord, HIKER);
        }

        if (world[yCoord][xCoord].rival.defeated == 0) {
            dijkstra_pathing(world[yCoord][xCoord].player.coords.x, world[yCoord][xCoord].player.coords.y, xCoord, yCoord, RIVAL);
        }

        if ((world[yCoord][xCoord].player.currSpace == TGRASS) && (rand() % 10 == 0)) {
            WINDOW *battle = newwin(ROWS, COLS, 1, 0);
            pokemonBattle(xCoord, yCoord, battle, win);
            delwin(battle);

            displayMap(xCoord, yCoord, win);
            mvwprintw(win, 0, 0, "                                                                                ");
        }
    }
    else if (world[yCoord][xCoord].map[newY][newX] == ROCK) {
        mvwprintw(win, 0, 0, "                                                                                ");
        mvwprintw(win, 0, 0, "A boulder blocks the path!");
    }
    else if (world[yCoord][xCoord].map[newY][newX] == FOREST) {
        mvwprintw(win, 0, 0, "                                                                                ");
        mvwprintw(win, 0, 0, "Here lies a large tree!");
    }
    else if (world[yCoord][xCoord].map[newY][newX] == WATER) {
        mvwprintw(win, 0, 0, "                                                                                ");
        mvwprintw(win, 0, 0, "A body of water expands in front of you!");
    }
    else if (world[yCoord][xCoord].map[newY][newX] == HIKER) {
        mvwprintw(win, 0, 0, "                                                                                ");
        mvwprintw(win, 0, 0, "Battle commence!");
        wrefresh(win);

        WINDOW *battle = newwin(ROWS, COLS, 1, 0);
        if (world[yCoord][xCoord].hiker.defeated == 0) {
            trainerBattle(xCoord, yCoord, -1, battle, win);
            delwin(battle);
            world[yCoord][xCoord].hiker.defeated = 1;
        }

        displayMap(xCoord, yCoord, win);
        mvwprintw(win, 0, 0, "                                                                                ");
    }
    else if (world[yCoord][xCoord].map[newY][newX] == RIVAL) {
        mvwprintw(win, 0, 0, "                                                                                ");
        mvwprintw(win, 0, 0, "Battle commence!");
        wrefresh(win);

        WINDOW *battle = newwin(ROWS, COLS, 1, 0);
        if (world[yCoord][xCoord].rival.defeated == 0) {
            trainerBattle(xCoord, yCoord, -2, battle, win);
            delwin(battle);
            world[yCoord][xCoord].rival.defeated = 1;
        }

        displayMap(xCoord, yCoord, win);
        mvwprintw(win, 0, 0, "                                                                                ");
    }
    else if (world[yCoord][xCoord].map[newY][newX] == SENTRY || world[yCoord][xCoord].map[newY][newX] == PACER
          || world[yCoord][xCoord].map[newY][newX] == WANDERER || world[yCoord][xCoord].map[newY][newX] == EXPLORER) {
        
        mvwprintw(win, 0, 0, "                                                                                ");
        mvwprintw(win, 0, 0, "Battle commence!");
        wrefresh(win);

        WINDOW *battle = newwin(ROWS, COLS, 1, 0);
        for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
            if (newY == world[yCoord][xCoord].npc[i].coords.y && newX == world[yCoord][xCoord].npc[i].coords.x) {
                if (world[yCoord][xCoord].npc[i].defeated == 0) {
                    trainerBattle(xCoord, yCoord, i, battle, win);
                    delwin(battle);
                    world[yCoord][xCoord].npc[i].defeated = 1;
                }
            }
        }

        displayMap(xCoord, yCoord, win);
        mvwprintw(win, 0, 0, "                                                                                ");
    }

    wrefresh(win);
    refresh();
}

void trainerList(int xCoord, int yCoord, int numTrainers, WINDOW *trainers) {
    int listInput;
    int startingRow = 1;
    int nextLineIndex = 1;
    int pc_x = world[yCoord][xCoord].player.coords.x;
    int pc_y = world[yCoord][xCoord].player.coords.y;

    box(trainers, 0, 0);

    if (numTrainers > 1) {
        world[yCoord][xCoord].hiker.coords.trainerX = world[yCoord][xCoord].hiker.coords.x;
        if (world[yCoord][xCoord].hiker.coords.trainerX > pc_x) {
            world[yCoord][xCoord].hiker.coords.distanceX = world[yCoord][xCoord].hiker.coords.trainerX - pc_x;
            world[yCoord][xCoord].hiker.coords.setDirectionX("east");
        }
        else {
            world[yCoord][xCoord].hiker.coords.distanceX = pc_x - world[yCoord][xCoord].hiker.coords.trainerX;
            world[yCoord][xCoord].hiker.coords.setDirectionX("west");
        }
        world[yCoord][xCoord].hiker.coords.trainerY = world[yCoord][xCoord].hiker.coords.y;
        if (world[yCoord][xCoord].hiker.coords.trainerY > pc_y) {
            world[yCoord][xCoord].hiker.coords.distanceY = world[yCoord][xCoord].hiker.coords.trainerY - pc_y;
            world[yCoord][xCoord].hiker.coords.setDirectionY("south");
        }
        else {
            world[yCoord][xCoord].hiker.coords.distanceY = pc_y - world[yCoord][xCoord].hiker.coords.trainerY;
            world[yCoord][xCoord].hiker.coords.setDirectionY("north");
        }
        mvwprintw(trainers, nextLineIndex, 2, "h, %d %s and %d %s   ", 
        world[yCoord][xCoord].hiker.coords.distanceY, world[yCoord][xCoord].hiker.coords.getDirectionY(), world[yCoord][xCoord].hiker.coords.distanceX, world[yCoord][xCoord].hiker.coords.getDirectionX());
        nextLineIndex += 1;
    }
    world[yCoord][xCoord].hiker.screenPos = 0;

    if (numTrainers > 0) {
        world[yCoord][xCoord].rival.coords.trainerX = world[yCoord][xCoord].rival.coords.x;
        if (world[yCoord][xCoord].rival.coords.trainerX > pc_x) {
            world[yCoord][xCoord].rival.coords.distanceX = world[yCoord][xCoord].rival.coords.trainerX - pc_x;
            world[yCoord][xCoord].rival.coords.setDirectionX("east");
        }
        else {
            world[yCoord][xCoord].rival.coords.distanceX = pc_x - world[yCoord][xCoord].rival.coords.trainerX;
            world[yCoord][xCoord].rival.coords.setDirectionX("west");
        }
        world[yCoord][xCoord].rival.coords.trainerY = world[yCoord][xCoord].rival.coords.y;
        if (world[yCoord][xCoord].rival.coords.trainerY > pc_y) {
            world[yCoord][xCoord].rival.coords.distanceY = world[yCoord][xCoord].rival.coords.trainerY - pc_y;
            world[yCoord][xCoord].rival.coords.setDirectionY("south");
        }
        else {
            world[yCoord][xCoord].rival.coords.distanceY = pc_y - world[yCoord][xCoord].rival.coords.trainerY;
            world[yCoord][xCoord].rival.coords.setDirectionY("north");
        }
        mvwprintw(trainers, nextLineIndex, 2, "r, %d %s and %d %s   ", 
        world[yCoord][xCoord].rival.coords.distanceY, world[yCoord][xCoord].rival.coords.getDirectionY(), world[yCoord][xCoord].rival.coords.distanceX, world[yCoord][xCoord].rival.coords.getDirectionX());
        nextLineIndex += 1;
    }
    world[yCoord][xCoord].rival.screenPos = 1;

    for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
        world[yCoord][xCoord].npc[i].coords.trainerX = world[yCoord][xCoord].npc[i].coords.x;
        if (world[yCoord][xCoord].npc[i].coords.trainerX > pc_x) {
            world[yCoord][xCoord].npc[i].coords.distanceX = world[yCoord][xCoord].npc[i].coords.trainerX - pc_x;
            world[yCoord][xCoord].npc[i].coords.setDirectionX("east");
        }
        else {
            world[yCoord][xCoord].npc[i].coords.distanceX = pc_x - world[yCoord][xCoord].npc[i].coords.trainerX;
            world[yCoord][xCoord].npc[i].coords.setDirectionX("west");
        }
        world[yCoord][xCoord].npc[i].coords.trainerY = world[yCoord][xCoord].npc[i].coords.y;
        if (world[yCoord][xCoord].npc[i].coords.trainerY > pc_y) {
            world[yCoord][xCoord].npc[i].coords.distanceY = world[yCoord][xCoord].npc[i].coords.trainerY - pc_y;
            world[yCoord][xCoord].npc[i].coords.setDirectionY("south");
        }
        else {
            world[yCoord][xCoord].npc[i].coords.distanceY = pc_y - world[yCoord][xCoord].npc[i].coords.trainerY;
            world[yCoord][xCoord].npc[i].coords.setDirectionY("north");
        }

        if (nextLineIndex <= 13) {
            mvwprintw(trainers, nextLineIndex, 2, "%c, %d %s and %d %s   ", world[yCoord][xCoord].npc[i].npcType, 
            world[yCoord][xCoord].npc[i].coords.distanceY, world[yCoord][xCoord].npc[i].coords.getDirectionY(), world[yCoord][xCoord].npc[i].coords.distanceX, world[yCoord][xCoord].npc[i].coords.getDirectionX());
            nextLineIndex += 1;
        }
        world[yCoord][xCoord].npc[i].screenPos = i + 2;
    }

    wrefresh(trainers);
    refresh();

    // +14
    nextLineIndex = 3;
    int j = 0;
    int endOfList = 0;
    while (listInput != 27) {
        listInput = getch();

        if (numTrainers > 13) {
            if (listInput == KEY_DOWN && endOfList == 0) {
                if (startingRow >= 1) {
                    startingRow++;

                    for (j = startingRow; j < world[yCoord][xCoord].npc.size(); j++) {
                        if (nextLineIndex <= 13) {
                            mvwprintw(trainers, nextLineIndex, 2, "%c, %d %s and %d %s   ", world[yCoord][xCoord].npc[j].npcType, 
                            world[yCoord][xCoord].npc[j].coords.distanceY, world[yCoord][xCoord].npc[j].coords.getDirectionY(), world[yCoord][xCoord].npc[j].coords.distanceX, world[yCoord][xCoord].npc[j].coords.getDirectionX());
                            nextLineIndex += 1;
                        }

                        if ((j + 1) == (world[yCoord][xCoord].npc.size())) {
                            endOfList = 1;
                        }

                        if (endOfList == 1) {
                            break;
                        }

                        if (nextLineIndex > 13) {
                            break;
                        }
                    }

                }  

                wrefresh(trainers);
            }
            else if (listInput == KEY_UP) {
                if (startingRow > 1) {
                    startingRow--;
                
                    for (j = startingRow; j < world[yCoord][xCoord].npc.size(); j++) {
                        if (nextLineIndex <= 13) {
                            mvwprintw(trainers, nextLineIndex, 2, "%c, %d %s and %d %s   ", world[yCoord][xCoord].npc[j].npcType, 
                            world[yCoord][xCoord].npc[j].coords.distanceY, world[yCoord][xCoord].npc[j].coords.getDirectionY(), world[yCoord][xCoord].npc[j].coords.distanceX, world[yCoord][xCoord].npc[j].coords.getDirectionX());
                            nextLineIndex += 1;
                        }

                        if (nextLineIndex > 13) {
                            break;
                        }
                    }

                }
                endOfList = 0;
                wrefresh(trainers);
            }
            
            nextLineIndex = 3;
        }
    }
}

void turnOrder(int xCoord, int yCoord, int numTrainers, int p_newX, int p_newY, WINDOW *win) {
    cost_t *c;
    heap_t h;

    heap_init(&h, cost_cmp, NULL);

    heap_insert(&h, (void*)&world[yCoord][xCoord].player.cost[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x]);

    if (numTrainers > 0) {
        heap_insert(&h, (void*)&world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x]);
    }

    if (numTrainers > 1) {
        heap_insert(&h, (void*)&world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x]);
    }

    if (numTrainers > 2) {
        for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
            heap_insert(&h, (void*)&world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[i].coords.y][world[yCoord][xCoord].npc[i].coords.x]);
        }
    }

    while (h.size > 0) {
        c = (cost_t *)heap_remove_min(&h);

        if (c == (void*)&world[yCoord][xCoord].player.cost[world[yCoord][xCoord].player.coords.y][world[yCoord][xCoord].player.coords.x]) {
            movePC(xCoord, yCoord, p_newX, p_newY, numTrainers, win);
        }
        else if (c == (void*)&world[yCoord][xCoord].rival.cost[world[yCoord][xCoord].rival.coords.y][world[yCoord][xCoord].rival.coords.x] && numTrainers > 0) {
            if (world[yCoord][xCoord].rival.defeated == 0) {    
                rivalTravel(xCoord, yCoord, numTrainers);
            }
            else {
                rival_defeatedTravel(xCoord, yCoord);
            }
        }
        else if (c == (void*)&world[yCoord][xCoord].hiker.cost[world[yCoord][xCoord].hiker.coords.y][world[yCoord][xCoord].hiker.coords.x] && numTrainers > 1) {
            if (world[yCoord][xCoord].hiker.defeated == 0) {    
                hikerTravel(xCoord, yCoord, numTrainers);
            }
            else {
                hiker_defeatedTravel(xCoord, yCoord);
            }
        }
        else {
            for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
                if (c == (void*)&world[yCoord][xCoord].generalCost[world[yCoord][xCoord].npc[i].coords.y][world[yCoord][xCoord].npc[i].coords.x]) {
                    if (world[yCoord][xCoord].npc[i].npcType == PACER) {
                        pacerTravel(xCoord, yCoord, i);
                    }
                    else if (world[yCoord][xCoord].npc[i].npcType == WANDERER) {
                        wandererTravel(xCoord, yCoord, i);
                    }
                    else if (world[yCoord][xCoord].npc[i].npcType == EXPLORER) {
                        explorerTravel(xCoord, yCoord, i);
                    }
                }
            }
        }
    }
    displayMap(xCoord, yCoord, win);
    heap_delete(&h);
}

void randMapVals(int *xIntersect, int *yIntersect, int xCoord, int yCoord) {
    gate[yCoord][xCoord].X1 = rand() % (78 - 3 + 1) + 3;
    gate[yCoord][xCoord].Y1 = rand() % (18 - 3 + 1) + 3;

    gate[yCoord][xCoord].X2 = rand() % (78 - 3 + 1) + 3;
    gate[yCoord][xCoord].Y2 = rand() % (18 - 3 + 1) + 3;

    *xIntersect = rand() % (76 - 5 + 1) + 5;
    *yIntersect = rand() % (16 - 5 + 1) + 5;
}

void setIntialDirections(int xCoord, int yCoord, int numTrainers) {
    if (numTrainers > 2) {
        for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
            if (world[yCoord][xCoord].npc[i].npcType == PACER) {
                world[yCoord][xCoord].npc[i].direction = rand() % 4;
            }
            else if (world[yCoord][xCoord].npc[i].npcType == SENTRY) {
                world[yCoord][xCoord].npc[i].direction = -1;
            }
            else {
                world[yCoord][xCoord].npc[i].direction = rand() % 8;
                if (world[yCoord][xCoord].npc[i].npcType == WANDERER) {
                    world[yCoord][xCoord].npc[i].homeTerrain = world[yCoord][xCoord].npc[i].currSpace;
                }
            }
        }
    }
}

void addTrainers(int xCoord, int yCoord, int numTrainers) {
    if (numTrainers > 2) {
        int index;
        for (int i = 0; i < numTrainers - 2; i++) {
            index = index % 4;
            non_pc temp;
            if (index == 0) {
                temp.npcType = PACER;
            }
            else if (index == 1) {
                temp.npcType = WANDERER;
            }
            else if (index == 2) {
                temp.npcType = EXPLORER;
            }
            else if (index == 3) {
                temp.npcType = SENTRY;
            }
            world[yCoord][xCoord].npc.push_back(temp);
            index++;
        }
    }
}

void initiateTrainerDetails(int xCoord, int yCoord, int index) {
    int k;
    if (index == -2) {
        int numPokemon = world[yCoord][xCoord].rival.poke.size();
        for (k = 0; k < numPokemon; k++) {
            // Get typing(s)
            world[yCoord][xCoord].rival.poke[k].typing = getTypings(world[yCoord][xCoord].rival.poke[k].id);
            world[yCoord][xCoord].rival.poke[k].shiny = isShiny();

            // Get moves
            world[yCoord][xCoord].rival.poke[k].moves = getMoves(world[yCoord][xCoord].rival.poke[k].species_id, world[yCoord][xCoord].rival.poke[k].lvl);

            // Get Stats
            world[yCoord][xCoord].rival.poke[k].ivs = getIV();
            world[yCoord][xCoord].rival.poke[k].stats = getStats(world[yCoord][xCoord].rival.poke[k].id, world[yCoord][xCoord].rival.poke[k].lvl, world[yCoord][xCoord].rival.poke[k].ivs);
            
            // Set HP
            world[yCoord][xCoord].rival.poke[k].currentHP = world[yCoord][xCoord].rival.poke[k].stats[0];
            world[yCoord][xCoord].rival.poke[k].ko = false;

            for (int i = 0; i < pokemonData.poke_spec.size(); i++) {
                if (world[yCoord][xCoord].rival.poke[k].id == pokemonData.poke_spec[i].id) {
                    world[yCoord][xCoord].rival.poke[k].growth_id = pokemonData.poke_spec[i].growth_rate_id;
                }
            }
            for (int i = 0; i < pokemonData.exp.size(); i++) {
                if (world[yCoord][xCoord].rival.poke[k].lvl == pokemonData.exp[i].level && world[yCoord][xCoord].rival.poke[k].growth_id == pokemonData.exp[i].growth_rate_id) {
                    world[yCoord][xCoord].rival.poke[k].currentEXP = pokemonData.exp[i].experience;
                }
            }
        }
    }
    else if (index == -1) {
        int numPokemon = world[yCoord][xCoord].hiker.poke.size();
        for (k = 0; k < numPokemon; k++) {
            // Get typing(s)
            world[yCoord][xCoord].hiker.poke[k].typing = getTypings(world[yCoord][xCoord].hiker.poke[k].id);
            world[yCoord][xCoord].hiker.poke[k].shiny = isShiny();

            // Get moves
            world[yCoord][xCoord].hiker.poke[k].moves = getMoves(world[yCoord][xCoord].hiker.poke[k].species_id, world[yCoord][xCoord].hiker.poke[k].lvl);

            // Get Stats
            world[yCoord][xCoord].hiker.poke[k].ivs = getIV();
            world[yCoord][xCoord].hiker.poke[k].stats = getStats(world[yCoord][xCoord].hiker.poke[k].id, world[yCoord][xCoord].hiker.poke[k].lvl, world[yCoord][xCoord].hiker.poke[k].ivs);
            
            // Set HP
            world[yCoord][xCoord].hiker.poke[k].currentHP = world[yCoord][xCoord].hiker.poke[k].stats[0];
            world[yCoord][xCoord].hiker.poke[k].ko = false;

            for (int i = 0; i < pokemonData.poke_spec.size(); i++) {
                if (world[yCoord][xCoord].hiker.poke[k].id == pokemonData.poke_spec[i].id) {
                    world[yCoord][xCoord].hiker.poke[k].growth_id = pokemonData.poke_spec[i].growth_rate_id;
                }
            }
            for (int i = 0; i < pokemonData.exp.size(); i++) {
                if (world[yCoord][xCoord].hiker.poke[k].lvl == pokemonData.exp[i].level && world[yCoord][xCoord].hiker.poke[k].growth_id == pokemonData.exp[i].growth_rate_id) {
                    world[yCoord][xCoord].hiker.poke[k].currentEXP = pokemonData.exp[i].experience;
                }
            }
        }
    }
    else if (index >= 0) {
        int numPokemon = world[yCoord][xCoord].npc[index].poke.size();
        for (k = 0; k < numPokemon; k++) {
            // Get typing(s)
            world[yCoord][xCoord].npc[index].poke[k].typing = getTypings(world[yCoord][xCoord].npc[index].poke[k].id);
            world[yCoord][xCoord].npc[index].poke[k].shiny = isShiny();

            // Get moves
            world[yCoord][xCoord].npc[index].poke[k].moves = getMoves(world[yCoord][xCoord].npc[index].poke[k].species_id, world[yCoord][xCoord].npc[index].poke[k].lvl);

            // Get Stats
            world[yCoord][xCoord].npc[index].poke[k].ivs = getIV();
            world[yCoord][xCoord].npc[index].poke[k].stats = getStats(world[yCoord][xCoord].npc[index].poke[k].id, world[yCoord][xCoord].npc[index].poke[k].lvl, world[yCoord][xCoord].npc[index].poke[k].ivs);
            
            // Set HP
            world[yCoord][xCoord].npc[index].poke[k].currentHP = world[yCoord][xCoord].npc[index].poke[k].stats[0];
            world[yCoord][xCoord].npc[index].poke[k].ko = false;

            for (int i = 0; i < pokemonData.poke_spec.size(); i++) {
                if (world[yCoord][xCoord].npc[index].poke[k].id == pokemonData.poke_spec[i].id) {
                    world[yCoord][xCoord].npc[index].poke[k].growth_id = pokemonData.poke_spec[i].growth_rate_id;
                }
            }
            for (int i = 0; i < pokemonData.exp.size(); i++) {
                if (world[yCoord][xCoord].npc[index].poke[k].lvl == pokemonData.exp[i].level && world[yCoord][xCoord].npc[index].poke[k].growth_id == pokemonData.exp[i].growth_rate_id) {
                    world[yCoord][xCoord].npc[index].poke[k].currentEXP = pokemonData.exp[i].experience;
                }
            }
        }
    }
}

void setTrainersPokemon(int xCoord, int yCoord, int numTrainers){
    int trainerPokemonIndex = 0;
    int pokemonChance = 0;
    int numRivalPokemon = 1, numHikerPokemon = 1, numNpcPokemon = 1;

    if (numTrainers > 0) {
        pokemon p;
        pokemonChance = (rand() % 5);
        while ((pokemonChance == 0 || pokemonChance == 2 || pokemonChance == 4) && numRivalPokemon < 6) {
            pokemonChance = (rand() % 5);
            numRivalPokemon++;
        }

        for (int i = 0; i < numRivalPokemon; i++) {
            trainerPokemonIndex = (rand() % 1092);
            p = pokemonData.poke[trainerPokemonIndex];
            world[yCoord][xCoord].rival.poke.push_back(p);
            world[yCoord][xCoord].rival.poke[i].index = trainerPokemonIndex;
            world[yCoord][xCoord].rival.poke[i].lvl = loadPokemonlvl(xCoord, yCoord);
        }
    }

    if (numTrainers > 1) {
        pokemon p;
        pokemonChance = (rand() % 5);
        while ((pokemonChance == 0 || pokemonChance == 2 || pokemonChance == 4) && numHikerPokemon < 6) {
            pokemonChance = (rand() % 5);
            numHikerPokemon++;
        }

        for (int i = 0; i < numHikerPokemon; i++) {
            trainerPokemonIndex = (rand() % 1092);
            p = pokemonData.poke[trainerPokemonIndex];
            world[yCoord][xCoord].hiker.poke.push_back(p);
            world[yCoord][xCoord].hiker.poke[i].index = trainerPokemonIndex;
            world[yCoord][xCoord].hiker.poke[i].lvl = loadPokemonlvl(xCoord, yCoord);
        }
    }

    if (numTrainers > 2) {
        pokemon p;
        // pokemonChance = (rand() % 5);
        // while ((pokemonChance == 0 || pokemonChance == 2 || pokemonChance == 4) && numNpcPokemon < 6) {
        //     pokemonChance = (rand() % 5);
        //     numNpcPokemon++;
        // }

        int j = 0;
        int i;
        for (i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
            trainerPokemonIndex = (rand() % 1092);
            p = pokemonData.poke[trainerPokemonIndex];
            world[yCoord][xCoord].npc[i].poke.push_back(p);
            world[yCoord][xCoord].npc[i].poke[j].index = trainerPokemonIndex;
            world[yCoord][xCoord].npc[i].poke[j].lvl = loadPokemonlvl(xCoord, yCoord);
        }

        // for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
        //     for (int j = 0; j < numNpcPokemon; j++) {
        //         trainerPokemonIndex = (rand() % 1092);
        //         p = pokemonData.poke[trainerPokemonIndex];
        //         world[yCoord][xCoord].npc[i].poke.push_back(p);
        //         world[yCoord][xCoord].npc[i].poke[j].index = trainerPokemonIndex;
        //         world[yCoord][xCoord].npc[i].poke[j].lvl = loadPokemonlvl(xCoord, yCoord);
        //     }
        // }
    }
}

void allocateMemory(int xCoord, int yCoord, int numTrainers) {
    //Allocate memory
    world[yCoord][xCoord].map = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].map[i] = (int*)malloc(COLS * sizeof(int*));
    }
    world[yCoord][xCoord].player.cost = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].player.cost[i] = (int*)malloc(COLS * sizeof(int*));
    }
    world[yCoord][xCoord].rival.cost = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].rival.cost[i] = (int*)malloc(COLS * sizeof(int*));
    }
    world[yCoord][xCoord].hiker.cost = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].hiker.cost[i] = (int*)malloc(COLS * sizeof(int*));
    }
    world[yCoord][xCoord].rival.pathCost = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].rival.pathCost[i] = (int*)malloc(COLS * sizeof(int*));
    }
    world[yCoord][xCoord].hiker.pathCost = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].hiker.pathCost[i] = (int*)malloc(COLS * sizeof(int*));
    }
    world[yCoord][xCoord].generalCost = (int**)malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) {
        world[yCoord][xCoord].generalCost[i] = (int*)malloc(COLS * sizeof(int*));
    }

    //world[yCoord][xCoord].npc = (npc*)malloc(numTrainers * sizeof(npc));


}

void generateMap(int xIntersect, int yIntersect, int xCoord, int yCoord, int numTrainers, WINDOW *win) { 
    allocateMemory(xCoord, yCoord, numTrainers);
    addTrainers(xCoord, yCoord, numTrainers);
    generateBorder(xCoord, yCoord);
    generateTerrain(xCoord, yCoord);
    checkGates(xCoord, yCoord);
    generateRoads(xIntersect, yIntersect, xCoord, yCoord);
    generateBuildings(xIntersect, yIntersect, xCoord, yCoord);
    if (numTrainers > 0) {
        setRivalCosts(xCoord, yCoord);
    }
    if (numTrainers > 1) {
        setHikerCosts(xCoord, yCoord);
    }
    setPlayerCosts(xCoord, yCoord);

    if ((xCoord == 200 && yCoord == 200 && world[yCoord][xCoord].visited == 0) || usrInput == 'f') {
        placeRandPC(xIntersect, yIntersect, xCoord, yCoord);
    }
    else {
        placePC(xCoord, yCoord);
    }
    world[yCoord][xCoord].visited = 1;

    dijkstra_pathing(world[yCoord][xCoord].player.coords.x, world[yCoord][xCoord].player.coords.y, xCoord, yCoord, HIKER);
    dijkstra_pathing(world[yCoord][xCoord].player.coords.x, world[yCoord][xCoord].player.coords.y, xCoord, yCoord, RIVAL);

    placeRandNPC(xCoord, yCoord, numTrainers);
    setIntialDirections(xCoord, yCoord, numTrainers);
    displayMap(xCoord, yCoord, win);

    setTrainersPokemon(xCoord, yCoord, numTrainers);
    
    initiateTrainerDetails(xCoord, yCoord, -2); // Rival
    initiateTrainerDetails(xCoord, yCoord, -1); // Hiker
    for (int i = 0; i < world[yCoord][xCoord].npc.size(); i++) {
        initiateTrainerDetails(xCoord, yCoord, i); // NPC x
    }
}

int main(int argc, char *argv[]) {

    srand(time(NULL));
    mt19937 rng(random_device{}());

    const string dataInput[] = {"pokemon", "moves", "pokemon_moves", "pokemon_species", "experience", "type_names", "pokemon_stats", "stats", "pokemon_types"};

    for (int k = 0; k < 9; k++) {
        ifstream iFile("csv_files/" + dataInput[k] + ".csv");
        if (!iFile) {
            iFile.close();
            cerr << "Error opening csv file!" << endl;
            return 1;
        }

        string firstLine;
        getline(iFile, firstLine);
        string line;

        if (dataInput[k] == "pokemon") {
            while (!iFile.eof()) {
                pokemon p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.id = INT_MAX; 
                }
                else {
                    p.id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.identifier = "";
                }
                else {
                    p.identifier = line;
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.species_id = INT_MAX;
                }
                else {
                    p.species_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.height = INT_MAX;
                }
                else {
                    p.height = stoi(line);
                }
                
                getline(iFile, line, ',');
                if (line == "") {
                    p.weight = INT_MAX;
                }
                else {
                    p.weight = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.base_experience = INT_MAX;
                }
                else {
                    p.base_experience = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.order = INT_MAX;
                }
                else {
                    p.order = stoi(line);
                }

                getline(iFile, line);
                if (line == "") {
                    p.is_default = INT_MAX;
                }
                else {
                    p.is_default = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.poke.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "moves") {
            while (!iFile.eof()) {
                moves p;

                getline(iFile, line, ',');
                if (line == "") { 
                    p.id = INT_MAX; 
                }
                else {
                    p.id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.identifier = "";
                }
                else {
                    p.identifier = line;
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.generation_id = INT_MAX;
                }
                else {
                    p.generation_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.type_id = INT_MAX;
                }
                else {
                    p.type_id = stoi(line);
                }
                
                getline(iFile, line, ',');
                if (line == "") {
                    p.power = INT_MAX;
                }
                else {
                    p.power = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.pp = INT_MAX;
                }
                else {
                    p.pp = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.accuracy = INT_MAX;
                }
                else {
                    p.accuracy = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.priority = INT_MAX;
                }
                else {
                    p.priority = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.target_id = INT_MAX;
                }
                else {
                    p.target_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.damage_class_id = INT_MAX;
                }
                else {
                    p.damage_class_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.effect_id = INT_MAX;
                }
                else {
                    p.effect_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.effect_chance = INT_MAX;
                }
                else {
                    p.effect_chance = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.contest_type_id = INT_MAX;
                }
                else {
                    p.contest_type_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.contest_effect_id = INT_MAX;
                }
                else {
                    p.contest_effect_id = stoi(line);
                }

                getline(iFile, line);
                if (line == "") {
                    p.super_contest_effect_id = INT_MAX;
                }
                else {
                    p.super_contest_effect_id = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.move.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "pokemon_moves") {
            while (!iFile.eof()) {
                pokemon_moves p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.pokemon_id = INT_MAX; 
                }
                else {
                    p.pokemon_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.version_group_id = INT_MAX;
                }
                else {
                    p.version_group_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.move_id = INT_MAX;
                }
                else {
                    p.move_id = stoi(line);
                }
                
                getline(iFile, line, ',');
                if (line == "") {
                    p.pokemon_move_method_id = INT_MAX;
                }
                else {
                    p.pokemon_move_method_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.level = INT_MAX;
                }
                else {
                    p.level = stoi(line);
                }

                getline(iFile, line);
                if (line == "") {
                    p.order = INT_MAX;
                }
                else {
                    p.order = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.poke_move.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "pokemon_species") {
            while (!iFile.eof()) {
                pokemon_species p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.id = INT_MAX; 
                }
                else {
                    p.id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.identifier = "";
                }
                else {
                    p.identifier = line;
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.generation_id = INT_MAX;
                }
                else {
                    p.generation_id = stoi(line);
                }
                
                getline(iFile, line, ',');
                if (line == "") {
                    p.evolves_from_species_id = INT_MAX;
                }
                else {
                    p.evolves_from_species_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.evolution_chain_id = INT_MAX;
                }
                else {
                    p.evolution_chain_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.color_id = INT_MAX;
                }
                else {
                    p.color_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.shape_id = INT_MAX;
                }
                else {
                    p.shape_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.habitat_id = INT_MAX;
                }
                else {
                    p.habitat_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.gender_rate = INT_MAX;
                }
                else {
                    p.gender_rate = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.capture_rate = INT_MAX;
                }
                else {
                    p.capture_rate = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.base_happiness = INT_MAX;
                }
                else {
                    p.base_happiness = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.is_baby = INT_MAX;
                }
                else {
                    p.is_baby = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.hatch_counter = INT_MAX;
                }
                else {
                    p.hatch_counter = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.has_gender_differences = INT_MAX;
                }
                else {
                    p.has_gender_differences = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.growth_rate_id = INT_MAX;
                }
                else {
                    p.growth_rate_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.forms_switchable = INT_MAX;
                }
                else {
                    p.forms_switchable = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.is_legendary = INT_MAX;
                }
                else {
                    p.is_legendary = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.is_mythical = INT_MAX;
                }
                else {
                    p.is_mythical = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.order = INT_MAX;
                }
                else {
                    p.order = stoi(line);
                }

                getline(iFile, line);
                if (line == "") {
                    p.conquest_order = INT_MAX;
                }
                else {
                    p.conquest_order = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.poke_spec.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "experience") {
            while (!iFile.eof()) {
                experience p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.growth_rate_id = INT_MAX; 
                }
                else {
                    p.growth_rate_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.level = INT_MAX;
                }
                else {
                    p.level = stoi(line);
                }
                
                getline(iFile, line);
                if (line == "") {
                    p.experience = INT_MAX;
                }
                else {
                    p.experience = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.exp.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "type_names") {
            while (!iFile.eof()) {
                type_names p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.type_id = INT_MAX; 
                }
                else {
                    p.type_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.local_language_id = INT_MAX;
                }
                else {
                    p.local_language_id = stoi(line);
                }
                
                getline(iFile, line);
                if (line == "") {
                    p.name = "";
                }
                else {
                    p.name = line;
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.type_name.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "pokemon_stats") {
            while (!iFile.eof()) {
                pokemon_stats p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.pokemon_id = INT_MAX; 
                }
                else {
                    p.pokemon_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.stat_id = INT_MAX;
                }
                else {
                    p.stat_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.base_stat = INT_MAX;
                }
                else {
                    p.base_stat = stoi(line);
                }
                
                getline(iFile, line);
                if (line == "") {
                    p.effort = INT_MAX;
                }
                else {
                    p.effort = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.poke_stat.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "stats") {
            while (!iFile.eof()) {
                stats p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.id = INT_MAX; 
                }
                else {
                    p.id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.damage_class_id = INT_MAX;
                }
                else {
                    p.damage_class_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.identifier = "";
                }
                else {
                    p.identifier = line;
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.is_battle_only = INT_MAX;
                }
                else {
                    p.is_battle_only = stoi(line);
                }
                
                getline(iFile, line);
                if (line == "") {
                    p.game_index = INT_MAX;
                }
                else {
                    p.game_index = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.stat.push_back(p);
            }

            iFile.close();

        }
        else if (dataInput[k] == "pokemon_types") {
            while (!iFile.eof()) {
                pokemon_types p;
                
                getline(iFile, line, ',');
                if (line == "") { 
                    p.pokemon_id = INT_MAX; 
                }
                else {
                    p.pokemon_id = stoi(line);
                }

                getline(iFile, line, ',');
                if (line == "") {
                    p.type_id = INT_MAX;
                }
                else {
                    p.type_id = stoi(line);
                }

                getline(iFile, line);
                if (line == "") {
                    p.slot = INT_MAX;
                }
                else {
                    p.slot = stoi(line);
                }

                if (iFile.peek() == '\n') {
                    break;
                }

                pokemonData.poke_type.push_back(p);
            }

            iFile.close();
        }
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    
    WINDOW *win = newwin(ROWS + 3, COLS, 0, 0);
    wrefresh(win);
    refresh();

    int xIntersect;
    int yIntersect;
    int xCoord = 200;
    int yCoord = 200;

    int numTrainers;
    if (argc > 1 && strcmp(argv[1], "--numtrainers") == 0) {
        numTrainers = atoi(argv[2]);
    }
    else {
        numTrainers = 10;
    }
    randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
    generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);

    mvwprintw(win, 0, 0, "                                                                                ");
    mvwprintw(win, 0, 0, "Starter Selection");
    wrefresh(win);

    WINDOW *starter = newwin(ROWS - 4, COLS - 40, 3, 20);
    pickStarter(starter);
    delwin(starter);
    displayMap(xCoord, yCoord, win);
    mvwprintw(win, 0, 0, "                                                                                ");


    mvwprintw(win, 0, 0, "                                                                                ");
    mvwprintw(win, 0, 0, "View Your Starting team");
    wrefresh(win);

    WINDOW *team = newwin(ROWS - 4, COLS - 10, 3, 5);
    initiatePlayerTeam();
    viewTeam(team);
    delwin(team);
    displayMap(xCoord, yCoord, win);
    mvwprintw(win, 0, 0, "                                                                                ");

    replenishItems();

    usrInput = getch();

    while(usrInput != 'Q') {
        if (usrInput == 'b' || usrInput == '1') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x - 1;
            int p_newY = world[yCoord][xCoord].player.coords.y + 1;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newX == gate[yCoord][xCoord].X2 && p_newY == (ROWS - 1)) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                yCoord--;
                world[yCoord][xCoord].player.spawnGate = 'n';
                
                //Checks if moving south is outside world border
                if (yCoord < 0) {
                    yCoord++;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else if(p_newY == gate[yCoord][xCoord].Y1 && p_newX == 0) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                xCoord--;
                world[yCoord][xCoord].player.spawnGate = 'e';
                //Checks if moving west is outside world border
                if (xCoord < 0) {
                    xCoord++;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'n' || usrInput == '3') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x + 1;
            int p_newY = world[yCoord][xCoord].player.coords.y + 1;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newX == gate[yCoord][xCoord].X2 && p_newY == (ROWS - 1)) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                yCoord--;
                world[yCoord][xCoord].player.spawnGate = 'n';
                //Checks if moving south is outside world border
                if (yCoord < 0) {
                    yCoord++;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else if (p_newY == gate[yCoord][xCoord].Y2 && p_newX == (COLS - 1)) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                xCoord++;
                world[yCoord][xCoord].player.spawnGate = 'w';
                //Checks if moving east is outide world border
                if (xCoord > 400) {
                    xCoord--;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'h' || usrInput == '4') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x - 1;
            int p_newY = world[yCoord][xCoord].player.coords.y;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if(p_newY == gate[yCoord][xCoord].Y1 && p_newX == 0) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                xCoord--;
                world[yCoord][xCoord].player.spawnGate = 'e';
                //Checks if moving west is outside world border
                if (xCoord < 0) {
                    xCoord++;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'k' || usrInput == '8') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x;
            int p_newY = world[yCoord][xCoord].player.coords.y - 1;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newX == gate[yCoord][xCoord].X1 && p_newY == 0) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                yCoord++;
                world[yCoord][xCoord].player.spawnGate = 's';
                //Checks if moving north is outside world border
                if (yCoord > 400) {
                    yCoord--;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'j' || usrInput == '2') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x;
            int p_newY = world[yCoord][xCoord].player.coords.y + 1;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newX == gate[yCoord][xCoord].X2 && p_newY == (ROWS - 1)) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                yCoord--;
                world[yCoord][xCoord].player.spawnGate = 'n';
                //Checks if moving south is outside world border
                if (yCoord < 0) {
                    yCoord++;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'l' || usrInput == '6') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x + 1;
            int p_newY = world[yCoord][xCoord].player.coords.y;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newY == gate[yCoord][xCoord].Y2 && p_newX == (COLS - 1)) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                xCoord++;
                world[yCoord][xCoord].player.spawnGate = 'w';
                //Checks if moving east is outide world border
                if (xCoord > 400) {
                    xCoord--;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'y' || usrInput == '7') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x - 1;
            int p_newY = world[yCoord][xCoord].player.coords.y - 1;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newX == gate[yCoord][xCoord].X1 && p_newY == 0) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                yCoord++;
                world[yCoord][xCoord].player.spawnGate = 's';
                //Checks if moving north is outside world border
                if (yCoord > 400) {
                    yCoord--;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else if(p_newY == gate[yCoord][xCoord].Y1 && p_newX == 0) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                xCoord--;
                world[yCoord][xCoord].player.spawnGate = 'e';
                //Checks if moving west is outside world border
                if (xCoord < 0) {
                    xCoord++;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'u' || usrInput == '9') {
            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            int p_newX = world[yCoord][xCoord].player.coords.x + 1;
            int p_newY = world[yCoord][xCoord].player.coords.y - 1;
            
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            if (p_newX == gate[yCoord][xCoord].X1 && p_newY == 0) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                yCoord++;
                world[yCoord][xCoord].player.spawnGate = 's';
                //Checks if moving north is outside world border
                if (yCoord > 400) {
                    yCoord--;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else if (p_newY == gate[yCoord][xCoord].Y2 && p_newX == (COLS - 1)) {
                world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
                xCoord++;
                world[yCoord][xCoord].player.spawnGate = 'w';
                //Checks if moving east is outide world border
                if (xCoord > 400) {
                    xCoord--;
                    world[yCoord][xCoord].map[p_oldY][p_oldX] = PLAYER;
                }
            }
            else {
                turnOrder(xCoord, yCoord, numTrainers, p_newX, p_newY, win);
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placePC(xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == 'f') {
            mvwprintw(win, 0, 0, "                                                                                ");
            wrefresh(win);
            WINDOW *fly = newwin(3, 30, 3, 25);
            openFly(xCoord, yCoord, fly);
            delwin(fly);

            int p_oldX = world[yCoord][xCoord].player.coords.x;
            int p_oldY = world[yCoord][xCoord].player.coords.y;
            world[yCoord][xCoord].map[p_oldY][p_oldX] = world[yCoord][xCoord].player.currSpace;
            usrX = usrX + 200;
            usrY = usrY + 200;
            int tmpX = xCoord;
            int tmpY = yCoord;
            xCoord = usrX;
            yCoord = usrY;
            if (xCoord > 400 || xCoord < 0 || yCoord > 400 || yCoord < 0) {
                xCoord = tmpX;
                yCoord = tmpY;
            }
            if (world[yCoord][xCoord].visited != 1) {
                randMapVals(&xIntersect, &yIntersect, xCoord, yCoord);
                generateMap(xIntersect, yIntersect, xCoord, yCoord, numTrainers, win);
            }
            else {
                placeRandPC(xIntersect, yIntersect, xCoord, yCoord);
                displayMap(xCoord, yCoord, win);
            }
        }
        else if (usrInput == '>') {
            if (world[yCoord][xCoord].player.currSpace == CENTER) {
                mvwprintw(win, 0, 0, "                                                                                ");
                mvwprintw(win, 0, 0, "Welcome to the Pokemon Center!");
                wrefresh(win);
                
                WINDOW *center = newwin(ROWS - 4, COLS - 10, 3, 5);
                openCenter(xCoord, yCoord, center);
                delwin(center);

                displayMap(xCoord, yCoord, win);
                mvwprintw(win, 0, 0, "                                                                                ");
            }
            else if (world[yCoord][xCoord].player.currSpace == MART) {
                mvwprintw(win, 0, 0, "                                                                                ");
                mvwprintw(win, 0, 0, "Welcome to the Pokemon Mart!");
                wrefresh(win);
                
                WINDOW *mart = newwin(ROWS - 4, COLS - 10, 3, 5);
                openMart(xCoord, yCoord, mart);
                delwin(mart);

                displayMap(xCoord, yCoord, win);
                mvwprintw(win, 0, 0, "                                                                                ");
            }
        }
        else if (usrInput == 32 || usrInput == '5' || usrInput == '.') { //space key
            turnOrder(xCoord, yCoord, numTrainers, world[yCoord][xCoord].player.coords.x, world[yCoord][xCoord].player.coords.y, win);
            displayMap(xCoord, yCoord, win);
        }
        else if (usrInput == 't') {
            mvwprintw(win, 0, 0, "                                                                                ");
            mvwprintw(win, 0, 0, "Trainer list:");
            wrefresh(win);

            WINDOW *trainers = newwin(ROWS - 6, COLS - 50, 4, 25);
            trainerList(xCoord, yCoord, numTrainers, trainers);
            delwin(trainers);

            mvwprintw(win, 0, 0, "                                                                                ");
            displayMap(xCoord, yCoord, win);
        }

        usrInput = getch();
    }

    for (int y = 0; y < WORLD; y++) {
        for (int x = 0; x < WORLD; x++) {
            if (world[y][x].visited == 1) {
                // Free memory for hiker.cost
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].hiker.cost[i]);
                }
                free(world[y][x].hiker.cost);

                // Free memory for hiker.pathCost
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].hiker.pathCost[i]);
                }
                free(world[y][x].hiker.pathCost);

                // Free memory for rival.cost
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].rival.cost[i]);
                }
                free(world[y][x].rival.cost);

                // Free memory for rival.pathCost
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].rival.pathCost[i]);
                }
                free(world[y][x].rival.pathCost);

                // Free memory for player.cost
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].player.cost[i]);
                }
                free(world[y][x].player.cost);

                // Free memory for generalCost
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].generalCost[i]);
                }
                free(world[y][x].generalCost);

                //free(world[y][x].npc);

                // Free memory for map
                for (int i = 0; i < ROWS; i++) {
                    free(world[y][x].map[i]);
                }
                free(world[y][x].map);
            }
        }
    }

    delwin(win);
    endwin();
    return 0;
}