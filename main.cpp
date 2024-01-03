// this is a sudoku game
#include <csignal>
#include "sudoku.h"
#include <fstream>

using namespace std;


int main() {
    init_ncurses();
    bool exit = false;
    while (true) {
// user input page
        // print ascii art
        print_asciiart(stdscr, 2, (COLS - 51) / 2, "sudoku-text.txt");
        // print instructions
        mvprintw(21, COLS / 2 - 10, "Press Enter to Start");
        refresh();
        // wait for enter
        string size_choices[] = {"4x4", "9x9", "16x16", "Custom"};
        string difficulty_choices[] = {"Easy", "Medium", "Hard"};
        WINDOW *w_size = newwin(9, 26, 10, COLS / 2 - 26);
        WINDOW *w_difficulty = newwin(9, 26, 10, COLS / 2);
        Choose sudoku_size(w_size, size_choices, 4, "Choose Size", 3, 4);
        Choose sudoku_difficulty(w_difficulty, difficulty_choices, 3, "Choose Difficulty", 3, 4);
        bool ok = false, choosingsize = true;
        while (!ok) {
            if (choosingsize) {
                box(w_size, 0, 0);
                wborder(w_difficulty, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            } else {
                box(w_difficulty, 0, 0);
                wborder(w_size, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            }
            sudoku_size.draw();
            sudoku_difficulty.draw();
            wrefresh(w_size);
            wrefresh(w_difficulty);
            switch (getch()) {
                case KEY_UP:
                    if (choosingsize)
                        sudoku_size.up();
                    else
                        sudoku_difficulty.up();
                    break;
                case KEY_DOWN:
                    if (choosingsize)
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
                case L'\n':
                    ok = true;
                    break;
                case L'q':
                    exit = true;
                    ok = true;
                    break;
                default:
                    break;
            }
        }
        delwin(w_size);
        delwin(w_difficulty);
        clear();
        refresh();
        if (exit)
            break;

// sudoku game page
        // print ascii art
        print_asciiart(stdscr, 2, (COLS - 51) / 2, "sudoku-text.txt");
        refresh();
        // print sudoku
        Sudoku su;
        if (sudoku_size.getSelected() == "Custom")
            su = Sudoku("sudoku.txt");
        else {
            su = Sudoku(sudoku_size.getSelected() == "9x9" ? 9 : sudoku_size.getSelected() == "16x16" ? 16 : 4);
            su.generate_all();
        }
        su.generate(sudoku_difficulty.getSelected());
        // print instructions
        print_asciiart(stdscr, 8+su.get_size()/2, (COLS - 80) / 2, "manual.txt");
        ok = false;
        while (!ok) {
            wchar_t c = getch();
            if (c != ERR) {
                switch (c) {
                    case KEY_UP:
                        su.cursor_up();
                        break;
                    case KEY_DOWN:
                        su.cursor_down();
                        break;
                    case KEY_RIGHT:
                        su.cursor_right();
                        break;
                    case KEY_LEFT:
                        su.cursor_left();
                        break;
                    case L'q':
                        ok = true;
                        break;
                    case L'\n':
                        su.remove();
                        break;
                    case L'h':
                        su.hint();
                        ok = su.finished();
                        break;
                    default:
                        su.input(c);
                        ok = su.finished();
                        break;
                }
            }
            su.drawsudoku(stdscr, 10, COLS / 2);
            refresh();
        }
        // fill?
        clear();
        refresh();
    }
    end_ncurses();
    return 0;
}