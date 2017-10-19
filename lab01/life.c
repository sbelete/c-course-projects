 /* CS033 Lab 01 - Life
   Written June 2012 by the CS033 Dev Team

   Executes Conway's Game of Life on the game board set in main,
   printing the results to stdout. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEFAULT_STEPS 10

/* Execute the Life algorithm for a number of steps given the initial generation array */
void do_life(int rows, int cols, int array[rows * cols], int steps);

/* Gets the index of a one-dimensional array that corresponds to the given row and column in a two-dimensional grid */
int get_index(int row, int col, int num_cols);

/* Performs a single iteration of the Life algorithm, given the old game board in old_array,
   and stores the result in new_array */
void update(int rows, int cols, int old_array[rows * cols], int new_array[rows * cols]);

/* Determines the next state for the cell at a particular row and column based on the previous iteration's board */
int get_next_state(int rows, int cols, int array[rows * cols], int row, int col);

/* Determines whether or not the cell at a particular row and column is on the game board. */
int is_in_range(int rows, int cols, int row, int col);

/* Determines the number of live neighbors of the cell at a particular row and column. */
int count_alive_neighbors(int rows, int cols, int array[rows * cols], int row, int col);

/* Determines whether or not the cell at a particular row and column is alive on the board array. */
int is_alive(int rows, int cols, int array[rows * cols], int row, int col);

/* Sets the cell at a particular row and column to be alive on the board array. */
void set_alive(int rows, int cols, int array[rows * cols], int row, int col);

/* Sets the cell at a particular row and column to be dead on the board array. */
void set_dead(int rows, int cols, int array[rows * cols], int row, int col);

/* Prints the given array to stdout. */
void print_array(int rows, int cols, int array[rows * cols]);

/* Function main

   This function is where the program begins.  Initializes the game board
   and calls do_life to execute the algorithm.

   Input: int    argc - The number of program arguments, including the executable name
          char **argv - An array of strings containing the program arguments.

   Output: 0 upon completion of the program

   Note: you do not need to modify this function to complete this lab.
 */
int main(int argc, char **argv) {
  if (argc != 4) {
    printf("Usage: ./life <board_file> <num_rows> <num_cols>\n");
    return 1;
  }

  char *board_file = argv[1];
  int rows = atoi(argv[2]);
  int cols = atoi(argv[3]);

  if (rows <= 0 || cols <= 0) {
    fprintf(stderr, "Dimension must be positive.\n");
    return 1;
  }

  FILE *in_file = fopen(board_file, "r");
  if (in_file == NULL) {
    fprintf(stderr, "Could not open file.\n");
    return 1;
  }

  /* read in the board_file into arr1*/
  int arr1[rows * cols];

  int cell = 0;
  int i, j;

  for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
      if (fscanf(in_file, "%1d", &cell) == EOF) {
       fprintf(stderr,"Dimension mismatched!\n");
       return 1;
      }
      arr1[(i*cols) + j] = cell;
    }
  }

  if (fscanf(in_file, "%1d", &cell) != EOF) {
    fprintf(stderr,"Dimension mismatched!\n");
    return 1;
  }

  /* Call the function do_life, passing in the board dimensions, the board itself,
    and the number of iterations to run the Life algorithm. */

  do_life(rows,cols,arr1,DEFAULT_STEPS);

  if (fclose(in_file)) {
    fprintf(stderr, "Could not close the file\n");
    return 1;
  }

  /* Return 0 to show that the program has completed successfully. */
  return 0;
}


/* Function do_life

   Executes Conway's Game of Life on board array for steps iterations,
   printing the board at each iteration.

   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the initial game board, a 1D array of zeros and ones
          int     steps - the number of iterations for which to run the life algorithm

   Output: None
 */
