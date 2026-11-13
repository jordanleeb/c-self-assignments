/*
 * Reads a count (1-100) and that many integer scores (0-100), then prints
 * the average, highest, lowest, sorted scores, and median.
 * Invalid input is rejected with an error message and re-prompted.
 * If input ends (EOF) before all values are read, exits with failure.
 */
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// min / max bounds for number of scores
#define COUNT_MIN 1
#define COUNT_MAX 100

// min / max bounds for entered scores
#define SCORE_MIN 0
#define SCORE_MAX 100

// size of the input buffer
#define INPUT_BUFFER_SIZE 64

typedef enum {
    READ_OK,       // read succeeded
    READ_EOF,      // input stream closed / error
    READ_TOO_LONG, // line exceeded buffer
    READ_INVALID,  // not a number, or trailing junk
    READ_RANGE     // valid number, outside [min, max]
} read_status;

static read_status read_line(char *input_buffer, size_t size) {
    // receive input to input buffer
    if (!fgets(input_buffer, size, stdin))
        return READ_EOF;

    // replace new line if it exists with \0
    char *new_line = strchr(input_buffer, '\n');
    if (new_line) {
        *new_line = '\0';
        return READ_OK;
    }

    // new line doesn't exist, already OK
    if (feof(stdin))
        return READ_OK;

    // clear stdin
    int  c;
    bool discarded = false;
    while ((c = getchar()) != '\n' && c != EOF)
        discarded = true;

    return discarded ? READ_TOO_LONG : READ_OK;
}

static read_status read_int(int *out, int min, int max) {
    char input_buffer[INPUT_BUFFER_SIZE];

    // receive user input into buffer
    read_status read_line_status = read_line(input_buffer, sizeof(input_buffer));
    if (read_line_status != READ_OK)
        return read_line_status;

    // convert buffer to an integer
    char *end;
    errno  = 0;
    long v = strtol(input_buffer, &end, 10);

    if (end == input_buffer)
        return READ_INVALID;

    // skip whitespace
    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0')
        return READ_INVALID;
    if (errno == ERANGE || v < min || v > max)
        return READ_RANGE;

    *out = (int)v;
    return READ_OK;
}

static void selection_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        // assume current position holds minimum element
        int min_idx = i;

        // find actual minimum
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }

        // swap minimum element into proper position
        int temp     = arr[i];
        arr[i]       = arr[min_idx];
        arr[min_idx] = temp;
    }
}

static double get_avg(int arr[], int n) {
    int sum = 0;

    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    double avg = (double)sum / (double)n;

    return avg;
}

static double get_med(int arr[], int n) {
    double med = 0.0;

    int mid = n / 2;

    if (n % 2) {
        // n is odd
        med = (double)arr[mid];
    } else {
        // n is even
        med = (arr[mid - 1] + arr[mid]) / 2.0;
    }

    return med;
}

// returns true if a valid value is stored in *out
// returns false if input stream is closed (EOF)
static bool prompt_int(int *out, int min, int max, const char *fmt, ...) {
    for (;;) {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);

        read_status status = read_int(out, min, max);
        switch (status) {
            case READ_OK:
                return true;
            case READ_EOF:
                fprintf(stderr, "\nError: input stream closed.\n");
                return false;
            case READ_TOO_LONG:
                fprintf(stderr, "Error: input exceeded buffer size.\n");
                break;
            case READ_INVALID:
                fprintf(stderr, "Error: please enter a valid number.\n");
                break;
            case READ_RANGE:
                fprintf(stderr, "Error: please enter a valid number between %d and %d.\n", min,
                        max);
                break;
        }
    }
}

int main(void) {
    int scores[COUNT_MAX]; // array for holding scores
    int num_scores = 0;    // number of scores to receive and process from user

    // read number of scores from user
    if (!prompt_int(&num_scores, COUNT_MIN, COUNT_MAX, "How many scores? "))
        return EXIT_FAILURE;

    // read scores from user
    for (int i = 0; i < num_scores; i++) {
        if (!prompt_int(&scores[i], SCORE_MIN, SCORE_MAX, "Score %d: ", i + 1))
            return EXIT_FAILURE;
    }

    // sort scores using selection sort
    selection_sort(scores, num_scores);

    // display statistics
    printf("Average: %.2f\n", get_avg(scores, num_scores));

    printf("Highest: %d\n", scores[num_scores - 1]);

    printf("Lowest: %d\n", scores[0]);

    printf("Sorted: ");
    for (int i = 0; i < num_scores - 1; i++) {
        printf("%d ", scores[i]);
    }
    printf("%d\n", scores[num_scores - 1]);

    printf("Median: %.1f\n", get_med(scores, num_scores));

    return EXIT_SUCCESS;
}
