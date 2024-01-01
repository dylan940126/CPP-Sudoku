// this is a sudoku game
#include "sudoku.h"

using namespace std;


int main() {
    initscr();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    raw();
    noecho();
    curs_set(0);
    string size_choices[3] = {"9x9", "16x16", "25x25"};
    string difficulty_choices[3] = {"Easy", "Medium", "Hard"};
    WINDOW *w_size = newwin(9, 26, LINES / 2 - 5, COLS / 2 - 26);
    WINDOW *w_difficulty = newwin(9, 26, LINES / 2 - 5, COLS / 2);
    Choose sudoku_size(w_size, size_choices, 3, "Choose Size", 3, 4);
    Choose sudoku_difficulty(w_difficulty, difficulty_choices, 3, "Choose Difficulty", 3, 4);
    // choose sudoku size
    bool ok = false, choosingsize = true;
    mvprintw(LINES / 2 + 7, COLS / 2 - 10, "Press Enter to Start");
    while (!ok) {
        if (choosingsize) {
            box(w_size, 0, 0);
            wborder(w_difficulty, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        }
        else {
            box(w_difficulty, 0, 0);
            wborder(w_size, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        }
        sudoku_size.draw();
        sudoku_difficulty.draw();
        wrefresh(w_size);
        wrefresh(w_difficulty);
        switch (getch()) {
            case KEY_UP:
                if(choosingsize)
                    sudoku_size.up();
                else
                    sudoku_difficulty.up();
                break;
            case KEY_DOWN:
                if(choosingsize)
                    sudoku_size.down();
                else
                    sudoku_difficulty.down();
                break;
            case KEY_RIGHT:
                choosingsize = false;
                break;
            case KEY_LEFT:
                choosingsize = true;
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
    curs_set(1);
    endwin();
    return 0;
}