#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "solver.h"

/*
 * Given a pointer to a maze_room, set its connections in all four directions based
 * on the hex value passed in.
 *
 * For example:
 *
 * 		create_room_connections(&maze[3][4], 0xb)
 *
 * 0xb is 1011 in binary, which means we have walls on all sides except the WEST.
 * This means that every connection in maze[3][4] should be set to 1, except the WEST
 * connection, which should be set to 0.
 *
 * See the handout for more details about our hexadecimal representation, as well as
 * examples on how to use bit masks to extract information from this hexadecimal
 * representation.
 *
 * Parameters:
 *	- room: pointer to the current room
 *	- hex: hexadecimal integer (between 0-15 inclusive) that represents the connections
 *		   in all four directions from the given room.
 *
 * Returns:
 *	- nothing. The connections should be saved in the maze_room struct pointed to by
 *	  the parameter (make sure to use pointers correctly!).
 */
void create_room_connections(struct maze_room *room, unsigned int hex) {
	// Parses room to make maze and room connections
	if(hex >= 8) {
		hex -= 8;
		room->connections[3] = 1; 
	} else {
		room-> connections[3] = 0;
	  }

	if(hex >= 4) {
		hex -= 4;
		room->connections[2] = 1;
	} else {
		room-> connections[2] = 0;
	  }

	if(hex >= 2) {
		hex -= 2;
		room->connections[1] = 1;
	} else {
		room-> connections[1] = 0;
	  }

	if(hex == 1) {
		hex -= 1;
		room->connections[0] = 1;
	} else {
		room-> connections[0] = 0;
	  }
}

/*
 * Recursive depth-first search algorithm for solving your maze.
 * This function should also print out either every visited room as it goes, or
 * the final pruned solution, depending on whether the FULL macro is set.
 * 
 * Note that you may need to add extra parameters to this function's signature,
 * depending on how you choose to store the pruned solution.
 *
 * See handout for more details, as well as a pseudocode implementation.
 *
 * Parameters:
 *	- row: row of the current room
 *	- col: column of the current room
 *	- goal_row: row of the goal room
 *	- goal_col: col of the goal room
 *	- num_rows: number of rows in the maze
 *	- num_cols: number of columns in the maze
 *	- maze: an array of maze_room structs representing your maze
 *	- file: the file to write the solution to
 *
 * Returns:
 *	- 1 if the current branch finds a valid solution, 0 if no branches are valid.
 */
int dfs(int row, int col, int goal_row, int goal_col, 
		int num_rows, int num_cols, struct maze_room maze[], FILE *file) {
	#ifdef FULL
	fprintf(file, "%d, %d\n", row, col);
	#endif
	
	struct maze_room *room = &maze[get_index(row, col, num_cols)];
	struct maze_room *m = maze;

	// Check to see if done with traversing maze
	if(row == goal_row && col == goal_col) {
		room->next = room;
		room->visited = 1;
		return 1;
	}

	room->visited = 1;  

	for(int i = 0; i < 4; i++) {
		if(direction_index(row, col, i, num_rows, num_cols) == 1) {
			struct maze_room *neighbor = 
				get_neighbor(m, num_rows, num_cols, room, i);
			
			room->next = neighbor;
		
			if(room->connections[i] == 0 && neighbor->visited == 0) {
				if(dfs(neighbor->r, neighbor->c, goal_row, goal_col, 
					   num_rows, num_cols, m, file) == 1) {
					return 1;
				}
				#ifdef FULL
				fprintf(file, "%d, %d\n", row, col);
				#endif
			}
		}
	}

	return 0;  // Didn't find the goal in this branch
}

/*
 * Prints the prunned path from start to goal.
 *
 * Parameters:
 *  - room: The current room in the path to the goal.
 *  - file: The file to print the path to.
 *
 * Returns:
 * 	Nothing
 */
void print_prunned(struct maze_room *room, FILE *file, int goal_row, int goal_col) {
	fprintf(file, "%d, %d\n", room->r, room->c);

	// Stop when at goal
	if(!(room->r == goal_row && room->c == goal_col)) {
		struct maze_room *n = room->next;
		print_prunned(n, file, goal_row, goal_col);
	}
}


/*
 * Main function
 *
 * Parameters:
 *	- argc: the number of command line arguments - for this function 9
 *	- **argv: a pointer to the first element in the command line
 *            arguments array - for this function:
 *		      ["solver", <input maze file>, <number of rows>, <number of columns>
 *				 <output path file>, <starting row>, <starting column>, 
 *				 <ending row>, <ending column>]
 *
 * Returns:
 *	- 0 if program exits correctly, 1 if there is an error
 */

int main(int argc, char **argv) {
	int num_rows, num_cols, start_row, start_col, goal_row, goal_col;
	char *maze_file;
	char *path_file;
	if (argc != 9) {
		printf("Incorrect number of arguments.\n");
		printf("./solver <input maze file> <number of rows> <number of columns>");
		printf(" <output path file> <starting row> <starting column>");
		printf(" <ending row> <ending column>\n");
		return 1;
	} else {
		maze_file = argv[1];
		num_rows = atoi(argv[2]);
		num_cols = atoi(argv[3]);
		path_file = argv[4];
		start_row = atoi(argv[5]);
		start_col = atoi(argv[6]);
		goal_row = atoi(argv[7]);
		goal_col = atoi(argv[8]);
	}

	if(num_cols <= 0 || num_rows <= 0) {
		fprintf(stderr, "Dimensions must be positive integers.\n");
		return 1;
	}

	if (num_rows <= start_row || start_row < 0 ||
			 num_cols <= start_col || start_col < 0) {
		fprintf(stderr, "Your starting row and column are out of bounds.\n");
		return 1;
	}

	if (num_rows <= goal_row || goal_row < 0 ||
			 num_cols <= goal_col || goal_col < 0) {
		fprintf(stderr, "Your goal row and column are out of bounds.\n");
		return 1;
	}

	struct maze_room maze[num_rows*num_cols];

	FILE *file = fopen(maze_file, "r");
	FILE *pfile = fopen(path_file, "w");

	if(!file) {
		fprintf(stderr, "Could not open read file.\n");
		return 1;
	}

	if(!pfile) {
		fprintf(stderr, "Could not open write file.\n");
		return 1;
	}

	#ifdef FULL
	fprintf(pfile, "FULL\n");

	#else 
	fprintf(pfile, "PRUNED\n");

	#endif

	unsigned int hex;
	int index;
	
	for (int i = 0; i < num_rows; i++) {
		for (int j = 0; j < num_cols; j++) {
			fscanf(file, "%1x", &hex);
			index = get_index(i, j, num_cols);

			create_room_connections(&maze[index], hex);
			// Initiallizing maze rooms
			maze[index].r = i;
			maze[index].c = j;
			maze[index].visited = 0;
			maze[index].next = '\0';
		}
	}

	struct maze_room *m = maze;

	if(dfs(start_row, start_col, goal_row, goal_col, 
		num_rows, num_cols, m, pfile) == 0) {
		fprintf(stderr, "No solution. Invalid Maze as input.\n");
	} else {
		#ifdef FULL
		#else
		print_prunned(&maze[get_index(start_row, start_col, num_cols)], 
			pfile, goal_row, goal_col);
		#endif
	}

	if(fclose(file) != 0) {
		fprintf(stderr, "A error occured in read file");
		return 1;
	}

	if(fclose(pfile)!= 0) {
		fprintf(stderr, "A error occured in write file");
	}

	return 0;
}

