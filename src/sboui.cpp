#include <string>
#include <iostream>
#include <string>
#include <vector>
#include "CLOParser.h"
#include "curses.h"
#include "settings.h"
#include "backend.h"
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

  // Set up ncurses (needed because we set colors while reading config file)

  initscr();
  curs_set(0);
  raw();
  noecho();
  set_escdelay(25);
  keypad(stdscr, TRUE);

  // Read config file. Temporarily leave ncurses mode so that stdin/out work
  // properly for this step.

  def_prog_mode();
  endwin();
  if (clos.requestInputFile()) { check = read_config(clos.inputFile()); }
  else { check = read_config(); }
  if (check != 0) { return check; }
  reset_prog_mode();

  // Read blacklist

  package_blacklist.read("/etc/sboui/package_blacklist"); 

  // User interaction loop (note: call constructor after setting colors)

  MainWindow mainwindow(PACKAGE_VERSION);
  mainwindow.initialize(true);
  mainwindow.exec();

  endwin();
  return 0;
}
