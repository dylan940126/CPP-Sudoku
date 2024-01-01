// this is a sudoku game
#include <curses.h>
#include <string.h>
#include <string>

using namespace std;

class Choose {
public:
    WINDOW *win;

    Choose(WINDOW *win, string *choices, int size) {
        this->win = win;
        this->choices = choices;
        this->size = size;
        this->choice = 0;
    }

    string getSelected() const {
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
                attron(A_REVERSE);
            }
            mvprintw(i, x, choices[i].c_str());
            attroff(A_REVERSE);
        }
    }

private:
    int x, y;
    string *choices;
    int size;
    int choice;
};

int main() {
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    raw();
    noecho();
    curs_set(0);
    // user use cursor to sudoku_size the sudoku sudoku_size
    // 4x4, 9x9, 16x16
    // can use up and down to sudoku_size and highlight the choice, and press enter to confirm
    string choices[3] = {"9x9", "16x16", "25x25"};
    WINDOW *win = newwin(LINES - 10, COLS + 10, 5, 5);
    box(win, '-', '-');
    wrefresh(win);
    Choose sudoku_size(COLS / 2, LINES / 2, choices, 3);
    Choose sudoku_difficulty(COLS / 2, LINES / 2 + 2, choices, 3);
    // choose sudoku size
    mvprintw(LINES / 2 - 2, COLS / 2 - 4, "Choose Sudoku Size");
    bool ok = false;
    while (!ok) {
        sudoku_size.draw();
        refresh();
        switch (getch()) {
            case KEY_UP:
                sudoku_size.up();
                break;
            case KEY_DOWN:
                sudoku_size.down();
                break;
            case '\n':
                clear();
                refresh();
                ok = true;
                break;
            default:
                break;
        }
    }
    finish_sudoku_size:;
}