#include <string>
#include "curses.h"
#include "settings.h"
#include "MainWindow.h"

using namespace color;

int main()
{
  MainWindow mainwindow;
  int check;

  initscr();
  curs_set(0);
  raw();
  noecho();
  set_escdelay(25);
  keypad(stdscr, TRUE);

  // Read config files

  read_config();
  set_colors();

  // User interaction loop

  check = mainwindow.initialize();
  mainwindow.show();

  endwin();
}
