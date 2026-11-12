/*
 * Reads a temperature in Celsius and prints it in Fahrenheit.
 * Input must be a finite number with nothing after it. Anything else
 * (letters, trailing characters, nan, inf, out-of-range values) is
 * rejected with an error message and the program exits with failure.
 */
#include <math.h>
#include <stdio.h>

#define F_PER_C  (9.0 / 5.0)
#define F_OFFSET 32.0

int main(void) {
    double celsius = 0.0;

    printf("Enter a temperature in Celsius: ");
    int scanf_result = scanf("%lf", &celsius);

    if (scanf_result != 1 || !isfinite(celsius)) {
        fprintf(stderr, "Error: that is not a valid number.\n");
        return 1;
    }

    double fahrenheit = celsius * F_PER_C + F_OFFSET;

    if (!isfinite(fahrenheit)) {
        fprintf(stderr, "Error: result too large.\n");
        return 1;
    }

    printf("%.1f C = %.1f F\n", celsius, fahrenheit);
    return 0;
}