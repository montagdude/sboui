#include <string>
#include <sstream>
#include <vector>
#include "curses.h"
#include "colors.h"
#include "ListBox.h"

std::string int2str(int inval)
{
  std::stringstream ss;
  ss << inval;
  return ss.str();
}

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
  ListBox list1, list2;
  WINDOW *win1, *win2;
  unsigned int i, curlist;
  bool display;
  std::string selection;
  std::vector<ListBox *> lists;

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
  } 
  attron(A_BOLD);
  printw("A tale of two lists");
  attroff(A_BOLD);
  refresh();

  // Create two windows

  win1 = newwin(25, 40, 5, 5);
  win2 = newwin(25, 40, 5, 45);

  // Create two lists

  list1.addItem("Item 1");
  list1.addItem("Item 2");
  list1.addItem("Item who?", true);
  list1.setWindow(win1);
  list1.setName("List 1");

  for ( i = 0; i < 30; i++ ) { list2.addItem("List 2, Item " + int2str(i)); }
  list2.setWindow(win2);
  list2.setName("List 2");

  lists.resize(2);
  lists[0] = &list1;
  lists[1] = &list2;

  // User interaction loop

  display = true;
  curlist = 0;
  lists[0]->setActivated(true);
  lists[1]->setActivated(false);
  list1.draw();
  list2.draw();
  while (display)
  {
    selection = lists[curlist]->exec();
    if (selection == "__QUIT__") { display = false; }
    else if (selection == "KEY_RIGHT")
    {
      if (curlist == 0)
      {
        lists[0]->setActivated(false);
        lists[1]->setActivated(true);
        lists[0]->draw();
        curlist = 1;
      }
    }
    else if (selection == "KEY_LEFT")
    {
      if (curlist == 1)
      {
        lists[0]->setActivated(true);
        lists[1]->setActivated(false);
        lists[1]->draw();
        curlist = 0;
      }
    }
  }  

  endwin();
}
