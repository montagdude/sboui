#include <string>
#include "curses.h"
#include "Color.h"
#include "color_settings.h"
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

  // Set colors

  if (has_colors())
  {
    // Dark color scheme
    //fg_normal = "white";     
    //bg_normal = "black";     
    //fg_title = "white";
    //bg_title = "blue";
    //fg_info = "white";
    //bg_info = "blue";
    //fg_highlight_active = "black";
    //bg_highlight_active = "cyan";
    //fg_highlight_inactive = "black";
    //bg_highlight_inactive = "white";
    //header = "yellow";
    //header_popup = "black";
    //tagged = "yellow";
    //fg_popup = "blue";
    //bg_popup = "white";
    //fg_warning = "white";
    //bg_warning = "red";

    // Light color scheme
    fg_normal = "black";
    bg_normal = "white";
    fg_title = "white";
    bg_title = "yellow";
    fg_info = "white";
    bg_info = "yellow";
    fg_highlight_active = "white";
    bg_highlight_active = "blue";
    fg_highlight_inactive = "white";
    bg_highlight_inactive = "black";
    header = "red";
    header_popup = "white";
    tagged = "red";
    fg_popup = "white";
    bg_popup = "black";
    fg_warning = "white";
    bg_warning = "red";

    colors.addPair(fg_normal, bg_normal);
    colors.addPair(fg_title, bg_title);
    colors.addPair(fg_info, bg_info);
    colors.addPair(fg_highlight_active, bg_highlight_active);
    colors.addPair(fg_highlight_inactive, bg_highlight_inactive);
    colors.addPair(header, bg_normal); 
    colors.addPair(header_popup, bg_popup); 
    colors.addPair(tagged, bg_normal); 
    colors.addPair(tagged, bg_highlight_active); 
    colors.addPair(tagged, bg_highlight_inactive); 
    colors.addPair(fg_popup, bg_popup); 
    colors.addPair(fg_warning, bg_warning); 
    bkgd(COLOR_PAIR(colors.pair(fg_normal, bg_normal)));
  } 

  // User interaction loop

  check = mainwindow.initialize();
  mainwindow.show();

  endwin();
}
