#include <string>
#include <curses.h>
#include <cmath>    // floor
#include "colors.h"
#include "ListBox.h"

std::string ListBox::resizeSignal = "__RESIZE__";
std::string ListBox::quitSignal = "__QUIT___";
std::string ListBox::keyRightSignal = "__RIGHT___";
std::string ListBox::keyLeftSignal = "__LEFT___";
std::string ListBox::tagSignal = "__TAG___";

/*******************************************************************************

Sets first item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightFirst()
{
  unsigned int retval;

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
  _highlight = _items.size() - 1;
  return determineFirstPrint();
}

/*******************************************************************************

Sets previous item to be highlighted. Return value is 0 if _firstprint has not
changed, 1 if it has.

*******************************************************************************/
int ListBox::highlightPrevious()
{
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
  if (_highlight == _items.size()-1) { return 0; }
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
  int rows, cols, rowsavail;
  unsigned int nitems;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-2;

  // Determine how far to page and which item to highlight

  nitems = _items.size();
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

  getmaxyx(_win, rows, cols);
  rowsavail = rows-2;

  // Determine how far to page

  if (int(_firstprint) - rowsavail <= 0)
  {
    if (int(_highlight) - rowsavail <= 0) { return highlightFirst(); }
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
  int rows, cols, rowsavail;
  unsigned int firstprintstore;

  getmaxyx(_win, rows, cols);
  firstprintstore = _firstprint;

  rowsavail = rows-2;
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

Draws window border and title

*******************************************************************************/
void ListBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, left, right, i;
  double mid;

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

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Arrows on right border to indicate scrolling

  if (_firstprint == 0) { mvwaddch(_win, 1, cols-1, ACS_VLINE); }
  else { mvwaddch(_win, 1, cols-1, ACS_DIAMOND); }
  if (_items.size() <= _firstprint + rows-2)
  {
    mvwaddch(_win, rows-2, cols-1, ACS_VLINE);
  }
  else { mvwaddch(_win, rows-2, cols-1, ACS_DIAMOND); }

  // Rest of right border
  
  for ( i = 2; i < rows-2; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void ListBox::redrawSingleItem(unsigned int idx)
{
  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+1, 1);

  // Turn on bold text for tagged items

  if (_items[idx].tagged) { wattron(_win, A_BOLD); }

  // Turn on highlight color

  if (idx == _highlight)
  {
    if (_activated)
    {
      if (colors::highlighted_active != -1) 
      { 
        wattron(_win, COLOR_PAIR(colors::highlighted_active));
      }
      else { wattron(_win, A_REVERSE); }
    }
    else
    {
      if (colors::highlighted_inactive != -1) 
      { 
        wattron(_win, COLOR_PAIR(colors::highlighted_inactive));
      }
      else { wattron(_win, A_REVERSE); }
    }
    _prevhighlight = _highlight; // Save highlight idx for redrawing later
                                 // Note: prevents this method from being const
  } 

  // Print item

  if (_items[idx].tagged) { printToEol("*" + _items[idx].name); }
  else { printToEol(_items[idx].name); }

  // Turn off bold text for tagged items
                                   
  if (_items[idx].tagged) { wattroff(_win, A_BOLD); }

  // Turn off highlight color

  if (idx == _highlight)
  {
    if (_activated)
    {
      if (colors::highlighted_active != -1) 
      { 
        wattroff(_win, COLOR_PAIR(colors::highlighted_active));
      }
      else { wattroff(_win, A_REVERSE); }
    }
    else
    {
      if (colors::highlighted_inactive != -1) 
      { 
        wattroff(_win, COLOR_PAIR(colors::highlighted_inactive));
      }
      else { wattroff(_win, A_REVERSE); }
    }
  } 
}

/*******************************************************************************

Redraws the previously and currently highlighted items

*******************************************************************************/
void ListBox::redrawChangedItems()
{
  int rows, cols, rowsavail;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-2;

  // Draw previously and currently highlighted items

  if ( (_prevhighlight >= _firstprint) &&
       (_prevhighlight < _firstprint+rowsavail) )
  {
    redrawSingleItem(_prevhighlight);
  }
  redrawSingleItem(_highlight);
}

/******************************************************************************

Redraws all items

*******************************************************************************/
void ListBox::redrawAllItems()
{
  unsigned int i;
  int rows, cols, rowsavail;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-2;

  if (_items.size() == 0)
  {
    wrefresh(_win);
    return;
  }
  for ( i = _firstprint; i < _firstprint+rowsavail; i++ )
  {
    redrawSingleItem(i);
    if (i == _items.size()-1) { break; }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
ListBox::ListBox()
{
  _win = NULL;
  _name = "";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
}

ListBox::ListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
}

/*******************************************************************************

Edit list items

*******************************************************************************/
void ListBox::addItem(const std::string & name, bool tagged)
{
  listitem item;

  item.name = name;
  item.tagged = tagged;
  _items.push_back(item);
}

void ListBox::removeItem(unsigned int idx)
{
  if (idx <= _items.size()) { _items.erase(_items.begin()+idx); }
}

void ListBox::toggleItemTag(unsigned int idx)
{
  if (idx <= _items.size()) { _items[idx].tagged = !_items[idx].tagged; }
}

void ListBox::clearList()
{
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
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

/*******************************************************************************

Draws list box (frame, items, etc.)

*******************************************************************************/
void ListBox::draw()
{
  wclear(_win);
  redrawFrame();
  redrawAllItems();
  wrefresh(_win);
}

/*******************************************************************************

User interaction loop

*******************************************************************************/
std::string ListBox::exec()
{
  int ch, check_redraw;
  bool getting_input;
  std::string redraw_type, retval;

  const int MY_ESC = 27;

  // Don't bother if there are no items

  if (_items.size() == 0) { return "EMPTY"; }

  getting_input = true;
  redraw_type = "all";

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  // Handle key input events

  while (1)
  {
    // Normally redraw the frame, since scroll indicators can change

    if (redraw_type == "all") { wclear(_win); }
    if (redraw_type != "none") { redrawFrame(); }
    if ( (redraw_type == "all") || (redraw_type == "items") ) { 
                                                              redrawAllItems(); }
    else if (redraw_type == "changed") { redrawChangedItems(); }
    wrefresh(_win);

    if (! getting_input) { break; }

    // Get user input

    switch (ch = getch()) {

      // Enter key: return name of highlighted item

      case '\n':
      case '\r':
      case KEY_ENTER:
        getting_input = false;
        retval = _items[_highlight].name; 
        redraw_type = "none";
        break;

      // Left or right key: return to calling function to decide what to do next

      case KEY_LEFT:
        getting_input = false;
        retval = keyLeftSignal;
        redraw_type = "none";
        break;
      case KEY_RIGHT:
        getting_input = false;
        retval = keyRightSignal;
        redraw_type = "none";
        break;
 
      // Arrows/Home/End/PgUp/Dn: change highlighted value

      case KEY_UP:
        check_redraw = highlightPrevious();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_DOWN:
        check_redraw = highlightNext();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_PPAGE:
        check_redraw = highlightPreviousPage();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_NPAGE:
        check_redraw = highlightNextPage();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_HOME:
        check_redraw = highlightFirst();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_END:
        check_redraw = highlightLast();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;

      // Resize signal: redraw (may not work with some curses implementations)

      case KEY_RESIZE:
        getting_input = false;
        retval = resizeSignal;
        break;

      // Toggle item tag

      case 't':
        getting_input = false;
        retval = tagSignal;
        toggleItemTag(_highlight);
        check_redraw = highlightNext();
        break;

      // Quit key

      case MY_ESC:
        getting_input = false;
        retval = quitSignal;
        redraw_type = "none";
        break;

      default:
        redraw_type = "none";
        break;
    }
  }
  return retval;
}
