#include <string>
#include "curses.h"
#include "settings.h"
#include "MainWindow.h"

int main()
{
  MainWindow mainwindow;
  int check;

  // Read config files

  check = read_config();
  if (check != 0) { return check; }

  // Set up ncurses

  initscr();
  curs_set(0);
  raw();
  noecho();
  set_escdelay(25);
  keypad(stdscr, TRUE);
  if (has_colors()) { apply_color_settings(); }

  // User interaction loop

  check = mainwindow.initialize();
  mainwindow.show();

  endwin();
}
