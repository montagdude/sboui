#include "curses.h"
#include "colors.h"
#include "MainWindow.h"

namespace colors
{
  int normal = -1;
  int title = -1;
  int info = -1;
  int highlighted_active = -1;
  int highlighted_inactive = -1;
  int disabled = -1;
  int disabled_highlighted = -1;
  int description = -1;
  int error = -1;
}

int main()
{
  MainWindow mainwindow;

  initscr();
  curs_set(0);
  raw();
  noecho();
  set_escdelay(25);
  keypad(stdscr, TRUE);

  // Set colors

  if (has_colors())
  {
    colors::normal = 1;
    colors::title = 2;
    colors::info = 3;
    colors::highlighted_active = 4;
    colors::highlighted_inactive = 5;
  
    start_color();
    init_pair(colors::normal, COLOR_WHITE, COLOR_BLACK);
    init_pair(colors::title, COLOR_WHITE, COLOR_BLUE);
    init_pair(colors::info, COLOR_WHITE, COLOR_BLUE);
    init_pair(colors::highlighted_active, COLOR_BLACK, COLOR_CYAN);
    init_pair(colors::highlighted_inactive, COLOR_BLACK, COLOR_WHITE);
    bkgd(COLOR_PAIR(colors::normal));
  } 

  // User interaction loop
  
  mainwindow.initialize();
  mainwindow.show();
  endwin();
}
