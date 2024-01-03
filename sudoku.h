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
#include <cmath>
#include <bitset>
#include <ncursesw/ncurses.h>
#include <fstream>
#include <cassert>


typedef std::vector<std::vector<char>> Matrix;

void init_ncurses() {
    setlocale(LC_ALL, "");
    system("sleep 0.1");
    initscr();
    cbreak();
    noecho();
    timeout(50);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
}

void end_ncurses() {
    clear();
    refresh();
    curs_set(1);
    endwin();
}

void print_asciiart(WINDOW *win, int y, int x, const std::string &file) {
    std::ifstream f(file);
    std::string line;
    while (getline(f, line)) {
        mvwprintw(win, y++, x, "%s", line.c_str());
    }
    f.close();
}

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
        if (choice > 0) {
            choice--;
        }
    }

    void down() {
        if (choice < size - 1) {
            choice++;
        }
    }

    void draw() {
        //highlight the choice
        for (int i = 0; i < size; i++) {
            if (i == choice) {
                wattron(win, A_REVERSE);
            }
            mvwprintw(win, y + i, x, "%s", choices[i].c_str());
            wattroff(win, A_REVERSE);
        }
        // draw the title
        mvwprintw(win, 0, x, "%s", title.c_str());
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
public:
    Sudoku(int size);

    void setMatrix(Matrix &matrix);

    void generate();

    void sovle();

    void drawsudoku(WINDOW *win, int y, int center_x);

    void cursor_up();

    void cursor_down();

    void cursor_left();

    void cursor_right();

    void toggle();

    void input(char c);

    void remove();

    bool check_finish();

private:
    int size;
    int box_size;
    Matrix matrix;
    int cursor_x;
    int cursor_y;
    bool input_mode;

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
};

Sudoku::Sudoku(int size) {
    assert(size == 4 || size == 9 || size == 16);
    this->size = size;
    this->box_size = sqrt(size);
    this->matrix = Matrix(size, std::vector<char>(size, '.'));
    this->cursor_x = 2;
    this->cursor_y = 2;
    this->input_mode = false;
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

void Sudoku::drawsudoku(WINDOW *win, int y, int center_x) {
    {
        std::vector<std::wstring> ans;
        int curs_y = -1, curs_x = -1;
        for (int i = 0; i < box_size; i++) {
            ans.emplace_back(1, L'┣');
            for (int j = 0; j < box_size; j++)
                ans.emplace_back(1, L'┃');
        }
        ans[0][0] = L'┏';
        ans.emplace_back(1, L'┗');
        for (int i = 0; i < box_size; i++) {
            int pr_y = 0;
            for (int ii = 0; ii < ans.size(); ii++) {
                std::wstring &s = ans[ii];
                if (s[0] == L'┣') {
                    s.append(box_size * 2 + 1, L'━');
                    s.push_back(L'╋');
                } else if (s[0] == L'┃') {
                    s.push_back(' ');
                    for (int j = 0; j < box_size; j++) {
                        int pr_x = i * box_size + j;
                        if (matrix[pr_y][pr_x] != '.')
                            s.push_back(matrix[pr_y][pr_x]);
                        else
                            s.push_back('_');
                        if (pr_y == cursor_y && pr_x == cursor_x) {
                            curs_y = ii;
                            curs_x = s.size() - 1;
                        }
                        s.push_back(' ');
                    }
                    pr_y++;
                    s.push_back(L'┃');
                } else if (s[0] == L'┗') {
                    s.append(box_size * 2 + 1, L'━');
                    s.push_back(L'┻');
                } else if (s[0] == L'┏') {
                    for (int j = 0; j < box_size * 2 + 1; j++) {
                        s.push_back(L'━');
                    }
                    s.push_back(L'┳');
                }
            }
        }
        for (std::wstring &i: ans) {
            if (i.back() == L'╋') {
                i.pop_back();
                i.push_back(L'┫');
            } else if (i.back() == L'┻') {
                i.pop_back();
                i.push_back(L'┛');
            } else if (i.back() == L'┳') {
                i.pop_back();
                i.push_back(L'┓');
            }
        }
        for (int i = 0; i < ans.size(); i++) {
            for (int j = 0; j < ans[i].size(); j++) {
                if (i == curs_y && j == curs_x) {
                    if (input_mode)
                        wattron(win, COLOR_PAIR(2));
                    else
                        wattron(win, COLOR_PAIR(1));
                }
                mvaddwstr(i + y, j + center_x - ans[i].size() / 2, &ans[i][j]);
                if (i == curs_y && j == curs_x) {
                    if (input_mode)
                        wattroff(win, COLOR_PAIR(2));
                    else
                        wattroff(win, COLOR_PAIR(1));
                }
            }
        }
    }
}

void Sudoku::setMatrix(Matrix &matrix) {
    this->matrix = matrix;
}

void Sudoku::cursor_up() {
    if (!input_mode && cursor_y > 0)
        cursor_y--;
}

void Sudoku::cursor_down() {
    if (!input_mode && cursor_y < size - 1)
        cursor_y++;
}

void Sudoku::cursor_left() {
    if (!input_mode && cursor_x > 0)
        cursor_x--;
}

void Sudoku::cursor_right() {
    if (!input_mode && cursor_x < size - 1)
        cursor_x++;
}

void Sudoku::toggle() {
    input_mode = !input_mode;
}

bool Sudoku::check_finish() {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (matrix[i][j] == '.')
                return false;
        }
    return true;
}

void Sudoku::input(char c) {
    if (size <= 9) {
        if (!(c >= '1' && c <= '9')) {
            return;
        }
    } else {
        if (!((c >= '1' && c <= '9') || (c >= 'A' && c <= 'G') || (c >= 'a' && c <= 'g'))) {
            return;
        }
    }
    if (c >= 'a' && c <= 'g')
        c = c - 'a' + 'A';
    if (input_mode && matrix[cursor_y][cursor_x] == '.') {
        matrix[cursor_y][cursor_x] = (char)c;
    }
}

#endif //NYCU_SUDOKU_H

