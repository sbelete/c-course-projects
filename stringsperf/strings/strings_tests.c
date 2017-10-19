#ifdef BUILTIN_STRINGS
#include <string.h>
#else
#include "./strings.h"
#endif

#include "./strings_unit_tests.h"
#include "./sec.h"
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define USEAGE "./run_tests <number of times to run test, default to 1000000> <all | names of tests to run>" \
    "\n   Ex. \"./run_tests 10000 all\" runs all tests 10000 times each" \
    "\n   Ex. \"./run_tests 100 strlen strchr\" runs the strlen and strchr tests 100 times each" \
    "\n   Ex. \"./run_tests strtok\" runs the strtok test 1000000 times" 

#define SUPER_LONG_STRING "./run_tests <number of times to run test, default to 1000000> <all | names of tests to run>" \
    "\n   Ex. \"./run_tests 10000 all\" runs all tests 10000 times each" \
    "\n   Ex. \"./run_tests 100 strlen strchr\" runs the strlen and strchr tests 100 times each" \
    "\n   Ex. \"./run_tests strtok\" runs the strtok test 1000000 times" \
    "./run_tests <number of times to run test, default to 1000000> <all | names of tests to run>" \
    "\n   Ex. \"./run_tests 10000 all\" runs all tests 10000 times each" \
    "\n   Ex. \"./run_tests 100 strlen strchr\" runs the strlen and strchr tests 100 times each" \
    "\n   Ex. \"./run_tests strtok\" runs the strtok test 1000000 times" \
    "./run_tests <number of times to run test, default to 1000000> <all | names of tests to run>" \
    "\n   Ex. \"./run_tests 10000 all\" runs all tests 10000 times each" \
    "\n   Ex. \"./run_tests 100 strlen strchr\" runs the strlen and strchr tests 100 times each" \
    "\n   Ex. \"./run_tests strtok\" runs the strtok test 1000000 times" 

#define DEFAULT_REPEAT 1000000

int num_repeat;

extern int should_print;
extern int should_count;

int should_time = 0;

void time_test(void (*)(), int, const char *);

// test functions
// size_t strlen(const char *s);
void test_strlen() {
    char s[350];
    sprintf(s, "strlen(\"%s\")", USEAGE);

    assert_equal_int(0, strlen(""), "strlen(\"\")");
    assert_equal_int(4, strlen("asdf"), "strlen(\"asdf\")");
    assert_equal_int(5, strlen("asdf "), "strlen(\"asdf \")");
    assert_equal_int(2, strlen("\n\n"), "strlen(\"\n\n\")");
    assert_equal_int(304, strlen(USEAGE), s);
}

// char *strchr(const char *s, int c);
void test_strchr() {
    char *str1 = "12345";
    assert_equal_str(str1, strchr(str1, '1'), "strchr(\"12345\", '1')");
    assert_equal_str((char *)&str1[1], strchr(str1, '2'), 
        "strchr(\"12345\", '2')");
    assert_equal_str(NULL, strchr(str1, '6'), "strchr(\"12345\", '6')");

    char *str2 = "brown university";
    assert_equal_str((char *)&str2[6], strchr(str2, 'u'), 
        "strchr(\"brown university\", 'u')");

    char s[350];
    sprintf(s, "strchr(\"%s\", '.')", USEAGE);
    assert_equal_str(USEAGE, strchr(USEAGE, '.'), s);
}

// char *strcpy(char *dest, const char *src);
void test_strcpy() { }

