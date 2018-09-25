#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // max, min
#include <chrono>     // sleep_for
#include <thread>     // this_thread
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "backend.h"  // find_name_in_list
#include "ListItem.h"
#include "AbstractListBox.h"
#include "ListBox.h"
#include "MouseEvent.h"

/*******************************************************************************

Sets first item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightFirst()
{
  unsigned int retval;

  if (_items.size() == 0) { return 0; }

  _prevhighlight = _highlight;
  _highlight = 0;
  if (_firstprint == 0) { retval = 0; }
  else { retval = 1; }
  _firstprint = 0;

  return retval;
}

/*******************************************************************************

Sets last item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightLast()
{
  if (_items.size() == 0) { return 0; }

  _prevhighlight = _highlight;
  _highlight = std::max(int(_items.size()) - 1, 0);
  return determineFirstPrint();
}

/*******************************************************************************

Sets previous item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightPrevious()
{
  if (_items.size() == 0) { return 0; }

  if (_highlight == 0) { return 0; }
  else
  {
    _prevhighlight = _highlight;
    _highlight -= 1;
    return determineFirstPrint();
  }
}

/*******************************************************************************

Sets next item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightNext()
{
  if (_items.size() == 0) { return 0; }

  if (_highlight == int(_items.size())-1) { return 0; }
  else
  {
    _prevhighlight = _highlight;
    _highlight += 1;
    return determineFirstPrint();
  }
}

/*******************************************************************************

Scrolls 1 page down. Return value of 0 means that _firstprint hasn't changed;
1 means it has.

*******************************************************************************/
int ListBox::highlightNextPage()
{
  int rows, cols, rowsavail, nitems;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Determine how far to page and which item to highlight

  nitems = _items.size();
  if (_highlight + rowsavail >= nitems-1) { return highlightLast(); }
  if (_firstprint + rowsavail >= nitems-1) { return highlightLast(); }
  else if (_firstprint + 2*rowsavail > nitems-1)
  {
    _firstprint = nitems - rowsavail;
  }
  else { _firstprint += rowsavail; }

  // Determine which choice to highlight

  _prevhighlight = _highlight;
  _highlight += rowsavail;

  return 1;
}

/*******************************************************************************

Scrolls 1 page up. Return value of 0 means that _firstprintstore hasn't changed;
1 means it has.

*******************************************************************************/
int ListBox::highlightPreviousPage()
{
  int rows, cols, rowsavail;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Determine how far to page

  if (_firstprint - rowsavail <= 0)
  {
    if (_highlight - rowsavail <= 0) { return highlightFirst(); }
    _firstprint = 0;
  }
  else { _firstprint -= rowsavail; }

  // Determine which choice to highlight

  _prevhighlight = _highlight;
  _highlight -= rowsavail;

  return 1;
}

/*******************************************************************************

Scrolls to a page based on a fractional value of all items in the list. Return
value of 0 means that _firstprint hasn't changed; 1 means it has.

*******************************************************************************/
int ListBox::highlightFractional(const double & frac)
{
  if (_items.size() == 0) { return 0; }
  if (frac < 0.) { return 0; }
  else if (frac > 1.) { return 0; }

  _prevhighlight = _highlight;
  _highlight = std::floor(frac*double(_items.size()-1));
  return determineFirstPrint();
}

/*******************************************************************************

Highlights next button. Return value of 0 means that the highlighted button has
not changed; 1 means that it has.

*******************************************************************************/
int ListBox::highlightNextButton()
{
  if (_buttons.size() < 2)
    return 0;
  else if (_highlighted_button < int(_buttons.size())-1)
  {
    _highlighted_button += 1;
    return 1;
  }
  else
    return 0;
}

/*******************************************************************************

Highlights previous button. Return value of 0 means that the highlighted button
has not changed; 1 means that it has.

*******************************************************************************/
int ListBox::highlightPreviousButton()
{
  if (_buttons.size() < 2)
    return 0;
  else if (_highlighted_button > 0)
  {
    _highlighted_button -= 1;
    return 1;
  }
  else
    return 0;
}

