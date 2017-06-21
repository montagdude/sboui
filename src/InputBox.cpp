#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // min, max
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "InputBox.h"

/*******************************************************************************

Setting item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int InputBox::highlightFirst() 
{ 
  unsigned int i, nitems, retval;

  nitems = _items.size();
  if (nitems > 0)
  {
    _prevhighlight = _highlight;
    for ( i = 0; i < nitems; i++ )
    {
      if (_items[i]->selectable())
      {
        _highlight = i;
        break;
      }
    }
  }

  if (_firstprint == _header_rows) { retval = 0; }
  else { retval = 1; }
  _firstprint = _header_rows;

  return retval;
}

int InputBox::highlightLast() 
{ 
  int i, nitems, rows, cols, rowsavail, y, firstprintstore;
  unsigned int retval;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  nitems = _items.size();
  if (nitems > 0) 
  { 
    _prevhighlight = _highlight;
    for ( i = nitems-1; i >= 0; i-- )
    {
      if (_items[i]->selectable())
      {
        _highlight = i;
        break;
      } 
    }
  }

  y = _items[nitems-1]->posy();
  firstprintstore = _firstprint;
  if (y >= _firstprint + rowsavail) { _firstprint = y - rowsavail + 1; }
  if (_firstprint == firstprintstore) { return 0; }
  else { return 1; }

  return retval;
}

int InputBox::highlightPrevious()
{
  int i, nitems;

  nitems = _items.size();

  if ( (nitems > 0) && (_highlight > 0) ) 
  { 
    _prevhighlight = _highlight;
    for ( i = _highlight-1; i >= 0; i-- )
    {
      if (_items[i]->selectable())
      {
        _highlight = i;
        break;
      }
    }
  }

  return determineFirstPrint();
}

int InputBox::highlightNext()
{
  unsigned int i, nitems;

  nitems = _items.size();
  if ( (nitems > 0) && (_highlight < int(_items.size())-1) ) 
  { 
    _prevhighlight = _highlight;
    for ( i = _highlight+1; i < nitems; i++ )
    {
      if (_items[i]->selectable())
      {
        _highlight = i;
        break;
      }
    }
  }

  return determineFirstPrint();
}

int InputBox::highlightPreviousPage()
{
  int rows, cols, rowsavail, firstprintstore;
  unsigned int i, nitems;
  bool highlight_found;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Determine how far to page

  firstprintstore = _firstprint;
  if (_firstprint - rowsavail <= _header_rows)
  {
    if (_items[_highlight]->posy() - rowsavail <= _header_rows)
      return highlightFirst();
    _firstprint = _header_rows;
  }
  else { _firstprint -= rowsavail; }

  // Highlight next selectable choice after _firstprint

  nitems = _items.size();
  highlight_found = false;
  for ( i = 0; i < nitems; i++ )
  {
    if ( (_items[i]->posy() >= _firstprint) && (_items[i]->selectable()) )
    {
      _highlight = i;
      highlight_found = true;
      break;
    }
  }
  if (! highlight_found) { highlightFirst(); }

  if (_firstprint == firstprintstore) { return 0; }
  else { return 1; }
}

int InputBox::highlightNextPage()
{
  int rows, cols, rowsavail, firstprintstore, lastprint;
  unsigned int i, nitems;
  bool highlight_found;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Determine how far to page

  nitems = _items.size();
  firstprintstore = _firstprint;
  lastprint = _items[nitems-1]->posy();
  if (_items[_highlight]->posy() + rowsavail-1 >= lastprint)
    return highlightLast();
  if (_firstprint + rowsavail-1 >= lastprint) { return highlightLast(); }
  else if (_firstprint + 2*(rowsavail-1) >= lastprint)
    _firstprint = lastprint - (rowsavail-1);
  else { _firstprint += rowsavail; }

  // Highlight next selectable choice after _firstprint

  highlight_found = false;
  for ( i = 0; i < nitems; i++ )
  {
    if ( (_items[i]->posy() >= _firstprint) && (_items[i]->selectable()) )
    {
      _highlight = i;
      highlight_found = true;
      break;
    }
  }
  if (! highlight_found) { highlightLast(); }
  
  if (_firstprint == firstprintstore) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Determine first line to print based on current highlighted and number of
available rows. Returns 1 if this number has changed; 0 if not.

*******************************************************************************/
int InputBox::determineFirstPrint()
{
  int rows, cols, rowsavail, firstprintstore, y;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  firstprintstore = _firstprint;

  rowsavail = rows-_reserved_rows;
  y = _items[_highlight]->posy();
  if (y < _firstprint) { _firstprint = y; }
  else if (y >= _firstprint + rowsavail) { _firstprint = y - rowsavail + 1; }

  if (firstprintstore == _firstprint) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Draws window border, message, and info

*******************************************************************************/
void InputBox::redrawFrame() const
{
  int rows, cols, msglen, i, left;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  msglen = _msg.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, 1, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, "fg_title", "bg_title");
  printSpaces(left-1);
  printToEol(_msg);
  colors.turnOff(_win);

  // Info on bottom of window

  msglen = _info.size();
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, "fg_info", "bg_info");
  printSpaces(left-1);
  printToEol(_info);
  colors.turnOff(_win);

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

  // Right border for header and footer

  mvwaddch(_win, 1, cols-1, ACS_VLINE);
  mvwaddch(_win, rows-2, cols-1, ACS_VLINE);

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

Redraws right border between header and footer and scroll indicators