// char *strncpy(char *dest, const char *src, size_t n);
void test_strncpy() {
    char abbreviation[5];
    strncpy(abbreviation, "professor", 3);
/*
    assert_equal_str("prof", abbreviation, 
        "strncpy(abbreviation, \"professor\", 4)");
*/
    assert_equal_char('p', abbreviation[0], 
        "strncpy(abbreviation, \"professor\", 3) first char");
    assert_equal_char('r', abbreviation[1], 
        "strncpy(abbreviation, \"professor\", 3) second char");
    assert_equal_char('o', abbreviation[2], 
        "strncpy(abbreviation, \"professor\", 3) third char");

    char buffer[10];
    strncpy(buffer, "brown", 10);
    assert_equal_str("brown", buffer, "strncpy(buffer, \"brown\", 10);");
    for (int i = 5; i < 10; ++i)
        assert_equal_char('\0', buffer[i], 
            "strncpy(buffer, \"brown\", 10) null bytes at end");

    char good_fit[5];
    strncpy(good_fit, "okay", 4);
    assert_equal_char('o', good_fit[0], 
        "strncpy(good_fit, \"brown\", 10) first char");
    assert_equal_char('k', good_fit[1], 
        "strncpy(good_fit, \"brown\", 10) second char");
    assert_equal_char('a', good_fit[2], 
        "strncpy(good_fit, \"brown\", 10) third char");
    assert_equal_char('y', good_fit[3], 
        "strncpy(good_fit, \"brown\", 10) fourth char");
}

// char *strcat(char *dest, const char *src);
void test_strcat() { }

// char *strncat(char *dest, const char *src, size_t n);
void test_strncat() { }

// char *strpbrk(const char *s, const char *accept);
void test_strpbrk() { }

// size_t strspn(const char *s, const char *accept);
void test_strspn() {
    assert_equal_int(2, strspn("professor", "rpf"), 
        "strspn(\"professor\", \"rpf\")");
    assert_equal_int(0, strspn("professor", ""), 
        "strspn(\"professor\", \"\")");

    char long_str_err[1000];
    sprintf(long_str_err, "strspn(\"%s\", \"r/.\")", SUPER_LONG_STRING);
    assert_equal_int(3, strspn(SUPER_LONG_STRING, "r/."), long_str_err);
}

// size_t strcspn(const char *s, const char *reject);
void test_strcspn() {
    assert_equal_int(0, strcspn("professor", "rpf"), 
        "strcspn(\"professor\", \"rpf\")");
    assert_equal_int(9, strcspn("professor", ""), 
        "strcspn(\"professor\", \"\")");
    assert_equal_int(4, strcspn("professor", "es"), 
        "strcspn(\"professor\", \"es\")");

    char long_str_err[1000];
    sprintf(long_str_err, "strcspn(\"%s\", \"r/.\")", SUPER_LONG_STRING);
    assert_equal_int(0, strcspn(SUPER_LONG_STRING, "r/."), long_str_err);
}

// int strcmp(const char *s1, const char *s2);
void test_strcmp() {
    assert_same_sign(-2, strcmp("professor", "rpf"), 
        "strcmp(\"professor\", \"rpf\")");
    assert_same_sign(17, strcmp("test", "case"), "strcmp(\"test\", \"case\")");
    assert_same_sign(0, strcmp("abc", "abc"), "strcmp(\"abc\", \"abc\")");
    assert_same_sign(101, strcmp("one", "on"), "strcmp(\"one\", \"on\")");
    assert_same_sign(114, strcmp("rpf", ""), "strcmp(\"rpf\", \"\")");

    char long_str_err[1000];
    char slightly_different_long_str[1000];
    sprintf(slightly_different_long_str, "x%s", USEAGE);
    sprintf(long_str_err, "strcmp(\"%s\", \"%s\")", 
        USEAGE, slightly_different_long_str);
    assert_same_sign(-74, strcmp(USEAGE, slightly_different_long_str),
        long_str_err);
}

// int strncmp(const char *s1, const char *s2, size_t n);
void test_strncmp() {
    assert_same_sign(-2, strncmp("professor", "rpf", 3), 
        "strncmp(\"professor\", \"rpf\", 3)");
    assert_same_sign(17, strncmp("test", "case", 1), 
        "strncmp(\"test\", \"case\", 1)");
    assert_same_sign(0, strncmp("abc", "abc", 5), 
        "strncmp(\"abc\", \"abc\", 5)");
    assert_same_sign(0, strncmp("asdf", "asde", 3), 
        "strncmp(\"asdf\", \"asde\", 3)");
    assert_same_sign(114, strncmp("rpf", "", 3), "strncmp(\"rpf\", \"\", 3)");
    assert_same_sign(0, strncmp("rpf", "", 0), "strncmp(\"rpf\", \"\", 0)");

    char long_str_err[750];
    char slightly_different_long_str[350];
    sprintf(slightly_different_long_str, "%sx", USEAGE);
    sprintf(long_str_err, "strncmp(\"%s\", \"%s\", 5)", 
        USEAGE, slightly_different_long_str);
    assert_same_sign(0, strncmp(USEAGE, slightly_different_long_str, 5), 
        long_str_err);
}

