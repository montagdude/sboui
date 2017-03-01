#include <string>
#include "curses.h"
#include "settings.h"
#include "MainWindow.h"

int main()
{
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
  if (backend::enable_color) { activate_color(); }
  else { deactivate_color(); }

  // User interaction loop

  MainWindow mainwindow;  // Call constructor after setting colors
  check = mainwindow.initialize();
  mainwindow.show();

  endwin();
}
