// this is a sudoku game
#include <csignal>
#include "sudoku.h"

using namespace std;


int main() {
    init_ncurses();
    bool exit = false;
    while (true) {
// user input page
        // print ascii art
        draw_asciiart(stdscr, 2, (COLS - 51) / 2, "sudoku-text.txt");
        // print instructions
        mvprintw(22, (COLS - 35) / 2, "Press Enter to Restart or q to Quit");
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
        draw_asciiart(stdscr, 2, (COLS - 51) / 2, "sudoku-text.txt");
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
        draw_asciiart(stdscr, 8 + su.get_size() / 2, (COLS - 80) / 2, "manual.txt");
        ok = false;
        time_t last_timer = time(nullptr);
        double timer = 0;
        while (!ok) {
            // timer
            timer += difftime(time(nullptr), last_timer);
            last_timer = time(nullptr);
            mvprintw(11 + su.get_size() / 2, (COLS + 50) / 2, "Time: %02d:%02d", (int) timer / 60, (int) timer % 60);

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
        clear();

// finish page
        wchar_t c;
        if (su.finished()) {
            mvprintw(25, (COLS - 35) / 2, "Press Enter to Restart or q to Quit");
            WINDOW *win = newwin(18, 80, 5, (COLS - 80) / 2);
            int i = 79;
            while (true) {
                c = getch();
                if (c == L'\n' || c == L'q')
                    break;
                wclear(win);
                if (!draw_asciiart(win, 3, i--, "congradulations.txt"))
                    i = 79;
                box(win, '-', 0);
                wrefresh(win);
            }
        } else {
            mvprintw(25, (COLS - 35) / 2, "Press Enter to Restart or q to Quit");
            WINDOW *win = newwin(10, 56, 5, (COLS - 56) / 2);
            int i = 79;
            while (true) {
                c = getch();
                if (c == L'\n' || c == L'q')
                    break;
                wclear(win);
                draw_asciiart(win, 1, 1, "fail" + to_string((i++) % 4) + ".txt");
                wrefresh(win);
                system("sleep 0.2");
            }
        }
        clear();
        refresh();
        if (c == L'q')
            break;
    }
    end_ncurses();
    return 0;
}