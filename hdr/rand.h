#pragma once

#include <time.h>
#include <stdlib.h>

void simple_set_srand(void) {
    struct timespec t;
    timespec_get(&t, TIME_UTC);
    srand(t.tv_nsec);
}

unsigned int simple_rand_max(int max) {
    return rand() % max;
}