void do_life(int rows, int cols, int array[rows * cols], int steps) {
    /* Create an array to store the next state of the board at each step */
    int spare[rows * cols];

    /* Create a pointer to each array */
    int *arr1 = array;
    int *arr2 = spare;

    /* Run the Life algorithm steps times.  At each iteration, you should do the following:
         1. Print the current generation of the game board by calling print_array.
         2. Get the next generation of the game board by calling update, passing in the old state
            and the spare array to hold the new state.
         3. Switch the pointers to the old and new states.
     */

    for(int i = 0; i < steps; i++) {
      print_array(rows, cols, arr1);
      update(rows, cols, arr1, arr2);

      int *temp = arr1;
      arr1 = arr2;
      arr2 = temp;
    }

    /*  Once this is complete, print the final generation of the game board by calling print_array. */
    print_array(rows, cols, array);
}

/* Function get_index

   Gets the index of a one-dimensional array that corresponds to the given row and column in a two-dimensional grid
   Input: int     row  - the row of the game board that is being queried
          int     col  - the column of the game board that is being queried
          int     num_cols  - the number of columns of the game board

    Output: the index in the one dimensional array that corresponds to the given row
    and column
*/
int get_index(int row, int col, int num_cols) {
  return (row * num_cols) + col;
}


/* Function update

   Given the previous generation of the game board in old_array, performs a single iteration of Conway's
   Game of Life, storing the new generation of the game board in new_array.

   Input: int     rows      - the number of rows of the game board
          int     cols      - the number of columns of the game board
          int[]   old_array - the previous generation of the game board, an array of zeros and ones
          int[]   new_array - an array where the next generation of the game board will be stored

   Output: None
 */
void update(int rows, int cols, int old_array[rows * cols], int new_array[rows * cols]) {
    /* For each cell in old_array, you should:
         1. Determine whether that cell will be alive or dead in the next generation
            by calling get_next_state on old_array.
         2. If the cell's next state is 1 (alive), set that cell to alive in new_array
            by calling set_alive.  Otherwise, set that cell to dead in new_array by calling
            set_dead. */
            for(int i = 0; i < rows; i++){
              for(int k = 0; k < cols; k++){
                if(get_next_state(rows, cols, old_array, i, k) == 1)
                 set_alive(rows, cols, new_array, i, k);
                else
                set_dead(rows, cols, new_array, i, k);
              }
            }
}


/* Function get_next_state

   Given the previous generation of the game board, determines the next state of the cell at a particular
   row and column (row, col).  If the cell is not on the game board, prints an error message and terminates the program.


   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the previous generation of the game board, a 1D array of zeros and ones
          int     row   - the row of the cell whose next state is being determined
          int     col   - the column of the cell whose next state is being determined

   Output: The next state of the cell at (row, col); 0 if the cell will be dead and 1 if it will be alive.
 */
int get_next_state(int rows, int cols, int array[rows * cols], int row, int col) {
    /* First, use an assert statement and call is_in_range to confirm the specified cell is on the game board. */
 assert(is_in_range(rows, cols, row, col));
    /* Then, assuming the assert succeeds:
         1. Determine how many live neighbors the cell had in the previous generation by calling count_alive_neighbors.
         2. Determine if the cell was alive in the previous generation by calling is_alive.
         3. Return 1 (alive) if the cell was alive and had 2 or 3 live neighbors, or if the cell was dead and had 3 live neighbors.
         4. Return 0 (dead) otherwise. */
   int neighbor = count_alive_neighbors(rows, cols, array, row, col);
         int check = is_alive(rows, cols, array, row, col);

         if(check == 1 && (neighbor == 2 || neighbor == 3))
          return 1;
         else if(check == 0 && neighbor == 3)
                return 1;
              else 
                return 0;
}

/* Function is_in_range

   Given a row and column (row, col), determines if the cell at that position is on a zero-indexed game board
   with a given number of rows and columns.


   Input: int rows - the number of rows of the game board
          int cols - the number of columns of the game board
          int row  - the row of the cell in question
          int col  - the column of the cell in question

   Output: 1 (true) if the cell is on the game board and 0 (false) otherwise
 */
int is_in_range(int rows, int cols, int row, int col) {
    /* Return 1 if row is between 0 and rows-1, inclusive and col is between 0 and cols-1, inclusive; otherwise return 0 */
  if(row > (rows-1) || col > (cols-1) || row < 0 || col < 0){
    return 0;
  }

  return 1; 
}

