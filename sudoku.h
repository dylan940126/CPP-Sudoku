#ifndef NYCU_SUDOKU_H
#define NYCU_SUDOKU_H

#include <ncurses.h>
#include <fstream>
#include <cassert>
#include <codecvt>
#include <chrono>
#include "gen_sudoku.cpp"

typedef std::vector<std::vector<char>> Matrix;

void init_ncurses() {
    setlocale(LC_ALL, "");
    usleep(100000);
    initscr();
    cbreak();
    noecho();
    timeout(50);
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);//default
    init_pair(1, COLOR_BLUE, COLOR_CYAN);  //cursor
    init_pair(2, COLOR_BLUE, COLOR_YELLOW);//highlight
    init_pair(3, COLOR_GREEN, COLOR_BLACK);//correct
    init_pair(4, COLOR_RED, COLOR_BLACK);  //wrong
}

void end_ncurses() {
    clear();
    refresh();
    curs_set(1);
    endwin();
}

double getTime() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}

bool draw_asciiart(WINDOW *win, int y, int x, const std::string &file) {
    if (x > getmaxx(win) || y > getmaxy(win))
        return false;
    bool flag = false;
    std::wifstream f(file, std::ios::binary);
    if (!f)
        return false;
    f.imbue(std::locale(f.getloc(), new std::codecvt_utf8<wchar_t>));
    std::wstring line;
    int xx;
    while (y < 0)
        y++, getline(f, line);
    while (y <= getmaxy(win) && getline(f, line)) {
        if (x < 0) {
            if (-x > line.size()) {
                y++;
                continue;
            }
            line = line.substr(-x);
            xx = 0;
        } else {
            xx = x;
        }
        if (xx + line.size() > getmaxx(win)) {
            line = line.substr(0, getmaxx(win) - xx);
        }
        if (!line.empty())
            flag = true;
        mvwprintw(win, y++, xx, "%ls", line.c_str());
    }
    f.close();
    return flag;
}

bool ask_if_exit(WINDOW *win, int edge) {
    //ask if the user want to ask_if_exit
    box(win, 0, 0);
    mvwprintw(win, edge, (getmaxx(win) - 6) / 2, "PAUSED");
    mvwprintw(win, edge + 2, (getmaxx(win) - 21) / 2, "Are you sure to QUIT?");
    bool exit = false;
    while (true) {
        int c = getch();
        if (c == KEY_RIGHT || c == 'l')
            exit = false;
        else if (c == KEY_LEFT || c == 'j')
            exit = true;
        else if (c == '\n')
            break;
        else if (c == 'q') {
            exit = false;
            break;
        }
        if (exit) {
            wattron(win, A_REVERSE);
            mvwprintw(win, getmaxy(win) - edge - 1, 4 * edge, "YES");
            wattroff(win, A_REVERSE);
            mvwprintw(win, getmaxy(win) - edge - 1, getmaxx(win) - 4 * edge - 3, "NO");
        } else {
            mvwprintw(win, getmaxy(win) - edge - 1, 4 * edge, "YES");
            wattron(win, A_REVERSE);
            mvwprintw(win, getmaxy(win) - edge - 1, getmaxx(win) - 4 * edge - 3, "NO");
            wattroff(win, A_REVERSE);
        }
        wrefresh(win);
    }
    wclear(win);
    wrefresh(win);
    return exit;
}

wchar_t cvt2pad(wchar_t c) {
    switch (c) {
        case 'i':
            return KEY_UP;
        case 'j':
            return KEY_LEFT;
        case 'k':
            return KEY_UP;
        case 'l':
            return KEY_RIGHT;
    }
    return c;
}

class Choose {
public:
    Choose(WINDOW *win, std::string *choices, int size, std::string title = "", int y = 0, int x = 0) {
        this->win = win;
        this->title = std::move(title);
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

    Sudoku(const std::string &filename);

    void init(int size);

    int get_size() const;

    void generate_all();

    void generate(const string &difficulty);

    void drawsudoku(WINDOW *win, int y, int center_x);

    void cursor_up();

    void cursor_down();

    void cursor_left();

    void cursor_right();

    bool input(wchar_t c);

    void remove();

    void hint();

    bool is_correct();

    bool finished();

private:
    int size{};
    int box_size{};
    Matrix matrix;
    int cursor_x{};
    int cursor_y{};
    vector<vector<int>> user_input;//0 for hide, 1 for show, 2 for input correct, 3 for input wrong
};

Sudoku::Sudoku(int size) {
    init(size);
    generate_all();
}

Sudoku::Sudoku(const std::string &filename) {
    ifstream fin;
    fin.open(filename);
    if (!fin)
        return;
    string s;
    char c;
    while (fin >> c)
        s += c;
    size = (int) sqrt(s.size());
    init(size);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = s[i * size + j];
}

void Sudoku::drawsudoku(WINDOW *win, int y, int center_x) {
    std::vector<std::wstring> ans;
    for (int i = 0; i < box_size; i++) {
        ans.emplace_back(1, L'┣');
        for (int j = 0; j < box_size; j++)
            ans.emplace_back(1, L'┃');
    }
    ans[0][0] = L'┏';
    ans.emplace_back(1, L'┗');


    for (int i = 0; i < box_size; i++) {
        int pr_y = 0;
        for (auto &s: ans) {
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


    int pr_y = 0, pr_x = 0, cursor_num;
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
                if (cursor_num != -1 && ans[i][j] == cursor_num)
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

bool Sudoku::input(wchar_t c) {
    if (size <= 9) {
        if (!(c >= '1' && c <= '9')) {
            return false;
        }
    } else {
        if (!((c >= '1' && c <= '9') || (c >= 'A' && c <= 'G') || (c >= 'a' && c <= 'g'))) {
            return false;
        }
    }
    if (c >= 'a' && c <= 'g')
        c = c - 'a' + 'A';
    if (!is_correct() && user_input[cursor_y][cursor_x] != c) {
        user_input[cursor_y][cursor_x] = c;
        return true;
    }
    return false;
}

void Sudoku::generate_all() {
    srand(time(nullptr));
    int sudoku[MaxSize][MaxSize] = {0};
    create_sudoku(sudoku, size, 0);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            if (sudoku[i][j] > 9)
                matrix[i][j] = 'A' + sudoku[i][j] - 10;
            else
                matrix[i][j] = '0' + sudoku[i][j];
        }
}

void Sudoku::generate(const string &difficulty) {
    double rate_of_blank;
    if (difficulty == "Easy")
        rate_of_blank = 0.5;
    else if (difficulty == "Medium")
        rate_of_blank = 0.6;
    else
        rate_of_blank = 0.75;
    user_input = generate_blank(size, rate_of_blank);
}

void Sudoku::remove() {
    if (!is_correct())
        user_input[cursor_y][cursor_x] = -1;
}

void Sudoku::hint() {
    if (!is_correct())
        user_input[cursor_y][cursor_x] = matrix[cursor_y][cursor_x];
}

int Sudoku::get_size() const {
    return size;
}

void Sudoku::init(int size) {
    assert(size == 4 || size == 9 || size == 16);
    this->size = size;
    this->box_size = (int) sqrt(size);
    this->matrix = Matrix(size, std::vector<char>(size, '.'));
    this->cursor_x = 2;
    this->cursor_y = 2;
    this->user_input = vector<vector<int>>(size, vector<int>(size, 0));
}

bool Sudoku::is_correct() {
    return user_input[cursor_y][cursor_x] == 0 ||
           user_input[cursor_y][cursor_x] == matrix[cursor_y][cursor_x];
}

#endif //NYCU_SUDOKU_H

