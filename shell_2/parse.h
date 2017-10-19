/*
 * Gets the next index where the next string (char array) starts
 *
 * Parameters:
 *  - str: null seperated strings
 *  - c: current index of the first char in the string
 *  - len: full length of the whole unparsed input
 *
 * Returns:
 *  - c the index of the first char in the next string found
 */
int find_next(char *str, int c, int len);

/*
 * Parses the input
 *
 * Parameters:
 *  - str: the unparsed string
 *  - iname: the current input file name
 *  - oname: the current output file name
 *  - iflag: the current input flag status
 *  - oflag: the current output flag status
 *  - narg: the number of arguments in the parsed string
 *  - argc: a list of the arguments index in str (set to 1024)
 *
 * Returns:
 *  - No return
 */
int parse(char *str, char *iname, char *oname, int *iflag, int *oflag,
    int *aflag, int *narg, int *argc);
