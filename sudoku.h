//數獨（Sudoku）是源自 18 世紀瑞士的一種數學遊戲。是一種運用紙、筆進 行演算的邏輯遊戲。玩家需要根據 9×9 盤面上的已知數字，推理出所有剩餘空格的數字，並滿足每一行、每一列、每一個粗線宮（3*3）內的數字均含 1-9，不重覆。
//需遵守以下要求：
//1.	能隨機生成數獨題目。
//2.	要可以自訂大小共4*4、9*9、16*16三種模式，大於9的數字用A(10)、B(11)、 C(12)、D(13)、E(14)、F(15)、G(16)代替。
//3.	使用鍵盤I(上)J(左)K(下)L(右)鍵控制游標位置，游標顏色為藍色。
//4.	在游標該點位置輸入數字時，數字便會顯示在該格位置。若數字正確顯示綠色，錯誤則為紅色。若游標位置為題目數字非空格，則無法輸入數字。
//5.	遊戲中按下Esc鍵可以跳出遊戲回到遊戲首頁。
//6.	當所有答案正確填完時跳出congratulations。

#ifndef NYCU_SUDOKU_H
#define NYCU_SUDOKU_H

#include <vector>
#include <cassert>
#include <cmath>
#include <bitset>
#include <ncurses.h>


class Choose {
public:
    Choose(WINDOW *win, std::string *choices, int size, std::string title = "", int y = 0, int x = 0) {
        this->win = win;
        this->title = title;
        this->y = y;
        this->x = x;
        this->choices = choices;
        this->size = size;
        this->choice = 0;
    }

    std::string getSelected() const {
        return choices[choice];
    }

    void up() {
        if (choice == 0) {
            choice = size - 1;
        } else {
            choice--;
        }
    }

    void down() {
        if (choice == size - 1) {
            choice = 0;
        } else {
            choice++;
        }
    }

    void draw() {
        //highlight the choice
        for (int i = 0; i < size; i++) {
            if (i == choice) {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, y + i, x, choices[i].c_str());
            wattroff(win, A_REVERSE);
        }
        // draw the title
        mvwprintw(win, 0, x, title.c_str());
    }

private:
    WINDOW *win;
    std::string title;
    int x;
    int y;
    const std::string *choices;
    int size;
    int choice;
};

class Sudoku {
private:
    typedef std::vector<std::vector<char>> Matrix;
    int size;
    int box_size;
    Matrix matrix;

    bool generate(int row, int *candis);

    class Solution {
        int size;
    public:
        Solution(int size) {
            this->size = size;
        }

        std::bitset<9> getPossibleStatus(int x, int y) {
            return ~(rows[x] | cols[y] | cells[x / 3][y / 3]);
        }

        std::vector<int> getNext(std::vector<std::vector<char>> &board) {
            std::vector<int> ret;
            int minCnt = 10;
            for (int i = 0; i < board.size(); i++) {
                for (int j = 0; j < board[i].size(); j++) {
                    if (board[i][j] != '.') continue;
                    auto cur = getPossibleStatus(i, j);
                    if (cur.count() >= minCnt) continue;
                    ret = {i, j};
                    minCnt = cur.count();
                }
            }
            return ret;
        }

        void fillNum(int x, int y, int n, bool fillFlag) {
            rows[x][n] = fillFlag;
            cols[y][n] = fillFlag;
            cells[x / 3][y / 3][n] = fillFlag;
        }

        bool dfs(std::vector<std::vector<char>> &board, int cnt) {
            if (cnt == 0) return true;

            auto next = getNext(board);
            auto bits = getPossibleStatus(next[0], next[1]);
            for (int n = 0; n < bits.size(); n++) {
                if (!bits.test(n)) continue;
                fillNum(next[0], next[1], n, true);
                board[next[0]][next[1]] = n + '1';
                if (dfs(board, cnt - 1)) return true;
                board[next[0]][next[1]] = '.';
                fillNum(next[0], next[1], n, false);
            }
            return false;
        }

