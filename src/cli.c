#include "cli.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_RESET   "\x1b[0m"

static int col_w[2] = {0, 0};

static bool use_color() {
    return isatty(STDOUT_FILENO);
}

static const char* colorForAlgorithm(const char* alg) {
    if (strcmp(alg, "kmp") == 0)           return ANSI_BLUE;
    else if (strcmp(alg, "bm") == 0)       return ANSI_YELLOW;
    else if (strcmp(alg, "sa") == 0
          || strcmp(alg, "shiftand") == 0) return ANSI_MAGENTA;
    else                                    return ANSI_RESET;
}

void printTableHeader(const char **cols, int nCols) {
    for (int i = 0; i < nCols; i++) {
        int len = (int)strlen(cols[i]);
        col_w[i] = len;
    }

    printf("|");
    for (int i = 0; i < nCols; i++) {
        printf(" %-*s |", col_w[i], cols[i]);
    }
    printf("\n|");

    for (int i = 0; i < nCols; i++) {
        int w = col_w[i] + 2;
        for (int j = 0; j < w; j++) printf("-");
        printf("|");
    }
    printf("\n");
}

void printTableRow(const char **cells, int nCols) {
    (void)nCols;

    if (use_color()) {
        const char *c_alg = colorForAlgorithm(cells[0]);
        const char *c_pos = ANSI_GREEN;

        printf("| %s%-*s%s | %s%*s%s |\n",
            c_alg, col_w[0], cells[0], ANSI_RESET,
            c_pos, col_w[1], cells[1], ANSI_RESET
        );
    } else {
        printf("| %-*s | %*s |\n",
            col_w[0], cells[0],
            col_w[1], cells[1]
        );
    }
}

void printMatch(size_t position, const char *algorithm) {
    if (use_color()) {
        const char *c_alg = colorForAlgorithm(algorithm);
        printf("%s✓ %s%s at %s%zu%s\n",
               ANSI_RESET,
               c_alg, algorithm,
               ANSI_GREEN, position,
               ANSI_RESET);
    } else {
        printf("✓ %s at %zu\n", algorithm, position);
    }
}

void printError(const char *msg) {
    if (use_color()) {
        fprintf(stderr, ANSI_RED "Error: %s" ANSI_RESET "\n", msg);
    } else {
        fprintf(stderr, "Error: %s\n", msg);
    }
}