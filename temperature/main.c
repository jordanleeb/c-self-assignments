/* Converts a temperature from Celsius to Fahrenheit.           */
/* Displays an error for invalid input and overflowing outputs. */
#include <stdio.h>
#include <math.h>

int main(void) {
    double celsius = 0.0;

    printf("Enter a temperature in Celsius: ");
    int scanf_result = scanf("%lf", &celsius);

    if (scanf_result != 1 || !isfinite(celsius)) {
        fprintf(stderr, "Error: that is not a valid number.\n");
        return 1;
    }

    double fahrenheit = celsius * 9.0/5.0 + 32.0;

    if (!isfinite(fahrenheit)) {
        fprintf(stderr, "Error: result too large.\n");
        return 1;
    }

    printf("%.1f C = %.1f F\n", celsius, fahrenheit);
    return 0;
}