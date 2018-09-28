#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include <chrono>    // sleep_for
#include <thread>    // this_thread
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

Scrolls to a page based on a fractional value of all items in the list. Return
value of 0 means that _firstprint hasn't changed; 1 means it has.

*******************************************************************************/
int ScrollBox::scrollFractional(const double & frac)
{
  int rows, cols, rowsavail, maxscroll, firstprintstore;

  if (_items.size() == 0) { return 0; }
  if (frac < 0.) { return 0; }
  else if (frac > 1.) { return 0; }

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  firstprintstore = _firstprint;
  maxscroll = _items.size() - rowsavail;
  if (frac < 0.5)
    _firstprint = std::ceil(frac*maxscroll);
  else
    _firstprint = std::floor(frac*maxscroll);
  if (_firstprint == firstprintstore)
    return 0;
  else
    return 1;
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

/*******************************************************************************

Redraws scroll indicator. Differs from regular list box scroll indicator in that
it is based on pages rather than highlighted item (there is no highlighted item
in this class).

*******************************************************************************/
void ScrollBox::redrawScrollIndicator() const
{
  int rows, cols, i, rowsavail, maxscroll, pos;
  bool need_up, need_dn;

  // Check if a scroll indicator is needed

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  need_up = false;
  need_dn = false;
  if (_firstprint != 0) { need_up = true; }
  if (_items.size() > _firstprint + rows-_reserved_rows) { need_dn = true; }

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
    maxscroll = _items.size() - rowsavail;
    pos = std::floor(double(_firstprint)/double(maxscroll)*(rowsavail-1));
    mvwaddch(_win, _header_rows+pos, cols-1, ACS_DIAMOND);
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

Handles mouse events

*******************************************************************************/
std::string ScrollBox::handleMouseEvent(MouseEvent * mevent)
{
  int rows, cols, begy, begx, ycurs, xcurs, rowsavail, check_redraw;
  unsigned i, nbuttons;
  double frac;

  if (_items.size() == 0)
    return signals::nullEvent;

  getmaxyx(_win, rows, cols);
  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;
  xcurs = mevent->x() - begx;
  rowsavail = rows-_reserved_rows;

  if ( (mevent->button() == 1) || (mevent->button() == 2) )
  {
    // Check for clicking on buttons

    nbuttons = _buttons.size();
    if ( (nbuttons > 0) && (ycurs == rows-2) )
    {
      for ( i = 0; i < nbuttons; i++ )
      {
        if ( (xcurs >= _button_left[i]) && (xcurs <= _button_right[i]) )
        {
          _highlighted_button = i;

          // Redraw and pause for .1 seconds to make button selection visible

          _redraw_type = "buttons";
          draw(true);
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          return _button_signals[i];
        }
      }
      return signals::nullEvent;
    }

    else if ( (ycurs < int(_header_rows)) ||
              (ycurs >= int(_header_rows)+rowsavail) )
      return signals::nullEvent;
    else if ( (xcurs < cols-1) || (xcurs >= cols) )
      return signals::nullEvent;

    // Check for clicking on scroll area

    else if (xcurs == cols-1)
    {
      if (ycurs == int(_header_rows))
      {
        if (_firstprint != 0)
        {
          check_redraw = scrollPreviousPage();
          if (check_redraw == 1)
            _redraw_type = "all";
          else
            _redraw_type = "none";
          return signals::scroll;
        }
        else
          return signals::nullEvent;
      }
      else if (ycurs == int(_header_rows)+rowsavail-1)
      {
        if (_items.size() > _firstprint + rows-_reserved_rows)
        {
          check_redraw = scrollNextPage();
          if (check_redraw == 1)
            _redraw_type = "all";
          else
            _redraw_type = "none";
          return signals::scroll;
        }
        else
          return signals::nullEvent;
      }
      else
      {
        frac = double(ycurs-_header_rows) /
               double(rowsavail-1);
        check_redraw = scrollFractional(frac);
        if (check_redraw == 1)
          _redraw_type = "all";
        else
          _redraw_type = "none";
        return signals::scroll;
      }
    }

    else
      return signals::nullEvent;
  }

  // Scroll wheel

  else if (mevent->button() == 4)
  {
    check_redraw = scrollPreviousPage();
    if (check_redraw == 1)
      _redraw_type = "all";
    else
      _redraw_type = "none";
    return signals::scroll;
  }
  else if (mevent->button() == 5)
  {
    check_redraw = scrollNextPage();
    if (check_redraw == 1)
      _redraw_type = "all";
    else
      _redraw_type = "none";
    return signals::scroll;
  }
  else
    return signals::nullEvent;

}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string ScrollBox::exec(MouseEvent * mevent)
{
  int ch, check_redraw;
  std::string retval;
  MEVENT event;

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

    // Mouse

    case KEY_MOUSE:
      if ( (getmouse(&event) == OK) && mevent )
      {
        mevent->recordClick(event);
        _redraw_type = "none";
        retval = signals::mouseEvent;
      }
      else
        return signals::nullEvent;
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  return retval;
}
