#include <iostream>
#include <string>
#include <csignal>
#include "locale.h"
#include "ncursesw/ncurses.h"
using namespace std;

int main()
{
    setlocale(LC_ALL, "");
    std::string str = "(☞ﾟヮﾟ)☞";   // String
    initscr();                     // Start curses mode
    printw(str.c_str());           // Print
    system("sleep 1");             // Wait for 1 second
    getch();                       // Wait for input
    endwin();
    return 0;
}