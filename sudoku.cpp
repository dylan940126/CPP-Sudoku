#include <iostream>
#include "sudoku.h"
#include <unistd.h>

using namespace std;

int main() {
//    double i = getTime();
    wifstream fin("sudoku.txt", ios::binary);
    if (!fin) {
        cout << "File not found" << endl;
        return 0;
    }
    wstring i;
    fin.imbue(locale(fin.getloc(), new codecvt_utf8<wchar_t>));
    while (fin >> i) {
        wcout << i << endl;
    }
}