/*******************************************************************************

Determine first item to print based on current highlighted and number of
available rows. Returns 1 if this number has changed; 0 if not.

*******************************************************************************/
int ListBox::determineFirstPrint()
{
  int rows, cols, rowsavail, firstprintstore;

  if (_items.size() == 0) { return 0; }

  getmaxyx(_win, rows, cols);
  firstprintstore = _firstprint;

  rowsavail = rows-_reserved_rows;
  if (_highlight < _firstprint) { _firstprint = _highlight; }
  else if (_highlight >= _firstprint + rowsavail)
    _firstprint = _highlight - rowsavail + 1;

  if (firstprintstore == _firstprint) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void ListBox::redrawFrame()
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

  //FIXME: draw buttons if present

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
void ListBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int color_pair1, color_pair2, len, i, hidx;

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
    color_pair1 = colors.getPair("fg_normal", "bg_normal");
    color_pair2 = colors.getPair("hotkey", "bg_normal");
  }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item

  len = _items[idx]->name().size();
  hidx = _items[idx]->hotKey();
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
  printToEol(std::string(""));

  // Turn off highlight color

  if (colors.turnOff(_win) != 0)
  { 
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Redraws the previously and currently highlighted items

*******************************************************************************/
void ListBox::redrawChangedItems()
{
  int rows, cols, rowsavail;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  // Draw previously and currently highlighted items

  if ( (_prevhighlight >= _firstprint) &&
       (_prevhighlight < _firstprint+rowsavail) )
  {
    if (_prevhighlight < int(_items.size())) 
      redrawSingleItem(_prevhighlight); 
  }
  if (_highlight < int(_items.size())) 
    redrawSingleItem(_highlight); 
}

/******************************************************************************

Redraws all items

*******************************************************************************/
void ListBox::redrawAllItems()
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
ListBox::ListBox()
{
  _activated = true;
  _buttons.resize(0);
  _button_signals.resize(0);
  _highlighted_button = -1;
}

ListBox::ListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _activated = true;
  _buttons.resize(0);
  _button_signals.resize(0);
  _highlighted_button = -1;
}

/*******************************************************************************

Edit list items

*******************************************************************************/
void ListBox::removeItem(unsigned int idx)
{
  if (idx <= _items.size()) { _items.erase(_items.begin()+idx); }
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
}

void ListBox::clearList()
{
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void ListBox::setActivated(bool activated) { _activated = activated; }
int ListBox::setHighlight(int highlight)
{
  if ( (highlight >= 0) && (highlight < int(_items.size())) )
  {
    _prevhighlight = _highlight;
    _highlight = highlight;
    determineFirstPrint();
    return 0;
  }
  else if (highlight >= int(_items.size()))
  {
    highlightLast();
    return 1;
  }
  else { return 1; }
}

int ListBox::setHighlight(const std::string & name)
{
  int i, nitems, retval;

  retval = 1;
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->name() == name) { retval = setHighlight(i); }
  }
  return retval;
}