        void solveSudoku(std::vector<std::vector<char>> &board) {
            rows = std::vector<std::bitset<9 >>
                    (9, std::bitset<9>());
            cols = std::vector<std::bitset<9 >>
                    (9, std::bitset<9>());
            cells = std::vector<std::vector<std::bitset<9>>>(3, std::vector<std::bitset<9 >>
                    (3, std::bitset<9>()));

            int cnt = 0;
            for (int i = 0; i < board.size(); i++) {
                for (int j = 0; j < board[i].size(); j++) {
                    cnt += (board[i][j] == '.');
                    if (board[i][j] == '.') continue;
                    int n = board[i][j] - '1';
                    rows[i] |= (1 << n);
                    cols[j] |= (1 << n);
                    cells[i / 3][j / 3] |= (1 << n);
                }
            }
            dfs(board, cnt);
        }

    private:
        std::vector<std::bitset<9>> rows;
        std::vector<std::bitset<9>> cols;
        std::vector<std::vector<std::bitset<9>>>
                cells;
    };

public:

    Sudoku(int size);

    void print();

    void generate();

    void sovle();

    static std::vector<std::vector<chtype>> printsudoku(int size) {
        std::vector<std::vector<chtype>> ans;
        int size2 = sqrt(size);
        for (int i = 0; i < size2; i++) {
            ans.emplace_back(1, ACS_LTEE);
            for (int j = 0; j < size2; j++) {
                ans.emplace_back(1, ACS_VLINE);
            }
        }
        ans[0][0] = ACS_ULCORNER;
        ans.emplace_back(1, ACS_LLCORNER);
        for (int i = 0; i < size2; i++) {
            for (std::vector<chtype> &i: ans) {
                if (i[0] == ACS_LTEE) {
                    for (int j = 0; j < size2 * 2 + 1; j++) {
                        i.emplace_back(ACS_HLINE);
                    }
                    i.emplace_back(ACS_PLUS);
                } else if (i[0] == ACS_VLINE) {
                    for (int j = 0; j < size2 * 2 + 1; j++) {
                        i.emplace_back(' ');
                    }
                    i.emplace_back(ACS_VLINE);
                } else if (i[0] == ACS_LLCORNER) {
                    for (int j = 0; j < size2 * 2 + 1; j++) {
                        i.emplace_back(ACS_HLINE);
                    }
                    i.emplace_back(ACS_BTEE);
                } else if (i[0] == ACS_ULCORNER) {
                    for (int j = 0; j < size2 * 2 + 1; j++) {
                        i.emplace_back(ACS_HLINE);
                    }
                    i.emplace_back(ACS_TTEE);
                }
            }
        }
        for (std::vector<chtype> &i: ans) {
            if (i.back() == ACS_PLUS) {
                i.pop_back();
                i.emplace_back(ACS_RTEE);
            } else if (i.back() == ACS_BTEE) {
                i.pop_back();
                i.emplace_back(ACS_LRCORNER);
            } else if (i.back() == ACS_TTEE) {
                i.pop_back();
                i.emplace_back(ACS_URCORNER);
            }
        }
        return ans;
    }
};

Sudoku::Sudoku(int size) {
    assert(size == 4 || size == 9 || size == 16);
    this->size = size;
    this->box_size = sqrt(size);
    this->matrix = Matrix(size, std::vector<char>(size, ' '));
}

bool Sudoku::generate(int row, int *candis) {
    if (row == size)
        return true;
    for (int i = 0; i < size; i++) {
        if (candis[i] == 0)
            continue;
        matrix[row][i] = candis[i] + '0';
        candis[i] = 0;
        if (generate(row + 1, candis))
            return true;
        candis[i] = matrix[row][i] - '0';
        matrix[row][i] = ' ';
    }
    return false;
}

#endif //NYCU_SUDOKU_H

