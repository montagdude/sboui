#include <string>
#include <iostream>
#include <string>
#include <vector>
#include "CLOParser.h"
#include "curses.h"
#include "settings.h"
#include "backend.h"
#include "MainWindow.h"
#include "MouseEvent.h"

#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION ""
#endif

int main(int argc, char *argv[])
{
  int check;
  CLOParser clos;
  MouseEvent mevent;

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
#if NCURSES_MOUSE_VERSION > 1
  mousemask(BUTTON1_PRESSED | BUTTON3_PRESSED | BUTTON4_PRESSED |
            BUTTON5_PRESSED, NULL);
#else
  // Note: in some terminals, REPORT_MOUSE_POSITION can be used to simulate
  // button 5, but in others it gives undesired behavior, so to be safe disable
  // scroll-down. Button 4 can still be used for scroll up, but I don't think
  // it makes sense to have scroll up without scroll down.
  mousemask(BUTTON1_PRESSED | BUTTON3_PRESSED, NULL);
#endif
  mouseinterval(0);

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
  mainwindow.initialize();
  mainwindow.exec(&mevent);

  endwin();
  return 0;
}
