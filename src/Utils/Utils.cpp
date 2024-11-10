#include <stdlib.h>
#include <time.h>

// Initializes the random number generator. Call this once at the start of your program.
void initRandom() {
    srand((unsigned int)time(NULL));
}

// Generates a random number in the range [min, max]
int randInRange(int min, int max) {
    if (min > max) {
        // Handle invalid input
        int temp = min;
        min = max;
        max = temp;
    }
    return (rand() % (max - min + 1)) + min;
}
