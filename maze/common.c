#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
/*
 * Gets the index of a one-dimensional array that corresponds to the given row 
 * and column in a two-dimensional grid. This is used to convert from a two-dimensional
 * coordinate to a one-dimensional array index. If you are using two-dimensional arrays,
 * you do not need this function - feel free to comment it out.
 *
 * Parameters:
 *	- row: row of the room being queried
 *	- col: column of the room being queried
 *	- num_cols: number of columns in the maze
 * 
 * Returns:
 *	- the index in the one dimensional array that corresponds to the given 
 *	  row and column
 */
int get_index(int row, int col, int num_cols) {
	return (row * num_cols) + col;
}

/*
 * Determines whether or not the room at [row][col] is a valid room within the maze with
 * dimensions num_rows x num_cols
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 * 
 * Returns:
 *	- 0 if room is not in the maze, 1 if room is in the maze
 */
int is_in_range(int row, int col, int num_rows, int num_cols) {
	return row < num_rows && row >= 0 && col < num_cols && col >= 0;
}

/*
 * Tells if the index is in the right range.
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *
 * Returns:
 *  - 0 if room is not in the maze, 1 if room is in the maze
 */
int direction_index(int row, int col, Direction dir, 
	int num_rows, int num_cols) {
	switch (dir) {
		case 0: return is_in_range(row - 1, col, num_rows, num_cols);
			break;  // This checks the neighbor in the north
		case 1: return is_in_range(row + 1, col, num_rows, num_cols);
			break;  // This checks the neighbor in the south
		case 2: return is_in_range(row , col - 1, num_rows, num_cols);
			break;  // This checks the neighbor in the west
		case 3: return is_in_range(row , col + 1, num_rows, num_cols);
			break;  // This checks the neighbor in the east
		default:
			printf("Direction given was invalid (direction_index)");
			return 0;  // This should never happen
			break;
	}
}

/*
 * Given a pointer to the room and a Direction to travel in, return a pointer to the room
 * that neighbors the current room on the given Direction.
 * For example: get_neighbor(&maze[3][4], EAST) should return &maze[3][5]
 *
 * Parameters:
 *	- maze: an array of maze_room structs representing your maze
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- room: pointer to the current room
 *	- dir: Direction to get the neighbor from
 *
 * Returns:
 *	- pointer to the neighboring room
 */
struct maze_room *get_neighbor(struct maze_room maze[], int num_rows,
				 int num_cols, struct maze_room *room, Direction dir) {
	assert(is_in_range(room->r, room->c, num_rows, num_cols));
	// Makes sure the room is in the proper range

	switch(dir) {
		case 0: return &maze[get_index(room->r - 1, room->c, num_cols)];
			break;  // Gets the north neighbor
		case 1: return &maze[get_index(room->r + 1, room->c, num_cols)];
			break;  // Gets the south neighbor
		case 2: return &maze[get_index(room->r, room->c - 1, num_cols)];
			break;  // Gets the west neighbor
		case 3: return &maze[get_index(room->r, room->c + 1, num_cols)];
			break;  // Gets the east neighbor
		default:
			printf("Direction given was invalid (get_neighbor)");
			return 0;  // Should never happen because of the assert
			break;
	}
}

