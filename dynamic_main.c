#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *handle;
    double (*add)(double, double);
    double (*subtract)(double, double);
    double (*multiply)(double, double);
    double (*divide)(double, double);

    handle = dlopen("./libcalc.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    add = dlsym(handle, "add");
    subtract = dlsym(handle, "subtract");
    multiply = dlsym(handle, "multiply");
    divide = dlsym(handle, "divide");

    double a = 10.0, b = 5.0;
    printf("덧셈: %.2f + %.2f = %.2f\n", a, b, add(a, b));
    printf("뺄셈: %.2f - %.2f = %.2f\n", a, b, subtract(a, b));
    printf("곱셈: %.2f * %.2f = %.2f\n", a, b, multiply(a, b));
    printf("나눗셈: %.2f / %.2f = %.2f\n", a, b, divide(a, b));

    dlclose(handle);
    return 0;
}
