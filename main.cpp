// this is a sudoku game
#include <csignal>
#include "sudoku.h"

#define LINES getmaxy(stdscr)
#define COLS getmaxx(stdscr)
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
                delwin(w_size);
                delwin(w_difficulty);
                ok = true;
                break;
            default:
                break;
        }
        usleep(1000);
    }
    // print sudoku
    vector<vector<chtype>> sudoku = Sudoku::printsudoku(
            sudoku_size.getSelected() == "9x9" ? 9 : sudoku_size.getSelected() == "16x16" ? 16 : 25);
    for (int i = 0; i < sudoku.size(); i++) {
        for (int j = 0; j < sudoku[i].size(); j++) {
            mvaddch((LINES - sudoku.size()) / 2 + i, (COLS - sudoku[0].size()) / 2 + j, sudoku[i][j]);
        }
    }
    refresh();
//    mvprintw(LINES / 2 - 5, COLS / 2 - 4, sudoku);

    while (getch() != '\n');
    curs_set(1);
    endwin();
    return 0;
}