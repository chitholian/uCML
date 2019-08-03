#include "natives.hpp"
#include <cstdio>

extern "C" void printi(long long num) {
    printf("%lld\n", num);
}

extern "C" void printd(double num) {
    printf("%lf\n", num);
}
