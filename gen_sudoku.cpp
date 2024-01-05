#include <random>
#include <algorithm>

using namespace std;
const int MaxSize = 16; // maximum array size

bool isSafe(int sudoku[MaxSize][MaxSize], int row, int col, int num, int sudoku_size);

bool create_sudoku(int sudoku[MaxSize][MaxSize], int sudoku_size, int n = 0) {
    if (n == sudoku_size * sudoku_size) {
        return true;
    }
    int y = n / sudoku_size;
    int x = n % sudoku_size;
    if (sudoku[y][x] != 0) // skip filled cells
        return create_sudoku(sudoku, sudoku_size, n + 1);
    vector<int> nums(sudoku_size);
    for (int i = 0; i < sudoku_size; ++i) {
        nums[i] = i + 1;
    }
    shuffle(nums.begin(), nums.end(), std::mt19937(std::random_device()()));

    for (int num: nums) {
        if (isSafe(sudoku, y, x, num, sudoku_size)) {
            sudoku[y][x] = num;
            if (create_sudoku(sudoku, sudoku_size, n + 1)) {
                return true;
            }
        }
    }
    sudoku[y][x] = 0; // reset on backtrack
    return false;
}

//----------------------------------------------------------------------------------
bool isSafe(int sudoku[MaxSize][MaxSize], int row, int col, int num, int sudoku_size) {
    // check if there is same value in the same col and row
    for (int x = 0; x < sudoku_size; ++x) {
        if (sudoku[row][x] == num || sudoku[x][col] == num) {
            return false;
        }
    }

    // check if there is same value in the same square
    int sqrtSize = sqrt(sudoku_size);
    int startRow = row - row % sqrtSize;
    int startCol = col - col % sqrtSize;
    for (int i = 0; i < sqrtSize; ++i) {
        for (int j = 0; j < sqrtSize; ++j) {
            if (sudoku[i + startRow][j + startCol] == num) {
                return false;
            }
        }
    }

    return true;
}


vector<vector<int>> generate_blank(int sudoku_size, double rate_of_blank) {
    int blank_num = sudoku_size * sudoku_size * rate_of_blank, x, y;
    vector<vector<int>> index_status(sudoku_size, vector<int>(sudoku_size, 0));
    srand(time(nullptr));
    for (int i = 0; i < blank_num; i++) {
        do {
            x = rand() % sudoku_size;
            y = rand() % sudoku_size;
        } while (index_status[x][y] == -1);
        index_status[x][y] = -1;
    }
    return index_status;
}