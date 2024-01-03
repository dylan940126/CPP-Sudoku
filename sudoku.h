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
#include <algorithm>
#include <codecvt>
#include <ctime>
#include "gen_sudoku.cpp"

typedef std::vector<std::vector<char>> Matrix;

inline std::wstring to_wide_string(const std::string &input) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input);
}

inline std::string to_byte_string(const std::wstring &input) {
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}

void init_ncurses() {
    setlocale(LC_ALL, "");
    system("sleep 0.1");
    initscr();
    cbreak();
    noecho();
    timeout(20);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_BLUE, COLOR_YELLOW);
    init_pair(2, COLOR_BLUE, COLOR_CYAN);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
}

void end_ncurses() {
    clear();
    refresh();
    curs_set(1);
    endwin();
}

bool draw_asciiart(WINDOW *win, int y, int x, const std::string &file) {
    if (x > getmaxx(win) || y > getmaxy(win))
        return false;
    bool flag = false;
    std::ifstream f(file);
    std::string line;
    std::wstring line2;
    int xx;
    while (y < 0)
        y++, getline(f, line);
    while (y <= getmaxy(win) && getline(f, line)) {
        line2 = to_wide_string(line);
        if (x < 0) {
            if (-x > line2.size()) {
                y++;
                continue;
            }
            line2 = line2.substr(-x);
            xx = 0;
        } else {
            xx = x;
        }
        if (xx + line2.size() > getmaxx(win)) {
            line2 = line2.substr(0, getmaxx(win) - xx);
        }
        if (line2.size() > 0)
            flag = true;
        mvwprintw(win, y++, xx, "%s", to_byte_string(line2).c_str());
    }
    f.close();
    return flag;
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
        this->choice = 1;
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
    Sudoku() = default;

    Sudoku(int size);

    Sudoku(std::string filename);

    int get_size();

    void generate_all();

    void generate(string difficulty);

    void drawsudoku(WINDOW *win, int y, int center_x);

    void cursor_up();

    void cursor_down();

    void cursor_left();

    void cursor_right();

    void input(wchar_t c);

    void remove();

    void hint();

    bool finished();

private:
    int size;
    int box_size;
    Matrix matrix;
    int cursor_x;
    int cursor_y;
    vector<vector<int>> user_input;//0 for hide, 1 for show, 2 for input correct, 3 for input wrong
};

Sudoku::Sudoku(int size) {
    assert(size == 4 || size == 9 || size == 16);
    this->size = size;
    this->box_size = sqrt(size);
    this->matrix = Matrix(size, std::vector<char>(size, '.'));
    this->cursor_x = 2;
    this->cursor_y = 2;
}

Sudoku::Sudoku(std::string filename) {
    ifstream fin;
    fin.open(filename);
    if (!fin) {
        cout << "Error: file not found" << endl;
        return;
    }
    string s;
    char c;
    while (fin >> c)
        s += c;
    size = sqrt(s.size());
    assert(size == 4 || size == 9 || size == 16);
    this->size = size;
    this->box_size = sqrt(size);
    this->matrix = Matrix(size, std::vector<char>(size, '.'));
    this->cursor_x = 2;
    this->cursor_y = 2;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = s[i * size + j];
}

void Sudoku::drawsudoku(WINDOW *win, int y, int center_x) {
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
                    switch (user_input[pr_y][pr_x]) {
                        case -1:
                            s.push_back('_');
                            break;
                        case 0:
                            s.push_back(matrix[pr_y][pr_x]);
                            break;
                        default:
                            s.push_back(user_input[pr_y][pr_x]);
                            break;
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
    int pr_y = 0, pr_x = 0, cursor_num = -1;
    if (user_input[cursor_y][cursor_x] == 0)
        cursor_num = matrix[cursor_y][cursor_x];
    else
        cursor_num = user_input[cursor_y][cursor_x];
    for (int i = 0; i < ans.size(); i++) {
        for (int j = 0; j < ans[i].size(); j++) {
            if (ans[i][j] >= 'A' && ans[i][j] <= 'G' || ans[i][j] >= '0' && ans[i][j] <= '9' || ans[i][j] == '_') {
                if (pr_x == size)
                    pr_x = 0, pr_y++;
                switch (user_input[pr_y][pr_x]) {
                    case -1:
                        break;
                    case 0:
                        wattron(win, COLOR_PAIR(0));
                        break;
                    default:
                        if (user_input[pr_y][pr_x] == matrix[pr_y][pr_x]) {
                            wattron(win, COLOR_PAIR(3));
                        } else
                            wattron(win, COLOR_PAIR(4));
                        break;
                }
                if (ans[i][j] == cursor_num)
                    wattron(win, COLOR_PAIR(2));
                if (pr_x == cursor_x && pr_y == cursor_y)
                    wattron(win, COLOR_PAIR(1));
                pr_x++;
            }
            mvwaddwstr(win, i + y, j + center_x - ans[i].size() / 2, &ans[i][j]);
            wattroff(win, COLOR_PAIR(0));
            wattroff(win, COLOR_PAIR(1));
            wattroff(win, COLOR_PAIR(2));
            wattroff(win, COLOR_PAIR(3));
            wattroff(win, COLOR_PAIR(4));
        }
    }
}

void Sudoku::cursor_up() {
    if (cursor_y > 0)
        cursor_y--;
}

void Sudoku::cursor_down() {
    if (cursor_y < size - 1)
        cursor_y++;
}

void Sudoku::cursor_left() {
    if (cursor_x > 0)
        cursor_x--;
}

void Sudoku::cursor_right() {
    if (cursor_x < size - 1)
        cursor_x++;
}

bool Sudoku::finished() {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (user_input[i][j] != 0 && user_input[i][j] != matrix[i][j])
                return false;
        }
    return true;
}

void Sudoku::input(wchar_t c) {
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
    if (!(user_input[cursor_y][cursor_x] == 0 || user_input[cursor_y][cursor_x] == matrix[cursor_y][cursor_x])) {
        user_input[cursor_y][cursor_x] = c;
    }
}

void Sudoku::generate_all() {
    srand(time(NULL));
    int sudoku[MaxSize][MaxSize] = {0};
    create_sudoku(sudoku, size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (sudoku[i][j] > 9)
                matrix[i][j] = 'A' + sudoku[i][j] - 10;
            else
                matrix[i][j] = '0' + sudoku[i][j];
        }
}

void Sudoku::generate(string difficulty) {
    double rate_of_blank;
    if (difficulty == "Easy")
        rate_of_blank = 0.5;
    else if (difficulty == "Medium")
        rate_of_blank = 0.6;
    else if (difficulty == "Hard")
        rate_of_blank = 0.75;
    auto tmp = generate_blank(size, rate_of_blank);
    user_input = vector<vector<int>>(size, vector<int>(size, 0));
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (tmp[i][j] == 0)
                user_input[i][j] = -1;
            else
                user_input[i][j] = 0;
        }
}

void Sudoku::remove() {
    if (user_input[cursor_y][cursor_x] > 0 && user_input[cursor_y][cursor_x] != matrix[cursor_y][cursor_x])
        user_input[cursor_y][cursor_x] = -1;

}

void Sudoku::hint() {
    if (user_input[cursor_y][cursor_x] != 0)
        user_input[cursor_y][cursor_x] = matrix[cursor_y][cursor_x];
}

int Sudoku::get_size() {
    return size;
}

#endif //NYCU_SUDOKU_H

