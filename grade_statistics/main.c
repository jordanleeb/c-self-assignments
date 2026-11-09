/* Reports statistics on a set of test scores.                         */
/* Receives a number of scores to be read followed by a set of scores. */
/* Outputs the set of score's average, highest, lowest, median, and    */
/* a sorted version of the set of scores.                              */
/* Input scores must be a valid integer, and must be between SCORE_MIN */
/* and SCORE_MAX.                                                      */
/* The number of scores must also be between MIN_SCORES and MAX_SCORES.*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

// min / max bounds for number of scores
#define MIN_SCORES 1
#define MAX_SCORES 100

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
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return READ_TOO_LONG;
}

static read_status read_int(int *out, int min, int max) {
    char input_buffer[INPUT_BUFFER_SIZE];

    // receive user input into buffer
    read_status read_line_status = read_line(input_buffer, sizeof(input_buffer));
    if (read_line_status != READ_OK)
        return read_line_status;

    // convert buffer to an integer
    char *end;
    errno = 0;
    long v = strtol(input_buffer, &end, 10);

    if (end == input_buffer)
        return READ_INVALID;

    // skip whitespace
    while (isspace((unsigned char)*end)) end++;


    if (*end != '\0') return READ_INVALID;
    if (errno == ERANGE || v < min || v > max)
        return READ_RANGE;

    *out = (int)v;
    return READ_OK;
}

void selection_sort(int arr[], int n) {
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
        int temp = arr[i];
        arr[i] = arr[min_idx];
        arr[min_idx] = temp;
    } 
}

double get_avg(int arr[], int n) {
    int sum = 0;

    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    double avg = (double) sum / (double) n;

    return avg;
}

double get_med(int arr[], int n) {
    double med = 0.0;

    if (n % 2) {
        // n is odd
        med = (double) arr[n/2];
    } else {
        // n is even
        med = (double ) (arr[n/2 - 1] + arr[n/2]) / 2;
    }

    return med;
}

int main(void) {
    int scores[MAX_SCORES]; // array for holding scores
    int num_scores     = 0; // number of scores to receive and process from user
    int curr_score_idx = 0; // current index in scores array to receive input for

    // read number of scores from user
    while (!num_scores) {
        printf("How many scores? ");
        read_status num_scores_status = read_int(&num_scores, MIN_SCORES, MAX_SCORES);

        switch (num_scores_status) {
            case READ_OK:
                break;
            case READ_EOF:
                fprintf(stderr, "Error: input stream closed.\n");
                return EXIT_FAILURE;
            case READ_TOO_LONG:
                fprintf(stderr, "Error: input exceeded buffer size.\n");
                break;
            case READ_INVALID:
                fprintf(stderr, "Error: please enter a valid number of scores.\n");
                break;
            case READ_RANGE:
                fprintf(stderr, "Error: please enter a valid number of scores between %d and %d.\n", MIN_SCORES, MAX_SCORES);
                break;
        }
    }

    // read scores from user
    while (curr_score_idx < num_scores) {
        printf("Score %d: ", curr_score_idx + 1);
        read_status curr_score_status = read_int(&scores[curr_score_idx], SCORE_MIN, SCORE_MAX);

        switch (curr_score_status) {
            case READ_OK:
                curr_score_idx++;
                break;
            case READ_EOF:
                fprintf(stderr, "Error: input stream closed.\n");
                return EXIT_FAILURE;
            case READ_TOO_LONG:
                fprintf(stderr, "Error: input exceeded buffer size.\n");
                break;
            case READ_INVALID:
                fprintf(stderr, "Error: please enter a valid score.\n");
                break;
            case READ_RANGE:
                fprintf(stderr, "Error: please enter a valid score between %d and %d.\n", SCORE_MIN, SCORE_MAX);
                break;
        }
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

    return 0;
}