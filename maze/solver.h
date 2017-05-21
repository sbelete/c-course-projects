#include "common.h"

void create_room_connections(struct maze_room *room, unsigned int hex);

int dfs(int row, int col, int goal_row, int goal_col, 
	int num_rows, int num_cols, struct maze_room maze[], FILE *file);
