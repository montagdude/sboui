#include <string>
#include <curses.h>
#include "settings.h"
#include "Color.h"
#include "ListItem.h"
#include "MenubarList.h"
#include "MouseEvent.h"

/*******************************************************************************

Draw outer border around box

*******************************************************************************/
void MenubarList::redrawFrame()
{
  int rows, cols, i;

  getmaxyx(_win, rows, cols);

  // Outside border

  wmove(_win, 0, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }
  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Corners

  mvwaddch(_win, 0, 0, ACS_ULCORNER);
  mvwaddch(_win, 0, cols-1, ACS_URCORNER);
  mvwaddch(_win, rows-1, 0, ACS_LLCORNER);
  mvwaddch(_win, rows-1, cols-1, ACS_LRCORNER);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void MenubarList::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int rows, cols, rowsavail, lastrow, nspaces, i, hidx, len;
  int color_pair1, color_pair2;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;
  lastrow = _firstprint+rowsavail-1;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+_header_rows, 1);

  // Turn on highlight color

  if (int(idx) == _highlight)
  {
    if (_activated) 
    { 
      fg = "fg_highlight_active"; 
      bg = "bg_highlight_active"; 
    }
    else
    {
      fg = "fg_highlight_inactive"; 
      bg = "bg_highlight_inactive"; 
    }
    color_pair1 = colors.getPair(fg, bg);
    color_pair2 = colors.getPair("hotkey", bg);
    if (colors.turnOn(_win, color_pair1) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else
  {
    color_pair1 = colors.getPair("fg_combobox", "bg_combobox");
    color_pair2 = colors.getPair("hotkey", "bg_combobox");
  }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item and turn off highlight color

  nspaces = cols - 2 - 2*_outerpad - _items[idx]->name().size()
          - _items[idx]->getProp("shortcut").size();
  len = _items[idx]->name().size();
  hidx = _items[idx]->hotKey();
  for ( i = 0; i < int(_outerpad); i++ ) { waddch(_win, ' '); }
  for ( i = 0; i < len; i++ )
  {
    if ( i == hidx )
    {
      colors.turnOff(_win);
      if (colors.turnOn(_win, color_pair2) != 0) { wattron(_win, A_BOLD); }
      wprintw(_win, _items[idx]->name().substr(i,1).c_str());
      if (colors.turnOff(_win) != 0) { wattroff(_win, A_BOLD); }
      colors.turnOn(_win, color_pair1);
    }
    else { wprintw(_win, _items[idx]->name().substr(i,1).c_str()); }
  }
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
  printToEol(_items[idx]->getProp("shortcut"));
  if (colors.turnOff(_win) != 0)
  { 
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }

  // Redraw right border or scroll indicator

  if ( (_firstprint > 0) && (int(idx) == _firstprint) )
    waddch(_win, ACS_UARROW);
  else if ( (lastrow < int(_items.size())-1) && (int(idx) == lastrow) )
    waddch(_win, ACS_DARROW);
  else { waddch(_win, ACS_VLINE); }
}

/*******************************************************************************

Constructors

*******************************************************************************/
MenubarList::MenubarList()
{ 
  _reserved_rows = 2;
  _header_rows = 1;
  _outerpad = 0;
  _innerpad = 0;
  _hotkey = -1;
  clearButtons();
  setModal(false);
}

MenubarList::MenubarList(WINDOW *win) 
{
  _win = win;
  _reserved_rows = 2; 
  _header_rows = 1;
  _outerpad = 0;
  _innerpad = 0;
  _hotkey = -1;
  clearButtons();
  setModal(false);
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void MenubarList::setPad(unsigned int outerpad, unsigned int innerpad)
{
  _outerpad = outerpad;
  _innerpad = innerpad;
}

void MenubarList::setHotKey(int hotkey) { _hotkey = hotkey; }

/*******************************************************************************

Get attributes

*******************************************************************************/
int MenubarList::hotKey() const { return _hotkey; }
void MenubarList::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols;
  unsigned int i, nitems;
  std::string shortcut;

  // Minimum usable height

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Minimum usable width

  width = 0;
  reserved_cols = 2 + 2*_outerpad;    // 2 for border

  // Width for name + innerpad + shortcut

  for ( i = 0; i < nitems; i++ )
  {
    shortcut = _items[i]->getProp("shortcut");
    if (shortcut.size() > 0)
      namelen = _items[i]->name().size() + _innerpad
              + _items[i]->getProp("shortcut").size();
    else
      namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void MenubarList::preferredSize(int & height, int & width) const
{
  minimumSize(height, width);
}
