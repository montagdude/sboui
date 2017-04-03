#include <string>
#include <iostream>
#include <string>
#include <vector>
#include "CLOParser.h"
#include "curses.h"
#include "settings.h"
#include "MainWindow.h"

#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION ""
#endif

int main(int argc, char *argv[])
{
  int check;
  CLOParser clos;

  // Parse command line arguments

  check = clos.checkCLOs(argc, argv, PACKAGE_VERSION);
  if (check == 1) { return check; }
  else if (check == -1) { return 0; }

  // Read config file

  if (clos.requestInputFile()) { check = read_config(clos.inputFile()); }
  else { check = read_config(); }
  if (check != 0) { return check; }

  // Set up ncurses

  initscr();
  curs_set(0);
  raw();
  noecho();
  set_escdelay(25);
  keypad(stdscr, TRUE);
  if (settings::enable_color) { activate_color(); }
  else { deactivate_color(); }

  // User interaction loop (note: call constructor after setting colors)

  MainWindow mainwindow(PACKAGE_VERSION);
  mainwindow.initialize();
  mainwindow.exec();

  endwin();
  return 0;
}
