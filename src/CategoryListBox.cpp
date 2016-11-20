#include <vector>
#include <string>
#include <cmath>   // floor
#include "Color.h"
#include "color_settings.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "CategoryListBox.h"

using namespace color;

/*******************************************************************************

Draws window border, title, and header

*******************************************************************************/
void CategoryListBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, left, right, i;
  double mid;
  int pair_header;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(int(mid) - double(namelen)/2);
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
  for ( i = 1; i < left-1; i++ ) { waddch(_win, ACS_HLINE); }
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

  if (_firstprint != 0) { mvwaddch(_win, 3, cols-1, ACS_DIAMOND); }
  if (_items.size() > _firstprint + rows-4)
  {
    mvwaddch(_win, rows-2, cols-1, ACS_DIAMOND);
  }

  // Draw header

  wmove(_win, 1, 1);
  pair_header = colors.pair(header, bg_normal);
  if (pair_header != -1) { wattron(_win, COLOR_PAIR(pair_header)); }
  wattron(_win, A_BOLD);
  printToEol("Name");
  if (pair_header != -1) { wattroff(_win, COLOR_PAIR(pair_header)); }
  wattroff(_win, A_BOLD);
  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void CategoryListBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int color_pair;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+3, 1);

  // Turn on highlight color

  if (idx == _highlight)
  {
    if (_activated) 
    { 
      if (_items[idx]->tagged()) { fg = tagged; }
      else { fg = fg_highlight_active; }
      bg = bg_highlight_active; 
    }
    else
    {
      if (_items[idx]->tagged()) { fg = tagged; }
      else { fg = fg_highlight_inactive; }
      bg = bg_highlight_inactive; 
    }
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
    else { wattron(_win, A_REVERSE); }
  } 
  else
  {
    if (_items[idx]->tagged()) { fg = tagged; }
    else { fg = fg_normal; }
    bg = bg_normal;
    color_pair = colors.pair(fg, bg);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
  }

  if (_items[idx]->tagged()) { wattron(_win, A_BOLD); }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (idx == _highlight) { _prevhighlight = _highlight; }

  // Print item

  printToEol(_items[idx]->category());

  // Turn off highlight color

  if (color_pair != -1) { wattroff(_win, COLOR_PAIR(color_pair)); }
  else
  {
    if (idx == _highlight) { wattroff(_win, A_REVERSE); }
  }
  if (_items[idx]->tagged()) { wattroff(_win, A_BOLD); }
}

/*******************************************************************************

Constructors

*******************************************************************************/
CategoryListBox::CategoryListBox()
{
  _win = NULL;
  _name = "";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 4;
}

CategoryListBox::CategoryListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 4;
}
