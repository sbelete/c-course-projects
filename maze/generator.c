#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "generator.h"


/*
 * Given a direction, get its opposite
 *
 * Parameters:
 *	- dir: direction
 *
 * Returns:
 *	- the opposite direction to dir
 */
Direction get_opposite_dir(Direction dir) {
	switch (dir) {
		case 0: return SOUTH;
			break;
		case 1: return NORTH;
			break;
		case 2: return EAST;
			break;
		case 3: return WEST;
			break;
		default:
			printf("Direction given was invalid (get_opposite_dir)");
			return 0;
			break;
		}
}


/*
 * Given an array of the four directions, randomizes the order of the directions
 *
 * Parameters:
 *	- directions: an array of Direction enums of size 4
 *
 * Returns:
 *	- nothing - the array should be shuffled in place
 */
void shuffle_array(Direction directions[]) {
	for(int i = 0; i < 4; i++) {
		int random = rand() % 4;
		int temp = directions[i];

		directions[i] = directions[random];
		directions[random] = temp;
	}
}

/*
 * Recursive function for stepping through a maze and setting its connections/walls using
 * the drunken_walk algorithm
 *
 * Parameters:
 *	- maze: an array of maze_room structs representing your maze
 * 	- row: row of the current room
 *	- col: column of the current room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *
 * Returns:
 *	- nothing - the state of your maze should be saved in the maze array being passed
 *	  in (make sure to use pointers correctly!).
 */
void drunken_walk(struct maze_room maze[], int row, int col,
        int num_rows, int num_cols) {
	struct maze_room *room = &maze[get_index(row, col, num_cols)];

	room->visited = 1;  // Shows that the maze has been visited
	room->r = row;  // Show the row of the room
	room->c = col;  // Show the column of the room
	room->next = '\0';  // Set the next room to null

	Direction dir[4] = {NORTH, SOUTH, WEST, EAST};
	Direction *d = dir;

	shuffle_array(d);  // Shuffle up the four directions

	for(int i = 0; i < 4; i++) {
		if(direction_index(row, col, dir[i], num_rows, num_cols) == 0) {
			room->connections[dir[i]] = 1;  // Make a connection
		} else {
			struct maze_room *neighbor = 
				get_neighbor(maze, num_rows, num_cols, room, dir[i]);
			if(neighbor->visited == 0) {
				room->connections[dir[i]] = 0;  // Set no connection

				switch(dir[i]) {
					case 0: neighbor->c = col; neighbor->r = row -1; break;
					case 1: neighbor->c = col; neighbor->r = row +1; break;
					case 2: neighbor->r = row; neighbor->c = col -1; break;
					case 3: neighbor->r = row; neighbor->c = col+1; break;
					default: break;
				}

				drunken_walk(maze, neighbor->r, neighbor->c,
							 num_rows, num_cols);  // Recursive call
 			} else {
				Direction opposite_dir = get_opposite_dir(dir[i]);

				if(neighbor->connections[opposite_dir] == 1 || 
					neighbor->connections[opposite_dir] == 0)
					room->connections[dir[i]] = 
						neighbor->connections[opposite_dir];

				else
					room->connections[dir[i]] = 1;  // Make a connection
			}
		}
	}
}

/*
 * Represents a maze_room struct as a hexadecimal number based on its connections
 *
 * Parameters:
 *	- room: a struct maze_room to be converted to a hexadecimal number
 *
 * Returns:
 *	- the hexadecimal representation of room
 */
int encode_room(struct maze_room room) {
	return (room.connections[0] * 1) + (room.connections[1] * 2) + 
			(room.connections[2] * 4) + (room.connections[3] * 8);
}

/*
 * Represents a maze_room array as a hexadecimal array based on its connections
 *
 * Parameters:
 *	- maze: an array of maze_room structs representing the maze
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- result: an array of hexadecimal numbers representing the maze
 *
 * Returns:
 *	- nothing - each maze_room in the maze should be represented
 *	  as a hexadecimal number  and put in nums at the corresponding index
 */
void encode_maze(struct maze_room maze[], int num_rows, 
	int num_cols, int result[]) {	
	int *r = result;  
	struct maze_room *m = maze;

	for(int i = 0; i < num_rows*num_cols; i++) {	
			r[i] = encode_room(m[i]);  // Encodes each room in result maze
	}		
}

/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 4
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["generator", <output file>, <number of rows>, <number of columns>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */
int main(int argc, char **argv) {
	char *file_name;
	int num_rows;
	int num_cols;

	if (argc != 4) {
		printf("Incorrect number of arguments.\n");
		printf("./generator <output file> <number of rows> <number of columns>\n");
		return 1;
	} else {
		file_name = argv[1];
		num_rows = atoi(argv[2]);
		num_cols = atoi(argv[3]);
	}

	if(num_cols <= 0 || num_rows <= 0) {
		fprintf(stderr, "Dimensions must be positive integers.\n");
		return 1;
	}

	struct maze_room maze[num_cols * num_rows];
	
	for(int i = 0; i < num_rows * num_cols; i++) {
			maze[i].visited = 0;

			for(int j = 0; j < 4; j++)
				maze[i].connections[j] = -1;
	}
	
	int result[num_cols * num_rows];

	drunken_walk(maze, 0, 0, num_rows, num_cols);
	encode_maze(maze, num_rows, num_cols, result);

	FILE *file = fopen(file_name, "w");

	if(!file) {
		fprintf(stderr, "Could not open read file.\n");
		return 1;
	}

	for(int i = 0; i < num_rows; i++) {
		for(int k = 0; k < num_cols; k++) {
			fprintf(file, "%x", result[i * num_cols + k]);
		}
		fprintf(file, "\n");
	}

	if(fclose(file) != 0) {
		fprintf(stderr, "A error occured in read file");
		return 1;
	}

	return 0;
}

