#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include<iomanip>

using namespace std;
const int MaxSize = 16; // maximum array size
bool create_sudoku(int sudoku[MaxSize][MaxSize], int sudoku_size);

void print(int sudoku[MaxSize][MaxSize], int sudoku_size);

bool isSafe(int sudoku[MaxSize][MaxSize], int row, int col, int num, int sudoku_size);

bool create_sudoku(int sudoku[MaxSize][MaxSize], int sudoku_size) {
    int row = -1;
    int col = -1;
    bool isEmpty = true;

    // find the unfilled blank
    for (int i = 0; i < sudoku_size; ++i) {
        for (int j = 0; j < sudoku_size; ++j) {
            if (sudoku[i][j] == 0) {
                row = i;
                col = j;
                isEmpty = false;
                break;
            }
        }
        if (!isEmpty) {
            break;
        }
    }

    // if the sudoku is filled
    if (isEmpty) {
        return true;
    }

    vector<int> nums(sudoku_size);
    for (int i = 0; i < sudoku_size; ++i) {
        nums[i] = i + 1;
    }
    random_shuffle(nums.begin(), nums.end());

    for (int i = 0; i < nums.size(); i++) {
        int num = nums[i];
        if (isSafe(sudoku, row, col, num, sudoku_size)) {
            sudoku[row][col] = num;
            if (create_sudoku(sudoku, sudoku_size)) {
                return true;
            }
            sudoku[row][col] = 0;
        }
    }

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
    vector<vector<int>> index_status(sudoku_size, vector<int>(sudoku_size, 0));
    srand(time(NULL));
    for (int i = 0; i < sudoku_size; i++) {
        for (int j = 0; j < sudoku_size; j++) {
            float x = rand() / (float) RAND_MAX;
            if (x > rate_of_blank)
                index_status[i][j] = 1;
            else
                index_status[i][j] = 0;
        }
    }
    return index_status;
}