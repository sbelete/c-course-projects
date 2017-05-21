/*
 * Enum to represent the four directions
 * Here is an example of how to use an enum:
 *
 * Direction dir = EAST;
 * if (dir == EAST) {
 *     printf("Heading EAST!");
 * }
 */
typedef enum {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3
} Direction;

/*
 * Struct to represent a room in the maze.
 * Must store:
 * 	- The location of the room
 * 	- Whether or not this room has been visited
 *	- The connections to neighboring rooms in all
 *	  four directions.
 */
struct maze_room {
	int r;  // row
	int c;  // column
	int visited;  // boolean of visted or not
	int connections[4];  // connections to four neighbors
	struct maze_room *next;
};


int get_index(int row, int col, int num_cols);

int is_in_range(int row, int col, int num_rows, int num_cols);

struct maze_room *get_neighbor(struct maze_room maze[], int num_rows,
        int num_cols, struct maze_room *room, Direction dir);

int direction_index(int row, int col, Direction dir, 
	int num_rows, int num_cols);