/* Function count_alive_neighbors

   Given a game board, array, and a row and column (row, col), determines the number of surrounding cells that are alive.
   If the cell is not on the game board, prints an error message and terminates the program.


   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the previous generation of the game board, a 1D array of zeros and ones
          int     row   - the row of the cell whose neighbors are being counted
          int     col   - the column of the cell whose neighbors are being counted

   Output: The number of surrounding cells that are alive
 */
int count_alive_neighbors(int rows, int cols, int array[rows * cols], int row, int col) {
    /* First, use an assert statement and call is_in_range to confirm the specified cell is on the game board. */
    assert(is_in_range(rows, cols, row, col));
    /* Then, assuming the assert succeeds:
         Compute and return the number of live neighbors by adding together the results of is_alive for each of
         the 8 surrounding cells.  Since is_alive checks to see if a cell is on the board, you do not need to do so here. */
int num_alive = 0;

for(int a = -1; a < 2; a++)
  for (int i = -1; i < 2; ++i){
    num_alive += is_alive(rows, cols, array, row + a, col + i);
  }

num_alive -= is_alive(rows, cols, array, row, col);

return num_alive;

}

/* Function is_alive

   Given a game board, array, and a row and column (row, col), determines if the cell is alive or not.  If the cell
   is not on the game board, returns 0.

   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the game board, a 1D array of zeros and ones
          int     row   - the row of the cell in question
          int     col   - the column of the cell in question

   Output: 1 if the cell is alive (is 1), 0 if it is dead (is 0) or out of range.
 */
int is_alive(int rows, int cols, int array[rows * cols], int row, int col) {
    /* First, determine if the cell is on the game board by calling is_in_range.
       If the result is 1 (true), then return the value of the cell
       Otherwise, return 0.  Unlike with other functions, this should not cause the program to terminate. */
       if(is_in_range(rows, cols, row, col)){
        return array[get_index(row, col, cols)];
       }
       
       return 0;
}

/* Function set_alive

   Given a game board, array, and a row and column (row, col), sets the cell to be alive.  If the cell is not on
   the game board, prints an error message and terminates the program.

   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the game board, a 1D array of zeros and ones
          int     row   - the row of the cell in question
          int     col   - the column of the cell in question

   Output: None
 */
void set_alive(int rows, int cols, int array[rows * cols], int row, int col) {
    /* First, use an assert statement and call is_in_range to confirm the specified cell is on the game board. */
    assert(is_in_range(rows, cols, row, col));
  /* Then, assuming the assert succeeds, set that cell's value to 1 in array.*/
   array[get_index(row, col, cols)] = 1;
}

/* Function set_dead

   Given a game board, array, and a row and column (row, col), sets the cell to be dead.  If the cell is not on
   the game board, prints an error message and terminates the program.

   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the game board, a 1D array of zeros and ones
          int     row   - the row of the cell in question
          int     col   - the column of the cell in question

   Output: None
 */
void set_dead(int rows, int cols, int array[rows * cols], int row, int col) {
    /* First, use an assert statement and call is_in_range to confirm the specified cell is on the game board. */
  assert(is_in_range(rows, cols, row, col));
    /* Then, assuming the assert succeeds, set that cell's value to 0 in array.*/
  array[get_index(row, col, cols)] = 0;
}

/* Function print_array

   Given a game board, array, prints the board to stdout using printf.

   Input: int     rows  - the number of rows of the game board
          int     cols  - the number of columns of the game board
          int[]   array - the game board, a 1D array of zeros and ones

   Output: None
 */
void print_array(int rows, int cols, int array[rows * cols]) {
    /* For each cell on the game board, use printf to print the result of is_alive for that cell.
       Cells should be separated by single spaces. After each row has been printed, be sure to
       print a new line. Additionally, at the end of each iteration, be sure to print a new line
       to separate the generations. */

       for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
          printf("%i ", array[get_index(i, j, cols)]);
        }

        printf("\n");
       }

       printf("\n");
}

