// this is a sudoku game
#include <csignal>
#include "sudoku.h"
#include <fstream>

using namespace std;


int main() {
    init_ncurses();

    // print ascii art
    print_asciiart(stdscr, 2, (COLS - 51) / 2, "sudoku.txt");

    // print instructions
    mvprintw(21, COLS / 2 - 10, "Press Enter to Start");
    refresh();


    // wait for enter
    string size_choices[3] = {"4x4", "9x9", "16x16"};
    string difficulty_choices[3] = {"Easy", "Medium", "Hard"};
    WINDOW *w_size = newwin(9, 26, 10, COLS / 2 - 26);
    WINDOW *w_difficulty = newwin(9, 26, 10, COLS / 2);
    Choose sudoku_size(w_size, size_choices, 3, "Choose Size", 3, 4);
    Choose sudoku_difficulty(w_difficulty, difficulty_choices, 3, "Choose Difficulty", 3, 4);
    bool ok = false, choosingsize = true;
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
                ok = true;
                break;
            default:
                break;
        }
        usleep(1000);
    }
    delwin(w_size);
    delwin(w_difficulty);
    clear();
    print_asciiart(stdscr, 2, (COLS - 51) / 2, "sudoku.txt");
    refresh();
    // print sudoku
    int size = sudoku_size.getSelected() == "9x9" ? 9 : sudoku_size.getSelected() == "16x16" ? 16 : 4;
    vector<vector<chtype>> sudoku = Sudoku::printsudoku(size);
    for (int i = 0; i < sudoku.size(); i++)
        for (int j = 0; j < sudoku[i].size(); j++)
            mvaddch(10 + i, (COLS - sudoku[0].size() - 1) / 2 + j, sudoku[i][j]);
    refresh();
//    mvprintw(LINES / 2 - 5, COLS / 2 - 4, sudoku);
    refresh();
    while (getch() != '\n');
    curs_set(1);
    endwin();
    return 0;
}