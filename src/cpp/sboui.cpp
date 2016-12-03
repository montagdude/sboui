#include <string>
#include "curses.h"
#include "Color.h"
#include "color_settings.h"
#include "MainWindow.h"

namespace color
{
  Color colors;
  std::string fg_normal, bg_normal;
  std::string fg_title, bg_title;
  std::string fg_info, bg_info;
  std::string fg_highlight_active, bg_highlight_active;
  std::string fg_highlight_inactive, bg_highlight_inactive;
  std::string header;
  std::string tagged;
  std::string fg_popup, bg_popup;
} 

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
    //tagged = "yellow";
    //fg_popup = "blue";
    //bg_popup = "white";

    // Light color scheme
    fg_normal = "black";
    bg_normal = "white";
    fg_title = "white";
    bg_title = "blue";
    fg_info = "white";
    bg_info = "blue";
    fg_highlight_active = "black";
    bg_highlight_active = "cyan";
    fg_highlight_inactive = "white";
    bg_highlight_inactive = "black";
    header = "red";
    tagged = "red";
    fg_popup = "white";
    bg_popup = "black";

    colors.addPair(fg_normal, bg_normal);
    colors.addPair(fg_title, bg_title);
    colors.addPair(fg_info, bg_info);
    colors.addPair(fg_highlight_active, bg_highlight_active);
    colors.addPair(fg_highlight_inactive, bg_highlight_inactive);
    colors.addPair(header, bg_normal); 
    colors.addPair(tagged, bg_normal); 
    colors.addPair(tagged, bg_highlight_active); 
    colors.addPair(tagged, bg_highlight_inactive); 
    colors.addPair(fg_popup, bg_popup); 
    bkgd(COLOR_PAIR(colors.pair(fg_normal, bg_normal)));
  } 

  // User interaction loop

  check = mainwindow.initialize();
  mainwindow.show();

  endwin();
}
