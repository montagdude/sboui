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

  wmove(_win, 1, 1);
  colors.turnOn(_win, header, bg_normal);
  wprintw(_win, "Action");
  colors.turnOff(_win);

  vlineloc = cols-2 - _shortcutwidth;
  nspaces = vlineloc - std::string("Action").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  colors.turnOn(_win, header, bg_normal);
  printToEol(" Shortcut");
  wmove(_win, 2, 1);
  colors.turnOff(_win);

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
  int ndots, vlineloc, printlen;
  unsigned int rows, cols, i; 

  getmaxyx(_win, rows, cols);

  // Go to item location

  wmove(_win, idx-_firstprint+3, 1);

  // Turn on bold for header

  if (_items[idx]->getBoolProp("header"))
    wattron(_win, A_BOLD);

  // Print name (if not a 'space' item)

  vlineloc = cols-2 - _shortcutwidth - 1;
  printlen = std::min(int(_items[idx]->name().size()), vlineloc);

  if (_items[idx]->getBoolProp("space"))
    ndots = vlineloc;
  else
  {
    ndots = vlineloc - _items[idx]->name().size();
    wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());
  }

  // Turn off bold for header

  if (_items[idx]->getBoolProp("header"))
    wattroff(_win, A_BOLD);

  // Print dots and shortcut 

  if ( (! _items[idx]->getBoolProp("header")) &&
       (! _items[idx]->getBoolProp("space")) )
  {
    for ( i = 0; int(i) < ndots; i++ ) { waddch(_win, '.'); }
    wmove(_win, idx-_firstprint+3, vlineloc+3);
    printToEol(_items[idx]->getProp("shortcut"));
  }
  else 
    for ( i = 0; int(i) < ndots; i++ ) { waddch(_win, ' '); }

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

  addItem(new HelpItem("Main window", "", true, false));
  addItem(new HelpItem("Filter SlackBuilds", "f"));
  addItem(new HelpItem("Help / show shortcuts", "?"));
  addItem(new HelpItem("Options", "o"));
  addItem(new HelpItem("Quit", "q"));
  addItem(new HelpItem("Search", "/"));
  addItem(new HelpItem("Switch active list", "Tab"));
  addItem(new HelpItem("Sync / update repository", "s"));
  addItem(new HelpItem("Toggle vertical/horizontal layout", "l"));

  addItem(new HelpItem("space1", "", false, true));

  addItem(new HelpItem("Lists & input boxes", "", true, false));
  addItem(new HelpItem("Go back / cancel", "Esc"));
  addItem(new HelpItem("Jump to beginning", "Home"));
  addItem(new HelpItem("Jump to end", "End"));
  addItem(new HelpItem("Next input field", "Tab"));
  addItem(new HelpItem("Previous input field", "Shift+Tab"));
  addItem(new HelpItem("Pull down combo box", "Space"));
  addItem(new HelpItem("Scroll down", "Down arrow"));
  addItem(new HelpItem("Scroll up", "Up arrow"));
  addItem(new HelpItem("Scroll down a page", "Page down"));
  addItem(new HelpItem("Scroll up a page", "Page up"));
  addItem(new HelpItem("Select highlighted", "Enter"));
  addItem(new HelpItem("Toggle check box", "Space"));

  addItem(new HelpItem("space2", "", false, true));

  addItem(new HelpItem("Tagging", "", true, false));
  addItem(new HelpItem("Install tagged", "i"));
  addItem(new HelpItem("Reinstall tagged", "e"));
  addItem(new HelpItem("Remove tagged", "r"));
  addItem(new HelpItem("Tag and move down", "t"));
  addItem(new HelpItem("Tag and move up", "T"));
  addItem(new HelpItem("Upgrade tagged", "u"));
   
  // Determine width needed to display shortcut

  nitems = numItems();
  _shortcutwidth = 0;
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->getProp("shortcut").size() > _shortcutwidth)
      _shortcutwidth = _items[i]->getProp("shortcut").size();
  } 
  if (std::string("Shortcut").size() > _shortcutwidth)
    _shortcutwidth = std::string("Shortcut").size();
  _shortcutwidth += 2;  // Margins
}

/*******************************************************************************

Constructors and destructor

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

HelpWindow::~HelpWindow()
{
  unsigned int i, nitems;

  nitems = numItems();
  for ( i = 0; i < nitems; i++ ) { delete _items[i]; }
  _items.resize(0);
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
