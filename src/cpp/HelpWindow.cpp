#include <vector>
#include <string>
#include <cmath>     // floor
#include <algorithm> // min
#include "Color.h"
#include "settings.h"
#include "HelpItem.h"
#include "HelpWindow.h"

using namespace color;

/*******************************************************************************

Draws window border, title, and header

*******************************************************************************/
void HelpWindow::redrawFrame() const
{
  unsigned int rows, cols, namelen, i, nspaces, vlineloc;
  double mid, left, right;
  int pair_header;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  wattron(_win, A_BOLD);
  wprintw(_win, _name.c_str());
  wattroff(_win, A_BOLD);

  // Corners

  wmove(_win, 0, 0);
  waddch(_win, ACS_ULCORNER);
  wmove(_win, rows-1, 0);
  waddch(_win, ACS_LLCORNER);
  wmove(_win, rows-1, cols-1);
  waddch(_win, ACS_LRCORNER);
  wmove(_win, 0, cols-1);
  waddch(_win, ACS_URCORNER);

  // Top border

  wmove(_win, 0, 1);
  for ( i = 1; int(i) < left-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, 0, right+1);
  for ( i = right+1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Symbols on right border to indicate scrolling

  if (_firstprint != 0) { mvwaddch(_win, 3, cols-1, ACS_UARROW); }
  if (_items.size() > _firstprint + rows-4)
  {
    mvwaddch(_win, rows-2, cols-1, ACS_DARROW);
  }

  // Draw header

  pair_header = colors.pair(header, bg_normal);

  wmove(_win, 1, 1);
  if (pair_header != -1) { wattron(_win, COLOR_PAIR(pair_header)); }
  wattron(_win, A_BOLD);
  wprintw(_win, "Action");
  if (pair_header != -1) { wattroff(_win, COLOR_PAIR(pair_header)); }
  wattroff(_win, A_BOLD);

  vlineloc = cols-2 - _shortcutwidth;
  nspaces = vlineloc - std::string("Action").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  if (pair_header != -1) { wattron(_win, COLOR_PAIR(pair_header)); }
  wattron(_win, A_BOLD);
  printToEol(" Shortcut");
  wmove(_win, 2, 1);
  if (pair_header != -1) { wattroff(_win, COLOR_PAIR(pair_header)); }
  wattroff(_win, A_BOLD);

  // Draw horizontal and then vertical line

  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  for ( i = 3; i < rows-1; i++ ) { mvwaddch(_win, i, vlineloc, ACS_VLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, 2, vlineloc, ACS_TTEE);
  mvwaddch(_win, rows-1, vlineloc, ACS_BTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void HelpWindow::redrawSingleItem(unsigned int idx)
{
  int nspaces, vlineloc, printlen;
  unsigned int rows, cols, i; 

  getmaxyx(_win, rows, cols);

  // Go to item location and print item

  wmove(_win, idx-_firstprint+3, 1);

  // Print name, spaces, shortcut

  vlineloc = cols-2 - _shortcutwidth - 1;
  printlen = std::min(int(_items[idx]->name().size()), vlineloc);

  nspaces = vlineloc - _items[idx]->name().size();
  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());

  for ( i = 0; int(i) < nspaces; i++ ) { waddch(_win, ' '); }

  wmove(_win, idx-_firstprint+3, vlineloc+3);
  printToEol(_items[idx]->getProp("shortcut"));

  // Divider

  wmove(_win, idx-_firstprint+3, vlineloc+1);
  waddch(_win, ACS_VLINE);
}

/*******************************************************************************

Constructs list to display

*******************************************************************************/
void HelpWindow::createList()
{
  unsigned int i, nitems;
  std::vector<std::string> names, shortcuts;

  names.push_back("Filter SlackBuilds");
  names.push_back("Sync repository");
  names.push_back("Search");
  names.push_back("Help / show shortcuts");
  names.push_back("Quit");
  names.push_back("Toggle vertical/horizontal layout");
  names.push_back("Switch active list");
  names.push_back("Scroll up");
  names.push_back("Scroll down");
  names.push_back("Scroll page up");
  names.push_back("Scroll page down");
  names.push_back("Jump to end");
  names.push_back("Jump to beginning");
  names.push_back("Select highlighted");
  names.push_back("Go back / cancel");
  names.push_back("Toggle selection");
  names.push_back("Tag/untag SlackBuild or group");
  names.push_back("Install tagged");
  names.push_back("Upgrade tagged");
  names.push_back("Remove tagged");
  names.push_back("Reinstall tagged");
   
  shortcuts.push_back("f");
  shortcuts.push_back("s");
  shortcuts.push_back("/");
  shortcuts.push_back("?");
  shortcuts.push_back("q");
  shortcuts.push_back("l");
  shortcuts.push_back("Tab");
  shortcuts.push_back("Up arrow");
  shortcuts.push_back("Down arrow");
  shortcuts.push_back("Page up");
  shortcuts.push_back("Page down");
  shortcuts.push_back("Home");
  shortcuts.push_back("End");
  shortcuts.push_back("Enter");
  shortcuts.push_back("Esc");
  shortcuts.push_back("Space");
  shortcuts.push_back("t");
  shortcuts.push_back("i");
  shortcuts.push_back("u");
  shortcuts.push_back("r");
  shortcuts.push_back("e");

  nitems = names.size();
  for ( i = 0; i < nitems; i++ )
  {
    HelpItem item;
    item.setName(names[i]);
    item.setProp("shortcut", shortcuts[i]);
    _helpitems.push_back(item);
  }
  for ( i = 0; i < nitems; i++ ) { addItem(&_helpitems[i]); }

  // Determine width needed to display shortcut

  _shortcutwidth = 0;
  for ( i = 0; i < nitems; i++ )
  {
    if (shortcuts[i].size() > _shortcutwidth)
      _shortcutwidth = shortcuts[i].size();
  } 
  if (std::string("Shortcut").size() > _shortcutwidth)
    _shortcutwidth = std::string("Shortcut").size();
  _shortcutwidth += 2;  // Margins
}

/*******************************************************************************

Constructors

*******************************************************************************/
HelpWindow::HelpWindow()
{ 
  _name = "Keyboard shortcuts";
  _reserved_rows = 4; 
  _shortcutwidth = 0;
  createList();
}

HelpWindow::HelpWindow(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _reserved_rows = 4;
  _shortcutwidth = 0;
  createList();
}

/*******************************************************************************

Sizes and places window

*******************************************************************************/
void HelpWindow::placeWindow() const
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  mvwin(_win, 2, 0);
  wresize(_win, rows-4, cols);
}
