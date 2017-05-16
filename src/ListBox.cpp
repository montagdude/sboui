#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // max, min
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "backend.h"  // find_name_in_list
#include "ListItem.h"
#include "AbstractListBox.h"
#include "ListBox.h"

/*******************************************************************************

Sets first item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightFirst()
{
  unsigned int retval;

  if (_items.size() == 0) { return 0; }

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
  else if (_firstprint + 2*rowsavail >= nitems-1)
  {
    _firstprint = nitems - rowsavail;
  }
  else { _firstprint += rowsavail; }

  // Determine which choice to highlight

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

  _highlight -= rowsavail;

  return 1;
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
void ListBox::redrawFrame() const
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
      fg = color_settings.fg_highlight_active; 
      bg = color_settings.bg_highlight_active; 
    }
    else
    {
      fg = color_settings.fg_highlight_inactive; 
      bg = color_settings.bg_highlight_inactive; 
    }
    color_pair1 = colors.getPair(fg, bg);
    color_pair2 = colors.getPair(color_settings.hotkey, bg);
    if (colors.turnOn(_win, color_pair1) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else
  {
    color_pair1 = colors.getPair(color_settings.fg_normal,
                                 color_settings.bg_normal);
    color_pair2 = colors.getPair(color_settings.hotkey,
                                 color_settings.bg_normal);
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
  _highlight = 0;
  _prevhighlight = 0;
  _activated = true;
}

ListBox::ListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _highlight = 0;
  _prevhighlight = 0;
  _activated = true;
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

Draws list box (frame, items, etc.) as needed

*******************************************************************************/
void ListBox::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  // Draw list elements

  if (_redraw_type == "all")
  { 
    wclear(_win); 
    colors.setBackground(_win, color_settings.fg_normal,
                               color_settings.bg_normal);
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
std::string ListBox::exec()
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;
  const int MY_TAB = 9;

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

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

    // Left/right arrows

    case KEY_LEFT:
      retval = signals::keyLeft;
      _redraw_type = "changed";
      break;
    case KEY_RIGHT:
      retval = signals::keyRight;
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
