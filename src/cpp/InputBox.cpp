#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // min, max
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "InputBox.h"

using namespace color;

/*******************************************************************************

Setting item to be highlighted

*******************************************************************************/
void InputBox::highlightFirst() 
{ 
  _prevhighlight = _highlight;
  _highlight = 0; 
}

void InputBox::highlightLast() 
{ 
  if (_items.size() > 0) 
  { 
    _prevhighlight = _highlight;
    _highlight = _items.size()-1; 
  }
}

void InputBox::highlightPrevious()
{
  if ( (_items.size() > 0) && (_highlight > 0) ) 
  { 
    _prevhighlight = _highlight;
    _highlight--; 
  }
}

void InputBox::highlightNext()
{
  if ( (_items.size() > 0) && (_highlight < _items.size()-1) ) 
  { 
    _prevhighlight = _highlight;
    _highlight++; 
  }
}

/*******************************************************************************

Draws window border, message, and info

*******************************************************************************/
void InputBox::redrawFrame() const
{
  unsigned int rows, cols, msglen, i;
  int left, pair_title, pair_info;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  msglen = _msg.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, 1, 1);
  wclrtoeol(_win);
  pair_title = colors.pair(fg_title, bg_title);
  if (pair_title != -1) { wattron(_win, COLOR_PAIR(pair_title)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_msg);
  if (pair_title != -1) { wattroff(_win, COLOR_PAIR(pair_title)); }
  wattroff(_win, A_BOLD);

  // Info on bottom of window

  msglen = _info.size();
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_info != -1) { wattron(_win, COLOR_PAIR(pair_info)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_info);
  if (pair_info != -1) { wattroff(_win, COLOR_PAIR(pair_info)); }
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

Constructors

*******************************************************************************/
InputBox::InputBox()
{
  _msg = "";
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _highlight = 0;
  _prevhighlight = 0;
}

InputBox::InputBox(WINDOW *win, const std::string & msg)
{
  _win = win;
  _msg = msg;
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _highlight = 0;
  _prevhighlight = 0;
}

/*******************************************************************************

Adds item to the input box, setting proper position and size

*******************************************************************************/
void InputBox::addItem(InputItem *item)
{
  int rows, cols;
  unsigned int nitems;

  getmaxyx(_win, rows, cols);
  nitems = _items.size();
  item->setPosition(nitems+3,1);
  item->setWidth(cols-2);
  item->setWindow(_win);
  _items.push_back(item);
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void InputBox::setMessage(const std::string & msg) { _msg = msg; }
void InputBox::setInfo(const std::string & info) { _info = info; }
void InputBox::setWindow(WINDOW *win) 
{ 
  unsigned int i, nitems;

  _win = win; 
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ ) { _items[i]->setWindow(win); }
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void InputBox::minimumSize(int & height, int & width) const
{
  int reserved_cols;

  // Scrolling is not implemented for this - just give number of rows needed

  height = 6 + _items.size();

  // Minimum usable width (at least 2 characters visible in entry)

  width = std::max(_msg.size(), _info.size());
  width = std::max(width, 2);
  reserved_cols = 2;       // For frame border
  width += reserved_cols;
}

void InputBox::preferredSize(int & height, int & width) const
{
  int reserved_cols;

  // Scrolling is not implemented for this - just give number of rows needed

  height = 6 + _items.size();

  // Preferred width (at least 30 characters visible in entries)

  width = std::max(_msg.size(), _info.size());
  width = std::max(width, 30);
  reserved_cols = 2;
  width += reserved_cols;
}

/*******************************************************************************

Redraws box and all input items

*******************************************************************************/
void InputBox::draw(bool force)
{
  int rows, cols;
  unsigned int i, nitems;
  int pair_popup;

  nitems = _items.size();
  getmaxyx(_win, rows, cols);

  if (force) { _redraw_type = "all"; }

  if (_redraw_type == "all") 
  { 
    wclear(_win);
    pair_popup = colors.pair(fg_popup, bg_popup);
    if (pair_popup != -1) { wbkgd(_win, COLOR_PAIR(pair_popup)); }
    redrawFrame();
    for ( i = 0; i < nitems; i++ ) 
    { 
      _items[i]->setWidth(cols-2);           // Dimensions may have changed
      _items[i]->draw(force, i==_highlight);  
    }
  }
  else 
  { 
    _items[_prevhighlight]->draw(force, false); 
    _items[_highlight]->draw(force, true); 
  }
  wrefresh(_win);
}

/*******************************************************************************

User interaction with input items in the box

*******************************************************************************/
std::string InputBox::exec()
{
  bool getting_input;
  std::string selection, retval;

  getting_input = true;
  while (getting_input)
  {

    // Draw input box elements
  
    draw();
    _redraw_type = "changed";

    // Get user input from highlighted item

    selection = _items[_highlight]->exec();
    if (selection == signals::resize)
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if (selection == signals::quit)
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if (selection == signals::highlightFirst) { highlightFirst(); }
    else if (selection == signals::highlightLast) { highlightLast(); }
    else if (selection == signals::highlightPrev) { highlightPrevious(); }
    else if (selection == signals::highlightNext) { highlightNext(); }
    else
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
  }

  return retval;
}
