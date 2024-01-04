// this is a sudoku game
#include "sudoku.h"

using namespace std;


int main() {
    init_ncurses();
    bool exit = false;
    while (true) {
// user input page
        // wait for enter
        string size_choices[] = {"4x4", "9x9", "16x16", "From File"};
        string difficulty_choices[] = {"Easy", "Medium", "Hard"};
        WINDOW *w_size = newwin(9, 26, 10, COLS / 2 - 26);
        WINDOW *w_difficulty = newwin(9, 26, 10, COLS / 2);
        Choose sudoku_size(w_size, size_choices, 4, "Choose Size", 3, 4);
        Choose sudoku_difficulty(w_difficulty, difficulty_choices, 3, "Choose Difficulty", 3, 4);
        bool ok = false, choosingsize = true;
        while (!ok) {
            // print ascii art
            clear();
            int i = (uint64_t) (getTime() * 2) % 6;
            int x = (COLS - 51) / 2;
            draw_asciiart(stdscr, 2 - (i == 0), x, "asciiarts/sudoku-s.txt");
            draw_asciiart(stdscr, 2 - (i == 1), x += 8, "asciiarts/sudoku-u.txt");
            draw_asciiart(stdscr, 2 - (i == 2), x += 9, "asciiarts/sudoku-d.txt");
            draw_asciiart(stdscr, 2 - (i == 3), x += 8, "asciiarts/sudoku-o.txt");
            draw_asciiart(stdscr, 2 - (i == 4), x += 9, "asciiarts/sudoku-k.txt");
            draw_asciiart(stdscr, 2 - (i == 5), x += 8, "asciiarts/sudoku-u.txt");
            // print instructions
            mvprintw(22, (COLS - 35) / 2, "Press Enter to Restart or q to Quit");

            if (choosingsize) {
                box(w_size, 0, 0);
                wborder(w_difficulty, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            } else {
                box(w_difficulty, 0, 0);
                wborder(w_size, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            }
            sudoku_size.draw();
            sudoku_difficulty.draw();
            refresh();
            wrefresh(w_size);
            wrefresh(w_difficulty);
            switch (cvt2pad(getch())) {
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
                case '\n':
                    ok = true;
                    break;
                case 'q':
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
        // print sudoku
        Sudoku su;
        if (sudoku_size.getSelected() == "From File")
            su = Sudoku("sudoku.txt");
        else {
            su = Sudoku(sudoku_size.getSelected() == "9x9" ? 9 : sudoku_size.getSelected() == "16x16" ? 16 : 4);
            su.generate_all();
        }
        su.generate(sudoku_difficulty.getSelected());
        ok = false;
        double last_timer = getTime(), timer = 0;
        //for exit window
        WINDOW *w_exit;
        while (!ok) {
            // timer and print
            timer += getTime() - last_timer;
            last_timer = getTime();

            // print ascii art
            clear();
            int i = (uint64_t) (getTime() * 2) % 6;
            int x = (COLS - 51) / 2;
            draw_asciiart(stdscr, 2 - (i == 0), x, "asciiarts/sudoku-s.txt");
            draw_asciiart(stdscr, 2 - (i == 1), x += 8, "asciiarts/sudoku-u.txt");
            draw_asciiart(stdscr, 2 - (i == 2), x += 9, "asciiarts/sudoku-d.txt");
            draw_asciiart(stdscr, 2 - (i == 3), x += 8, "asciiarts/sudoku-o.txt");
            draw_asciiart(stdscr, 2 - (i == 4), x += 9, "asciiarts/sudoku-k.txt");
            draw_asciiart(stdscr, 2 - (i == 5), x += 8, "asciiarts/sudoku-u.txt");
            // print instructions
            draw_asciiart(stdscr, 8 + su.get_size() / 2, (COLS - 80) / 2, "asciiarts/manual.txt");
            // print sudoku
            su.drawsudoku(stdscr, 10, COLS / 2);
            // print timer
            mvprintw(11 + su.get_size() / 2, (COLS + 50) / 2, "Time: %02d:%02d", (int) timer / 60, (int) timer % 60);
            refresh();

            wchar_t c = getch();
            if (c != ERR) {
                switch (cvt2pad(c)) {
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
                    case 'q':
                        w_exit = newwin(7, 30, 12, (COLS - 30) / 2);
                        if (ask_if_exit(w_exit, 1))
                            ok = true;
                        delwin(w_exit);
                        last_timer = getTime();
                        break;
                    case '\n':
                        su.remove();
                        break;
                    case 'h':
                        if (su.is_correct())
                            break;
                        su.hint();
                        timer += 20;
                        ok = su.finished();
                        break;
                    default:
                        if (su.input(c)) {
                            if (!su.is_correct())
                                timer += 5;
                            ok = su.finished();
                        }
                        break;
                }
            }
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
                if (!draw_asciiart(win, 3, i--, "asciiarts/congradulations.txt"))
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
                draw_asciiart(win, 1, 1, "asciiarts/fail" + to_string((i++) % 4) + ".txt");
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