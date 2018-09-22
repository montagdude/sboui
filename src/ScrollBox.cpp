#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "ListItem.h"
#include "AbstractListBox.h"
#include "ScrollBox.h"
#include "MouseEvent.h"

/*******************************************************************************

Scrolls to first item. Return value is 0 if _firstprint has not changed, 1 if it
has.

*******************************************************************************/
int ScrollBox::scrollFirst()
{
  unsigned int retval;

  if (_items.size() == 0) { return 0; }

  if (_firstprint == 0) { retval = 0; }
  else { retval = 1; }
  _firstprint = 0;

  return retval;
}

/*******************************************************************************

Scrolls to last item. Return value is 0 if _firstprint has not changed, 1 if it
has.

*******************************************************************************/
int ScrollBox::scrollLast()
{
  int rows, cols, firstprintstore, rowsavail;

  if (_items.size() == 0) { return 0; }
  
  getmaxyx(_win, rows, cols);
  firstprintstore = _firstprint;
  rowsavail = rows - _reserved_rows;
  
  _firstprint = std::max(int(_items.size())-rowsavail, 0);
  if (firstprintstore == _firstprint) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Scrolls to the previous item. Returns 0 if _firstprint has not changed, 1 if it
has.

*******************************************************************************/
int ScrollBox::scrollUp()
{
  if (_items.size() == 0) { return 0; }

  if (_firstprint > 0)
  {
    _firstprint--;
    return 1;
  }
  else { return 0; }
}

/*******************************************************************************

Scrolls to the next item. Returns 0 if _firstprint has not changed, 1 if it
has.

*******************************************************************************/
int ScrollBox::scrollDown()
{
  int rows, cols, rowsavail;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  rowsavail = rows - _reserved_rows;
  if (_firstprint < int(_items.size()) - rowsavail)
  {
    _firstprint++;
    return 1;
  }
  else { return 0; }
}

/*******************************************************************************

Scrolls 1 page up. Return value of 0 means that _firstprint hasn't changed;
1 means it has.

*******************************************************************************/
int ScrollBox::scrollPreviousPage()
{
  int rows, cols, rowsavail;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Determine how far to page

  if (_firstprint - rowsavail <= 0) { return scrollFirst(); }
  else { _firstprint -= rowsavail; }

  return 1;
}

/*******************************************************************************

Scrolls 1 page down. Return value of 0 means that _firstprint hasn't changed;
1 means it has.

*******************************************************************************/
int ScrollBox::scrollNextPage()
{
  int rows, cols, rowsavail, nitems, firstprintstore;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Determine how far to page

  firstprintstore = _firstprint;
  nitems = _items.size();
  if (_firstprint + 2*rowsavail >= nitems-1) { return scrollLast(); }
  else { _firstprint += rowsavail; }

  if (firstprintstore == _firstprint) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void ScrollBox::redrawFrame() const
{
  int rows, cols, namelen, i, left, right;
  double mid;

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

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void ScrollBox::redrawSingleItem(unsigned int idx)
{
  // Go to item location and print item

  wmove(_win, idx-_firstprint+_header_rows, 1);
  printToEol(_items[idx]->name());
}

/******************************************************************************

Redraws all items

*******************************************************************************/
void ScrollBox::redrawAllItems()
{
  int rows, cols, rowsavail, i;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  if (_items.size() == 0)
  {
    wrefresh(_win);
    return;
  }
  for ( i = _firstprint; i < _firstprint+rowsavail; i++ )
  {
    redrawSingleItem(i);
    if (i == int(_items.size())-1) { break; }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
ScrollBox::ScrollBox() {}

ScrollBox::ScrollBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void ScrollBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols;
  unsigned int i, nitems;

  // Minimum usable height

  height = _reserved_rows + 2;

  // Minimum usable width

  width = _name.size();
  reserved_cols = 2;
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void ScrollBox::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols, widthpadding;
  unsigned int i, nitems;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size();
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

/*******************************************************************************

Handles mouse events

*******************************************************************************/
std::string ScrollBox::handleMouseEvent(const MouseEvent * event)
{
  //FIXME: implement
  return signals::nullEvent;
}

/*******************************************************************************

Draws scroll box (frame, items, etc.) as needed

*******************************************************************************/
void ScrollBox::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  // Draw list elements

  if (_redraw_type == "all")
  { 
    clearWindow();
    colors.setBackground(_win, "fg_normal", "bg_normal");
  }
  if (_redraw_type != "none")
  {
    redrawFrame();
    redrawScrollIndicator();
  }
  if ( (_redraw_type == "all") || (_redraw_type == "items")) { 
                                                            redrawAllItems(); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string ScrollBox::exec(MouseEvent * mevent)
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;
  const int MY_TAB = 9;

  // Draw list elements

  draw();

  // Get user input

  switch (ch = getch()) {

    // Enter key: accept selection

    case '\n':
    case '\r':
    case KEY_ENTER:
      retval = signals::keyEnter;
      _redraw_type = "all";
      break;

    // Tab key: return keyTab

    case MY_TAB:
      retval = signals::keyTab;
      _redraw_type = "changed";
      break;

    // Arrows/Home/End/PgUp/Dn: scrolling

    case KEY_UP:
      retval = signals::scroll;
      check_redraw = scrollUp();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_DOWN:
      retval = signals::scroll;
      check_redraw = scrollDown();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_PPAGE:
      retval = signals::scroll;
      check_redraw = scrollPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_NPAGE:
      retval = signals::scroll;
      check_redraw = scrollNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_HOME:
      retval = signals::scroll;
      check_redraw = scrollFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_END:
      retval = signals::scroll;
      check_redraw = scrollLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;

    // Resize signal

    case KEY_RESIZE:
      retval = signals::resize;
      _redraw_type = "all";
      break;

    // Quit key

    case MY_ESC:
      retval = signals::quit;
      _redraw_type = "all";
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  return retval;
}
