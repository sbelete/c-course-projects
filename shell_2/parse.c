#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "./parse.h"

int find_next(char *str, int c, int len) {
  // Stepping through current string's characters
  while (c <= len && str[c] != '\0') { c++; }

  // Stepping through the null seperating the strings
  while (str[c] == '\0' && c <= len) { c++; }

  return c;  // The first index of the next string (or null last character)
}

int parse(char *str, char *iname, char *oname, int *iflag, int *oflag,
    int *aflag, int *narg, int *argc) {
  int flip = 0;  // helps keep track of number of arguments

  int i;  // Step through the whole string
  for (i = 0; str[i] != '\0'; i++) {
    //  Looks for all the spaces in teh string
    if (isspace(str[i])) {
      flip = 0;
      str[i] = '\0';  // Converts all spaces to null
    } else if (flip == 0) {
      *narg += 1;  // Detects changes from 0 to 1 in flip
      flip = 1;
    }
  }

  // If the input has all white space arguments return -1
  if (*narg == 0) { return -1;}

  int c = 0;  // Find the first non-whitespace character
  while (str[c] == '\0' && c <= i) {c++;}

  int index = 0;  // Find first char index of non-redirects or redirects' names
  while (str[c] != '\0') {
    if (!strcmp(&str[c], "&") && index+1 == *narg) {
      *narg -= 1;
      *aflag += 1;

      c = find_next(str, c, i);
    }

    //  Look for append output and handle the match
    if (!strcmp(&str[c], ">>")) {
      if (*oflag != 0) {
        perror("syntax error: multiple output file\n");
        return -1;
      }

      *oflag = 2;
      *narg -= 2;
      c = find_next(str, c, i);
      strcpy(oname, &str[c]);

      if (!strcmp(oname, ">>") || !strcmp(oname, ">") ||
       !strcmp(oname, "<")) {
         perror("syntax error: output file is a redirection symbol\n");
         return -1;
       }

      if (oname[0] == '\0') {
        perror("syntax error: no output file\n");
        return -1;
      }
      // Look for truncate output and handle the match
    } else if (!strcmp(&str[c], ">")) {
      if (*oflag != 0) {
        perror("syntax error: multiple output files\n");
        return -1;
      }

      *oflag = 1;
      *narg -= 2;
      c = find_next(str, c, i);
      strcpy(oname, &str[c]);

      if (!strcmp(oname, ">>") && !strcmp(oname, ">") &&
       !strcmp(oname, "<")) {
         perror("syntax error: output file is a redirection symbol\n");
         return -1;
       }

      if (oname[0] == '\0') {
        perror("syntax error: no output file\n");
        return -1;
      }
      // Look for input and handle the match
    } else if (!strcmp(&str[c], "<")) {
      if (*iflag != 0) {
        perror("syntax error: multiple input files\n");
        return -1;
      }

      *iflag = 3;
      *narg -= 2;
      c = find_next(str, c, i);
      strcpy(iname, &str[c]);

      if (!strcmp(iname, ">>") && !strcmp(iname, ">") &&
       !strcmp(iname, "<")) {
         perror("syntax error: input file is a redirection symbol\n");
         return -1;
       }

      if (iname[0] == '\0') {
        perror("syntax error: no input file\n");
        return -1;
      }
    } else {
      // This is the first char index of non-redirects or redirects' names
      argc[index] = c;
      index++;
    }

    c = find_next(str, c, i);  // Find new first char of next string
  }

  return 1;  // Succefully parsed the original string
}
