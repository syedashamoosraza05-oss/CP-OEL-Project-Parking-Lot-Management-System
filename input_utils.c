#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "input_utils.h"


/* Reads an integer in [min,max]; keeps asking until valid. */
int readInt(const char prompt[], int min, int max) {
    int val, result, c;

    while (1) {
        printf("%s", prompt);
        result = scanf("%d", &val);

        /* throw away anything left on the line */
        while ((c = getchar()) != '\n' && c != EOF);

        if (result == EOF) exit(1);

        if (result == 1 && val >= min && val <= max)
            return val;

        printf(RED "  Invalid input. Enter a number between %d and %d.\n" RESET, min, max);
    }
}

/* Reads a non-empty string (max size-1 chars) into buf. */
void readString(const char prompt[], char buf[], int size) {
    int len;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, size, stdin) == NULL) exit(1);

        len = strlen(buf);

        /* remove the Enter key (newline) at the end */
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
            len--;
        }
        if (len > 0) return;
        printf(RED "  Input cannot be empty.\n" RESET);
    }
}

/* Converts a string to upper case (for number plates). */
void toUpperStr(char *s) {
    for (int i = 0; s[i]; i++) s[i] = (char)toupper((unsigned char)s[i]);
}
