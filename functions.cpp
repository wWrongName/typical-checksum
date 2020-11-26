#include "functions.hpp"

int factorial(int number) {
    int fact = 1;
    for (int i = 2; i <= number; i++) {
        fact *= i;
    }
    return fact;
    // return (number < 0) ? 0 : ((number < 2) ? 1 : number * factorial(number - 1));
};

double count_combination(int bottom, int top) {  // n, i
    return (double)factorial(bottom) / (double)(factorial(bottom - top) * factorial(top));
};