void ListBox::addButton(const std::string & button, const std::string & signal)
{
  _buttons.push_back(button);
  _button_signals.push_back(signal);
  _button_left.resize(_buttons.size());
  _button_right.resize(_buttons.size());
  _highlighted_button = 0;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
unsigned int ListBox::highlight() const { return _highlight; }
const std::string & ListBox::highlightedName() const
{ 
  return _items[_highlight]->name();
}

void ListBox::minimumSize(int & height, int & width) const
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

void ListBox::preferredSize(int & height, int & width) const
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

int ListBox::highlightedButton() const { return _highlighted_button; }

/*******************************************************************************

Sets highlight based on a search. It doesn't have to be an exact match; the
first item that begins with the typed string will be highlighted. Returns 0
on successful match or 1 otherwise.

*******************************************************************************/
int ListBox::highlightSearch(const std::string & pattern)
{
  int check, idx, lbound, rbound;

  lbound = 0;
  rbound = numItems()-1;
  check = find_name_in_list(pattern, _items, idx, lbound, rbound, false);
  if (check == 0)
  {
    _highlight = idx;
    determineFirstPrint();
    draw(true);
  }

  return 0; 
}

/*******************************************************************************

Returns pointer to item

*******************************************************************************/
ListItem * ListBox::highlightedItem()
{
  if (_items.size() == 0) { return NULL; }
  else { return _items[_highlight]; }
}

/*******************************************************************************

Handles mouse events

*******************************************************************************/
std::string ListBox::handleMouseEvent(MouseEvent * mevent)
{
  int rows, cols, begy, begx, ycurs, xcurs, rowsavail, check_redraw;
  unsigned int i, nbuttons;
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

    if (ycurs == rows-2)
    {
      nbuttons = _buttons.size();
      if (nbuttons > 0)
      {
        for ( i = 0; i < nbuttons; i++ )
        {
          if ( (xcurs >= _button_left[i]) && (xcurs <= _button_right[i]) )
          {
            _highlighted_button = i;

            // Redraw and pause for .1 seconds to make button selection visible

            _redraw_type = "changed";  //FIXME: buttons
            draw(true);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return _button_signals[i];
          }
        }
        return signals::nullEvent;
      }
      else
        return signals::nullEvent;
    }

    else if ( (ycurs < int(_header_rows)) ||
              (ycurs >= int(_header_rows)+rowsavail) )
      return signals::nullEvent;
    else if ( (xcurs < 1) || (xcurs >= cols) )
      return signals::nullEvent;

    // Check for clicking on scroll area

    else if (xcurs == cols-1)
    {
      if (ycurs == int(_header_rows))
      {
        if (_firstprint != 0)
        {
          check_redraw = highlightPreviousPage();
          if (check_redraw == 1)
            _redraw_type = "all";
          else
            _redraw_type = "changed";
          return signals::highlight;
        }
        else
          return signals::nullEvent;
      }
      else if (ycurs == int(_header_rows)+rowsavail-1)
      {
        if (_items.size() > _firstprint + rows-_reserved_rows)
        {
          check_redraw = highlightNextPage();
          if (check_redraw == 1)
            _redraw_type = "all";
          else
            _redraw_type = "changed";
          return signals::highlight;
        }
        else
          return signals::nullEvent;
      }
      else
      {
        frac = double(ycurs-_header_rows) /
               double(rowsavail-1);
        check_redraw = highlightFractional(frac);
        if (check_redraw == 1)
          _redraw_type = "all";
        else
          _redraw_type = "changed";
        return signals::highlight;
      }
    }

    // Clicked on empty space in the list box

    else if (_firstprint + (ycurs - _header_rows) >= _items.size())
      return signals::nullEvent;

    // Clicked in the box -> change highlighted item

    else
    {
      _prevhighlight = _highlight;
      _highlight = _firstprint + (ycurs - _header_rows);
      if (determineFirstPrint() == 1)
        _redraw_type = "all";
      else
        _redraw_type = "changed";

      if (mevent->doubleClick())
        return signals::keyEnter;
      else
        return signals::highlight;
    }
  }

  // Scroll wheel

  else if (mevent->button() == 4)
  {
    check_redraw = highlightPreviousPage();
    if (check_redraw == 1)
      _redraw_type = "all";
    else
      _redraw_type = "changed";
    return signals::highlight;
  }
  else if (mevent->button() == 5)
  {
    check_redraw = highlightNextPage();
    if (check_redraw == 1)
      _redraw_type = "all";
    else
      _redraw_type = "changed";
    return signals::highlight;
  }
  else
    return signals::nullEvent;
}

/*******************************************************************************

Draws list box (frame, items, etc.) as needed

*******************************************************************************/
void ListBox::draw(bool force)
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
  else if (_redraw_type == "changed") { redrawChangedItems(); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string ListBox::exec(MouseEvent * mevent)
{
  int ch, check_redraw;
  std::string retval;
  MEVENT event;

  const int MY_ESC = 27;
  const int MY_TAB = 9;

  // Highlight first entry on first display

  if ( (_highlight == 0) && (_prevhighlight == 0) ) { highlightFirst(); }

  // Draw list elements

  draw();

  // Get user input

  switch (ch = getch()) {

    // Enter key: accept selection

    case '\n':
    case '\r':
    case KEY_ENTER:
      if (_buttons.size() > 0)
        retval = _button_signals[_highlighted_button];
      else
        retval = signals::keyEnter;
      _redraw_type = "all";
      break;

    // Tab key: return keyTab

    case MY_TAB:
      retval = signals::keyTab;
      _redraw_type = "changed";
      break;

    // Arrows/Home/End/PgUp/Dn: change highlighted value

    case KEY_UP:
      retval = signals::highlight;
      check_redraw = highlightPrevious();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_DOWN:
      retval = signals::highlight;
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_PPAGE:
      retval = signals::highlight;
      check_redraw = highlightPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_NPAGE:
      retval = signals::highlight;
      check_redraw = highlightNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_HOME:
      retval = signals::highlight;
      check_redraw = highlightFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_END:
      retval = signals::highlight;
      check_redraw = highlightLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    // Right/Left: change highlighted button

    case KEY_RIGHT:
      retval = signals::keyRight;
      check_redraw = highlightNextButton();
      if (check_redraw == 1) { _redraw_type = "changed"; }
      else { _redraw_type = "none"; }
      break;

    case KEY_LEFT:
      retval = signals::keyLeft;
      check_redraw = highlightPreviousButton();
      if (check_redraw == 1) { _redraw_type = "changed"; }
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
        _redraw_type = "changed";
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
