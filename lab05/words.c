#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "words.h"
#include "ttable.h"

#define is_letter(c) ((c >= 0x41 && c <= 0x5a) || (c >= 0x61 && c <= 0x7a))
#define is_uppercase(c) (c >= 0x41 && c <= 0x5a)

/*
 * Converts letters in str to lower case.
 */
void lower(char *str) {
    unsigned int i;
    for (i = 0; i < strlen(str); i++) {
        if (is_uppercase(str[i])) {
            // Ors letter with 32, which adds 32 to an uppercase 
            // letter, making it lowercase.
            str[i] |= 0x20;
        }
    }
}

/*
 * Reads the word stored at bufpos in buf and stores a 
 * lowercase version of the word in result.
 * Returns the length of the word.
 */
int word(const char *buf, size_t *bufpos, size_t file_len, char *result, int len) {
    int pos;
    int c;

    // 1. Finds the start and end of this word.
	c = buf[*bufpos];
    for (pos = 0; *bufpos < file_len && pos < len-1; ++(*bufpos)) {
		c = buf[*bufpos];
        if (is_letter(c)) {

            // Basically do lower now
            if(is_uppercase(c)) {
                result[pos++] = 0x20 | (char)c;
            } else result[pos++] = (char)c;

            continue;
        }
        if (c == '-') {
            // c is a hyphen. How it's treated depends on 
            // the next character (if any).
            int nextc = buf[(*bufpos)++];

            // If nextc is a letter, this is a hyphenated word and the hyphen 
            // should be included in the result.
            if (is_letter(nextc)) {
                result[pos++] = (char)nextc;
                continue;
            }

            // If nextc is a newline, then the hyphen is splitting the word 
            // across two lines and should be ignored.
            if (nextc == '\n') {
                continue;
            }

            /* If none of the above are true, c is not acting as a hyphen 
            and is treated as white space */
        }

        // If c is not a letter or a hyphen, it is treated as whitespace 
        // (both whitespace and punctuation are considered whitespace).
        if (pos == 0) {
            // Whitespace or punctuation at beginning of word is ignored.
            continue;
        }
        // Whitespace at end of word signals the end of the word.
        break;
    }

    result[pos] = '\0';
	if( *bufpos >= file_len )
		return -1;

    // 2. Lowercases this word.
   // lower(result);

    return pos; /* pos is the length of the word */
}

/*
 * Appends src to destination.
 */
void strconcat(char *dest, char *src, int len) {
    int i, j;
    //printf("dest: %s\nsrc: %s\n", dest, src);
    // Find the end of the destination string.
    //for (i = 0; dest[i] != 0; ++i) {}
    i = len;
    // Append src to destination.
    for (j = 0; src[j] != 0; ++i, ++j) {
        dest[i] = src[j];
    }
    dest[i] = 0;
}

/*
 * Concatenate the strings w1 and w2, saving the result in res.
 * If w1 is "a" and w2 is "b", then "a b" is stored in res.
 */
void concat(char *w1, char *w2, char *res, int l1) {
    res[0] = 0;
    strconcat(res, w1, 0);
    strconcat(res, " ", l1);
    strconcat(res, w2, l1+1);
}

/*
 * Takes a text file (file_buffer) and reads through the file, storing every bigram in a hashtable.
 * It also takes a character array (word_buffer), which it uses as temporary storage while calculating bigrams.
 */
int run_words(char *file_buffer, size_t file_len, char *word_buffer) {
	size_t pos = 0; 	// position in word buffer
    char *tmp;   // for swap of w1 and w2

    // Will store the first word, second word, and bigram in 
    // word_buffer while calculating, so partitons word_buffer 
    // to make enough space for all three.
    char *w1 = word_buffer;
    char *w2 = word_buffer + MAX_WORD_SIZE;
    char *bigram = word_buffer + 2*MAX_WORD_SIZE;

    // Make the first word in the file lowercase. Then on every 
    // loop, make the second word in each bigram lowercase.
    int l1 = word(file_buffer, &pos, file_len, w1, MAX_WORD_SIZE);
    int l2 = word(file_buffer, &pos, file_len, w2, MAX_WORD_SIZE);

    while (l2 >= 0) {
        // Concatenate the lowercased w1 and w2 and store the result in bigram.
        concat(w1, w2, bigram, l1);

        // Insert bigram into the hashtable, which keeps track of 
        // how many of each pair have occurred.
        ttable_insert(bigram);

        // Swap w1 and w2, so that this bigram's second word is 
        // the first word in the next bigram.
        // (e.g. if text file is "A B C" then the first bigram 
        // is "A B" and the second is "B C").
        // The second word in the next bigram will overwrite 
        // this bigram's first word in word_buffer.
        tmp = w1;
        w1 = w2;
        w2 = tmp;
        l1 = l2;
        l2 = word(file_buffer, &pos, file_len, w2, MAX_WORD_SIZE);        
    }

    return 0;
}
