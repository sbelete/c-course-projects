/*
 * Brown University - Department of Computer Science
 * CS033 - Introduction To Computer Systems - Fall 2015
 * Prof. Thomas W. Doeppner
 * Assignment 6 - Performance 2: A CS033 String Library - strings.c
 * Due date: 10/27/15
 */

/*      _        _
 *  ___| |_ _ __(_)_ __   __ _ ___ 
 * / __| __| '__| | '_ \ / _` / __|
 * \__ \ |_| |  | | | | | (_| \__ \
 * |___/\__|_|  |_|_| |_|\__, |___/
 *                       |___/     
 */

/* XXX: Preprocessor instruction to enable basic macros; do not modify. */
#include <stddef.h>

/* 
 * strlen() - abbreviated from "string length".
 * 
 * - Description: determine the number of characters in a character string.
 * 
 * - Arguments: a pointer to a null-terminated character string.
 * 
 * - Return value: the number of characters using a special unsigned numerical 
 *   type, which is intended specifically for pointer arithmetic, called size_t.
 *
 * - Hint: the length of a C string does _not_ include the null ('\0')
 *   character that terminates it.
 */
size_t strlen(const char *s) {
  int length = 0;  // accumulator for return

  // iterates through s until the end
  for (length = 0; s[length] != '\0'; length++) {
  }

  return length;  // returns the length of the string
}

/*
 * strspn() - abbreviated from "string span".
 * 
 * - Description: determine the number of characters in a character string 
 *   that consist of characters in accept
 * 
 * - Arguments: a pointer to a null-terminated character string, 
 *   and a point to a null terminated string of characters to accept
 * 
 * - Return value: the number of characters at the beginning of s that are in accept 
 */
size_t strspn(const char *s, const char *accept) {
    int i;  // accumulator for return
    int j;  // iterator for the second loop

    // Iterates through s
    for (i = 0; s[i] != '\0'; i++) {
        // Iterates through accept
        for (j = 0; accept[j] != '\0'; j++) {
            // Compares accept and s
          if (s[i] == accept[j])
            break;  // Break if they are the same
        }
        
        // Checks to see if accept reached the end
        if (accept[j] == '\0')
          break;  // Break if true
    }

    // Return accumulator
    return i;
}

/*
 * strcspn() - abbreviated from "string character span".
 * 
 * - Description: determine the number of characters in a character string 
 *   that consist of characters not in reject
 * 
 * - Arguments: a pointer to a null-terminated character string, 
 *   and a point to a null terminated string of characters to reject
 * 
 * - Return value: the number of characters at the beginning of s that are not in reject 
 */
size_t strcspn(const char *s, const char *reject) {
  int i;  // accumulator for return
  int j;  // iterator for the second loop

  // Iterates through s
  for (i = 0; s[i] != '\0'; i++) {
      // Iterates through reject
      for (j = 0; reject[j] != '\0'; j++) {
        // Compares accept and s
        if (s[i] == reject[j])
          break;  // Break if they are the same
      }
        
      // Checks to see if reject reached the end
      if (reject[j] != '\0')
        break;  // Break if false
    }

  // Return accumulator
  return i;
}

/*
 * strncmp() - abbreviated from "string compare (n bytes)".
 * 
 * - Description: compares the first n bytes of two strings, s1 and s2
 * 
 * - Arguments: two null terminated strings (s1 and s2) and the maximum number of bytes
 *   to compare (n).
 * 
 * - Return value: negative number if s1 < s2, 0 if s1 == s2, positive number if s1 > s2
 */
int strncmp(const char *s1, const char *s2, size_t n) {
    int val;  // The value of the difference between s1 and s2 

    // While s1 hasn't reached the end and we haven't exceeded n continue loop
    while (*s1 != '\0' && n != 0) {
      val = *s1 - *s2;  // Value is the diffence between s1 and s2

      // Checks to see if s1 != s2
      if (val != 0)
        return val;  // If s1 and s2 are diffrent return val

      n--;  // Decrease n for max number of iterations
      s1++;  // iterate to next char in s1
      s2++;  // iterate to next char in s2
    }

    if (n == 0) 
      return 0;  // made it to the end of n with no diffrences  

    return *s1-*s2;  // the final diffrence between s1 and s2
}

/*
 * strstr() - abbreviated from "string (find) string".
 *
 * - Description: Find the string "needle" in the string "haystack". This is
 *   not some joke from the TAs but the actual terminology used in the Linux
 *   version of strstr(). Don't believe us? Type "man strstr" into a terminal,
 *   and see for yourself! For another example of systems programming humor,
 *   you may also appreciate "man strfry".
 *
 * - Hint: Use strncmp().
 */
char *strstr(const char *haystack, const char *needle) {
  // Save the length of needle, for strncmp use
  int length = strlen(needle);

  // A little short cut for zero lengths
  if (length == 0) {
    return ('\0');
  }
  
  // Iterator
  int i;

  // Iterates through haystack
  for (i = 0; haystack[i] != '\0'; i++) {
    // Compare needle to haystack (starting from different chars)
    if (strncmp(&haystack[i], needle, length) == 0)  
      return (char *) &haystack[i];  // If equal then return rest of haystack
  }

  return ('\0');  // If no needle was found return null
}

/*
 * strtok() - abbreviated from "string tokenize".
 *
 * Examples:
 * 1. > char *str = "1,2,,,3,4";
 *    > char *s1 = strtok(str, ",");  // s1 should point to string starting @ 1
 *    > char *s2 = strtok(NULL, ","); // s2 should point to string starting @ 2
 *    > char *s3 = strtok(NULL, ","); // s2 should point to string starting @ 3

 * - Hint: This function must save its state between calls, how might you 
 *   accomplish this? Note that this version of the function is destructive on
 *   the string str. Do not expect it to come out the other end in one piece... 
 *   literally.
 */
char *strtok(char *str, const char *delim) {
	static char* rem = '\0';  // Save state 

  // Check to make sure that str isn't null
  if (str != '\0') {
    rem = str;
  } else {
      if (rem == '\0') {
        return ('\0');  // Return null of end state
    }
  }

  str = rem + strspn(rem, delim);  // finds start delim in str and saves
  rem = str + strcspn(str, delim);  // finds delim not in rem and saves

  // check to see if rem equals str
  if (rem == str) {
    return '\0';  // Return null of end state
  }

  // if *rem is not null
  if (*rem != '\0') {
    *rem = '\0';
    rem += 1;  // inc by one
  } else {
    rem = '\0';  // set it to null
  }

  return str;  // return string of delim removed
}