// char *strstr(const char *haystack, const char *needle);
void test_strstr() {
    char *eg = "eg";
    char *dg = "dg";
    char *f = "f";
    assert_equal_str("defg", strstr("abcdefg", "de"), 
        "strstr(\"abcdefg\", \"de\")");  
    assert_equal_str("babab", strstr("abbabab", "bab"), 
        "strstr(\"abbabab\", \"bab\")"); 
    assert_equal_str("eg", strstr("abcdeg", eg), 
        "strstr(\"abcdeg\", \"eg\")");
    assert_equal_str(NULL, strstr("abcdeg", dg), 
        "strstr(\"abcdeg\", \"dg\")"); 
    assert_equal_str(NULL, strstr("abcdeg", f), 
        "strstr(\"abcdeg\", \"f\")"); 
    assert_equal_str("eg", strstr("eg", eg), "strstr(\"eg\", \"eg\")"); 
}

// char *strtok(char *str, const char *delim);
void test_strtok() {
    char str[80] = "1,2,,3,4";
    const char s[2] = ",";
    assert_equal_str("1", strtok(str, s), 
        "strtok(\"1,2,,3,4\", \",\") first time"); 
    assert_equal_str("2", strtok(NULL, s), 
        "strtok(\"1,2,,3,4\", \",\") second time");
    assert_equal_str("3", strtok(NULL, s), 
        "strtok(\"1,2,,3,4\", \",\") third time"); 
    assert_equal_str("4", strtok(NULL, s), 
        "strtok(\"1,2,,3,4\", \",\") fourth time"); 
    assert_equal_str(NULL, strtok(NULL, s), 
        "strtok(\"1,2,,3,4\", \",\") fifth time");  

    char str2[80] = ";;;33;;6;;77;";
    const char d[3] = ";;";
    assert_equal_str("33", strtok(str2, d), 
        "strtok(\";;;33;;6;;77;\", \";;\") first time"); 
    assert_equal_str("6", strtok(NULL, d), 
        "strtok(\";;;33;;6;;77;\", \";;\") second time");
    assert_equal_str("77", strtok(NULL, d), 
        "strtok(\";;;33;;6;;77;\", \";;\") third time"); 
    assert_equal_str(NULL, strtok(NULL, d), 
        "strtok(\";;;33;;6;;77;\", \";;\") fourth time"); 
}

int run_test_in_separate_process(void (*func)(), 
    int num_tests, 
    const char *message) { 
    int pid, status;
    if (!(pid = fork())) {
        func();
        if (strcmp_test(message, "all")) {
            printf("%s: ", message);
            print_test_summary();
        }
        exit(0);
    }
    waitpid(pid, &status, 0);
    if (status != 0) 
        printf("%s: Passed 0 tests, Failed %d tests | 0.00% (process exited with code: %d)\n", 
            message, num_tests, status);

    return status;
}

void test_all() {
    if (should_time) {
        time_test(&test_strlen, 5, "strlen");
        time_test(&test_strspn, 3, "strspn");
        time_test(&test_strcspn, 4, "strcspn");
        time_test(&test_strncmp, 7, "strncmp");
        time_test(&test_strncpy, 7, "strncpy");
        time_test(&test_strstr, 6, "strstr");
        time_test(&test_strtok, 9, "strtok");
    } else {
        run_test_in_separate_process(&test_strlen, 5, "strlen");
        run_test_in_separate_process(&test_strspn, 3, "strspn");
        run_test_in_separate_process(&test_strcspn, 4, "strcspn");
        run_test_in_separate_process(&test_strncmp, 7, "strncmp");
        run_test_in_separate_process(&test_strncpy, 7, "strncpy");
        run_test_in_separate_process(&test_strstr, 6, "strstr");
        run_test_in_separate_process(&test_strtok, 9, "strtok");
    }
}

