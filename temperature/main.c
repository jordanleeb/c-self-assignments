/*
 * Reads a temperature in Celsius and prints it in Fahrenheit.
 * Input must be a finite number with nothing after it. Anything else
 * (letters, trailing characters, nan, inf, out-of-range values) is
 * rejected with an error message and the program exits with failure.
 */
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// constants for Celsius to Fahrenheit conversion
#define F_PER_C  (9.0 / 5.0)
#define F_OFFSET 32.0

// size of the input buffer
#define INPUT_BUFFER_SIZE 64

typedef enum {
    READ_OK,       // read succeeded
    READ_EOF,      // input stream closed / error
    READ_TOO_LONG, // line exceeded buffer
    READ_INVALID,  // not a number, or trailing junk
    READ_RANGE     // out of range for the type
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

static read_status read_double(double *out) {
    char input_buffer[INPUT_BUFFER_SIZE];

    // receive user input into buffer
    read_status read_line_status = read_line(input_buffer, sizeof(input_buffer));
    if (read_line_status != READ_OK)
        return read_line_status;

    // convert buffer to a double
    char *end;
    errno    = 0;
    double v = strtod(input_buffer, &end);

    // no valid number entered check
    if (end == input_buffer)
        return READ_INVALID;

    // skip whitespace
    while (isspace((unsigned char)*end))
        end++;

    // trailing garbage check
    if (*end != '\0')
        return READ_INVALID;

    // overflow / infinite check
    if (!isfinite(v))
        return errno == ERANGE ? READ_RANGE : READ_INVALID;

    // write value back
    *out = v;

    return READ_OK;
}

int main(void) {
    double celsius = 0.0;

    printf("Enter a temperature in Celsius: ");
    read_status status = read_double(&celsius);
    switch (status) {
        case READ_OK:
            break;
        case READ_EOF:
            fprintf(stderr, "\nError: input stream closed.\n");
            return EXIT_FAILURE;
        case READ_TOO_LONG:
            fprintf(stderr, "Error: input exceeded buffer size.\n");
            return EXIT_FAILURE;
        case READ_INVALID:
            fprintf(stderr, "Error: please enter a valid number.\n");
            return EXIT_FAILURE;
        case READ_RANGE:
            fprintf(stderr, "Error: number is out of range.\n");
            return EXIT_FAILURE;
    }

    double fahrenheit = celsius * F_PER_C + F_OFFSET;

    if (!isfinite(fahrenheit)) {
        fprintf(stderr, "Error: result too large.\n");
        return EXIT_FAILURE;
    }

    printf("%.1f C = %.1f F\n", celsius, fahrenheit);

    return EXIT_SUCCESS;
}