*******************************************************************************/
void InputBox::redrawScrollIndicator() const
{
  int rows, cols, i, rowsavail, maxscroll, pos;
  bool need_up, need_dn;
  unsigned int nitems;

  // Check if a scroll indicator is needed

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  need_up = false;
  need_dn = false;
  nitems = _items.size();
  if (_firstprint != _header_rows) { need_up = true; }
  if (_items[nitems-1]->posy() > _firstprint + rows-_reserved_rows - 1)
    need_dn = true;

  // Draw right border

  for ( i = _header_rows; i < int(_header_rows)+rowsavail; i++ )
  {
    mvwaddch(_win, i, cols-1, ACS_VLINE);
  }

  // Draw up and down arrows

  if (need_up) { mvwaddch(_win, _header_rows, cols-1, ACS_UARROW); }
  if (need_dn) { mvwaddch(_win, _header_rows+rowsavail-1, cols-1, ACS_DARROW); }

  // Draw position indicator

  if ( (need_up) || (need_dn) )
  {
    maxscroll = _items[nitems-1]->posy() - rowsavail;
    pos = std::floor(double(_firstprint-_header_rows) /
                     double(maxscroll)*(rowsavail-1));
    mvwaddch(_win, _header_rows+pos, cols-1, ACS_DIAMOND);
  }
}

/*******************************************************************************

Redraws the previously and currently highlighted items

*******************************************************************************/
void InputBox::redrawChangedItems(bool force)
{
  int rows, cols, rowsavail, y_offset;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;
  y_offset = _firstprint - _header_rows;

  if (_prevhighlight < int(_items.size()))
  {
    if ( (_items[_prevhighlight]->posy() >= _firstprint) &&
         (_items[_prevhighlight]->posy() < _firstprint+rowsavail) )
      _items[_prevhighlight]->draw(y_offset, force, false);
  }
  if (_highlight < int(_items.size()))
    _items[_highlight]->draw(y_offset, force, true);
}

/*******************************************************************************

Redraws all items that are currently visible

*******************************************************************************/
void InputBox::redrawAllItems(bool force)
{
  int rows, cols, rowsavail, i, nitems, y, y_offset;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;
  y_offset = _firstprint - _header_rows;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ ) 
  { 
    y = _items[i]->posy();
    if (y >= _firstprint + rowsavail) { break; } 
    else if (y >= _firstprint) { _items[i]->draw(y_offset,
                                                 force, i==_highlight); }
  }
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
  _reserved_rows = 6;
  _header_rows = 3;
  _firstprint = _header_rows;
  _first_selectable = -1;
  _last_selectable = -1;
}

InputBox::InputBox(WINDOW *win, const std::string & msg)
{
  _win = win;
  _msg = msg;
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _highlight = 0;
  _prevhighlight = 0;
  _reserved_rows = 6;
  _header_rows = 3;
  _firstprint = _header_rows;
  _first_selectable = -1;
  _last_selectable = -1;
}

/*******************************************************************************

Adds item to the input box and sets first selectable item highlighted.

*******************************************************************************/
void InputBox::addItem(InputItem *item)
{
  int rows, cols;
  unsigned int nitems;

  getmaxyx(_win, rows, cols);
  nitems = _items.size();
  if (item->autoPosition()) { item->setPosition(nitems+_header_rows,1); }
  item->setWindow(_win);
  _items.push_back(item);
  highlightFirst();

  if (item->selectable())
  {
    if (_first_selectable == -1) { _first_selectable = nitems; }
    _last_selectable = nitems;
  }
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
  int reserved_cols, ymax, ymin, right;
  unsigned int nitems, i;

  // Get height needed (scrolling is not implemented for this - just give the
  // number of rows needed)

  ymin = 1000;
  ymax = 0;
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->posy() < ymin) { ymin = _items[i]->posy(); }
    if (_items[i]->posy() > ymax) { ymax = _items[i]->posy(); }
  }
  height = ymax - ymin + 1 + _reserved_rows;

  // Minimum usable width

  width = std::max(_msg.size(), _info.size());
  for ( i = 0; i < nitems; i++ )
  {
    right = _items[i]->posx() + _items[i]->width() - 1;
    if (right > width) { width = right; }
  } 
  reserved_cols = 2;       // For frame border
  width += reserved_cols;
}

void InputBox::preferredSize(int & height, int & width) const
{
  minimumSize(height, width);
}

/*******************************************************************************

Redraws box and all input items

*******************************************************************************/
void InputBox::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  if (_redraw_type == "all") 
  { 
    wclear(_win);
    colors.setBackground(_win, "fg_normal", "bg_normal");
    redrawFrame();
    redrawScrollIndicator();
    redrawAllItems(force);
  }
  else
  {
    redrawChangedItems(force);
    redrawScrollIndicator();
  }
  wrefresh(_win);
}

/*******************************************************************************

User interaction with input items in the box

*******************************************************************************/
std::string InputBox::exec()
{
  bool getting_input;
  int y_offset, check_redraw;
  std::string selection, retval;

  getting_input = true;
  while (getting_input)
  {
    // Draw input box elements
  
    draw();
    _redraw_type = "changed";
    y_offset = _firstprint - _header_rows;

    // Get user input from highlighted item

    selection = _items[_highlight]->exec(y_offset);
    if (selection == signals::resize)
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if ( (selection == signals::quit) || (selection == signals::keyEnter) )
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if (selection == signals::highlightFirst)
    { 
      if (highlightFirst() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightLast) 
    { 
      if (highlightLast() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightPrevPage)
    {
      if (highlightPreviousPage() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightNextPage)
    {
      if (highlightNextPage() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightPrev)
    { 
      if (_highlight == _first_selectable)
        check_redraw = highlightFirst();
      else { check_redraw = highlightPrevious(); }
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightNext)
    {
      if (_highlight == _last_selectable)
        check_redraw = highlightLast();
      else { check_redraw = highlightNext(); }
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
  }

  return retval;
}
