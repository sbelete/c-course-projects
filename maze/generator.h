#include "common.h"

Direction get_opposite_dir(Direction dir);

void shuffle_directions(Direction directions[]);

void drunken_walk(struct maze_room maze[], int row, int col,
        int num_rows, int num_cols);

int encode_room(struct maze_room room);

void encode_maze(struct maze_room maze[], int num_rows, int num_cols,
        int result[]);
