#include <iostream>
#include "sudoku.h"
#include <unistd.h>

using namespace std;

int main() {
    double i = getTime();
    while (true) {
        cout << getTime()-i << endl;
        usleep(100000);
    }
}