// func is a pointer to the function to call
// n is the number of times to repeat it
// message is what to print in front of the output
void time_test(void (*func)(), int num_tests, const char *message) {
    if (func == *test_all) {
        should_time = 1;
        test_all();
        should_time = 0;
        return;
    }

    should_time = 0;

    // don't want to print/count while testing performance
    // too many tests and the same errors over and over
    STOP_PRINTING
    STOP_COUNTING

    int pid, status;
    if (!(pid = fork())) {
        // warm up CPU+caches so timing is more consistent
        for (int i = 0; i < (num_repeat / 10); ++i)
            func();

        clock_t start = clock();
        for (int i = 0; i < num_repeat; ++i)
            func();
        int diff = 1000 * (clock() - start) / CLOCKS_PER_SEC;
        printf("%s (repeated %d times) took %ds %dms\n", 
            message, num_repeat, diff / 1000, diff % 1000);

        exit(0);
    }
    waitpid(pid, &status, 0);
    if (status != 0) 
        printf("%s (repeated %d times) took %ds %dms (process exited with code: %d)\n", 
            message, num_repeat, 0, 0, status);

    RESUME_COUNTING
    RESUME_PRINTING
}

void foreach_test(int num_tests, 
    char const *test_names[], 
    void (*wrapper)(void (*)(), int, const char *)) {
    int dummy;
    for (int i = 0; i < num_tests; ++i) {
        if (i == 0)
            continue;  // file name

        const char *test_name = test_names[i];
        if (!strcmp_test(test_name, "all"))
            wrapper(&test_all, 0, "all");
        else if (!strcmp_test(test_name, "strlen"))
            wrapper(&test_strlen, 5, "strlen");
        else if (!strcmp_test(test_name, "strchr"))
            wrapper(&test_strchr, 0, "strchr");
        else if (!strcmp_test(test_name, "strcpy"))
            wrapper(&test_strcpy, 0, "strcpy");
        else if (!strcmp_test(test_name, "strncpy"))
            wrapper(&test_strncpy, 0, "strncpy");
        else if (!strcmp_test(test_name, "strcat"))
            wrapper(&test_strcat, 0, "strcat");
        else if (!strcmp_test(test_name, "strncat"))
            wrapper(&test_strncat, 0, "strncat");
        else if (!strcmp_test(test_name, "strpbrk"))
            wrapper(&test_strpbrk, 0, "strpbrk");
        else if (!strcmp_test(test_name, "strspn"))
            wrapper(&test_strspn, 3, "strspn");
        else if (!strcmp_test(test_name, "strcspn"))
            wrapper(&test_strcspn, 4, "strcspn");
        else if (!strcmp_test(test_name, "strcmp"))
            wrapper(&test_strcmp, 0, "strcmp");
        else if (!strcmp_test(test_name, "strncmp"))
            wrapper(&test_strncmp, 5, "strncmp");
        else if (!strcmp_test(test_name, "strstr"))
            wrapper(&test_strstr, 7, "strstr");
        else if (!strcmp_test(test_name, "strtok"))
            wrapper(&test_strtok, 9, "strtok");
        else if (sscanf(test_name, "%d", &dummy) != 1)
            printf("Unknown test: %s\n", test_name);
    }
}

int main(int argc, char const *argv[]) {
    /* code */
    printf("Testing for correctness...\n");

    if (argc < 2) {
        printf("USAGE: %s\n", USEAGE);
        return 1;
    }
    if (sscanf(argv[1], "%d", &num_repeat) != 1)
        num_repeat = DEFAULT_REPEAT;

    install_seccomp();

    foreach_test(argc, argv, (void (*) ()) &run_test_in_separate_process);

    printf("\nTesting Performance...\n");
    foreach_test(argc, argv, (void (*) ()) &time_test);
    return 0;
}
