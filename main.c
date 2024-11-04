#include <stdio.h>
#include "lib/calc.h"

int main() {
    double a = 10.0, b = 5.0;

    printf("덧셈: %.2f + %.2f = %.2f\n", a, b, add(a, b));
    printf("뺄셈: %.2f - %.2f = %.2f\n", a, b, subtract(a, b));
    printf("곱셈: %.2f * %.2f = %.2f\n", a, b, multiply(a, b));
    printf("나눗셈: %.2f / %.2f = %.2f\n", a, b, divide(a, b));

    return 0;
}
