#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max
#include "Color.h"
#include "color_settings.h"
#include "ListItem.h"
#include "ListBox.h"

using namespace color;

std::string ListBox::resizeSignal = "__RESIZE__";
std::string ListBox::quitSignal = "__QUIT__";
std::string ListBox::highlightSignal = "__HIGHLIGHT__";
std::string ListBox::keyTabSignal = "__TAB__";

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
  {
    _firstprint = _highlight - rowsavail + 1;
  }

  if (firstprintstore == _firstprint) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Prints to end of line, padding with spaces and avoiding borders

*******************************************************************************/
void ListBox::printToEol(const std::string & msg) const
{
  int i, y, x, rows, cols, nspaces, msglen;

  getmaxyx(_win, rows, cols);
  getyx(_win, y, x);

  /* Math: Number of columns: cols
           Cursor position: x
           Number of spaces that can be printed to right = cols-1-x
           => Because the last column is taken up by the border */

  msglen = msg.size();
  if (msglen > cols-1-x) { wprintw(_win, msg.substr(0, cols-1-x).c_str()); }
  else
  {
    nspaces = std::max(cols-1-x-msglen, 0);
    wprintw(_win, msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { wprintw(_win, " "); }
  }
}

/*******************************************************************************

Prints a given number of spaces

*******************************************************************************/
void ListBox::printSpaces(unsigned int nspaces) const
{
  unsigned int i;

  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
}

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void ListBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, i;
  int left, right;
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

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Symbols on right border to indicate scrolling

  if (_firstprint != 0) { mvwaddch(_win, 1, cols-1, ACS_UARROW); }
  if (_items.size() > _firstprint + rows-_reserved_rows)
  {
    mvwaddch(_win, rows-2, cols-1, ACS_DARROW);
  }
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void ListBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int color_pair;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+1, 1);

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
    { 
      redrawSingleItem(_prevhighlight); 
    }
  }
  if (_highlight < int(_items.size())) 
  { 
    redrawSingleItem(_highlight); 
  }
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
  _win = NULL;
  _name = "";
  _redraw_type = "all";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 2;
}

ListBox::ListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _redraw_type = "all";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _reserved_rows = 2;
}

/*******************************************************************************

Edit list items

*******************************************************************************/
void ListBox::addItem(ListItem *item) { _items.push_back(item); }
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
void ListBox::setWindow(WINDOW *win) { _win = win; }
void ListBox::setName(const std::string & name) { _name = name; }
void ListBox::setActivated(bool activated) { _activated = activated; }

/*******************************************************************************

Get attributes

*******************************************************************************/
const std::string & ListBox::name() const { return _name; }
unsigned int ListBox::highlight() const { return _highlight; }
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

Returns pointer to highlighted item

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
  int pair_normal;

  // Draw list elements

  if ( (_redraw_type == "all") || force )
  { 
    wclear(_win); 
    pair_normal = colors.pair(fg_normal, bg_normal);
    if (pair_normal != -1) { wbkgd(_win, COLOR_PAIR(pair_normal)); }
  }
  if ( (_redraw_type != "none") || force ) { redrawFrame(); }
  if ( (_redraw_type == "all") || (_redraw_type == "items") || force ) { 
                                                            redrawAllItems(); }
  else if (_redraw_type == "changed") { redrawChangedItems(); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or selected item name

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

    // Enter key: return name of highlighted item

    case '\n':
    case '\r':
    case KEY_ENTER:
      retval = _items[_highlight]->name();
      _redraw_type = "all";
      break;

    // Tab key: return keyTabSignal

    case MY_TAB:
      retval = keyTabSignal;
      _redraw_type = "changed";
      break;

    // Arrows/Home/End/PgUp/Dn: change highlighted value

    case KEY_UP:
      retval = highlightSignal;
      check_redraw = highlightPrevious();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_DOWN:
      retval = highlightSignal;
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_PPAGE:
      retval = highlightSignal;
      check_redraw = highlightPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_NPAGE:
      retval = highlightSignal;
      check_redraw = highlightNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_HOME:
      retval = highlightSignal;
      check_redraw = highlightFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_END:
      retval = highlightSignal;
      check_redraw = highlightLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    // Resize signal: redraw (may not work with some curses implementations)

    case KEY_RESIZE:
      retval = resizeSignal;
      _redraw_type = "all";
      break;

    // Quit key

    case MY_ESC:
      retval = quitSignal;
      _redraw_type = "all";
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  return retval;
}
