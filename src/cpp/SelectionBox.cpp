#include <string>
#include <curses.h>
#include <cmath>    // floor
#include "Color.h"
#include "color_settings.h"
#include "ListItem.h"
#include "SelectionBox.h"

using namespace color;

/*******************************************************************************

Draws window border, title, and info

*******************************************************************************/
void SelectionBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, i;
  int left, pair_title, pair_info;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  wmove(_win, 1, 0);
  wclrtoeol(_win);
  pair_title = colors.pair(fg_title, bg_title);
  if (pair_title != -1) { wattron(_win, COLOR_PAIR(pair_title)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_name);
  if (pair_title != -1) { wattroff(_win, COLOR_PAIR(pair_title)); }
  wattroff(_win, A_BOLD);

  // Info on bottom of window

  namelen = _info.size();
  left = std::floor(mid - double(namelen)/2.0);
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_title != -1) { wattron(_win, COLOR_PAIR(pair_title)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_info);
  if (pair_title != -1) { wattroff(_win, COLOR_PAIR(pair_title)); }
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
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Symbols on right border to indicate scrolling

  if (_firstprint != 0) { mvwaddch(_win, 3, cols-1, ACS_UARROW); }
  if (_items.size() > _firstprint + rows-3)
  {
    mvwaddch(_win, rows-4, cols-1, ACS_DARROW);
  }

  // Horizontal dividers for header and footer

  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, rows-3, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Connections

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, rows-3, 0, ACS_LTEE);
  mvwaddch(_win, rows-3, cols-1, ACS_RTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void SelectionBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int color_pair;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+3, 1);

  // Turn on highlight color

  if (int(idx) == _highlight)
  {
    if (_activated) 
    { 
      fg = fg_highlight_active; 
      bg = bg_highlight_active; 
    }
    else
    {
      fg = fg_highlight_inactive; 
      bg = bg_highlight_inactive; 
    }
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
    else { wattron(_win, A_REVERSE); }
  } 

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item

  printToEol(_items[idx]->name());

  // Turn off highlight color

  if (color_pair != -1) { wattroff(_win, COLOR_PAIR(color_pair)); } 
  else
  { 
    if (int(idx) == _highlight) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
SelectionBox::SelectionBox()
{
  _win = NULL;
  _name = "";
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 6;
}

SelectionBox::SelectionBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 6;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void SelectionBox::setInfo(const std::string & info) { _info = info; }

/*******************************************************************************

User interaction: show display until user hits Enter or Esc

*******************************************************************************/
std::string SelectionBox::exec()
{
  int ch, check_redraw;
  std::string retval;
  bool getting_input;

  const int MY_ESC = 27;

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  getting_input = true;
  while (getting_input)
  {

    // Draw list elements
  
    draw();
  
    // Get user input
  
    switch (ch = getch()) {
  
      // Enter key: return name of highlighted item
  
      case '\n':
      case '\r':
      case KEY_ENTER:
        retval = _items[_highlight]->name();
        _redraw_type = "all";
        getting_input = false;
        break;
  
      // Arrows/Home/End/PgUp/Dn: change highlighted value
  
      case KEY_UP:
        check_redraw = highlightPrevious();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_DOWN:
        check_redraw = highlightNext();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_PPAGE:
        check_redraw = highlightPreviousPage();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_NPAGE:
        check_redraw = highlightNextPage();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_HOME:
        check_redraw = highlightFirst();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_END:
        check_redraw = highlightLast();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
  
      // Resize signal: redraw (may not work with some curses implementations)
  
      case KEY_RESIZE:
        _redraw_type = "all";
        break;
  
      // Quit key
  
      case MY_ESC:
        retval = quitSignal;
        _redraw_type = "all";
        getting_input = false;
        break;
  
      default:
        _redraw_type = "none";
        break;
    }
  }
  return retval;
}
