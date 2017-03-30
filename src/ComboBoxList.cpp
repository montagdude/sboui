#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "ListItem.h"
#include "ComboBoxList.h"

using namespace color_settings;

/*******************************************************************************

There is no frame, and scroll arrows are handled by redrawSingleItem, so
do nothing here!

*******************************************************************************/
void ComboBoxList::redrawFrame() const {}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void ComboBoxList::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int rows, cols, rowsavail, lastrow;

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;
  lastrow = _firstprint+rowsavail-1;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint, 0);

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
    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else { colors.turnOn(_win, fg_combobox, bg_combobox); }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item and scroll indicator if needed

  printToEol(_items[idx]->name());
  if ( (_firstprint > 0) && (int(idx) == _firstprint) )
    waddch(_win, ACS_UARROW);
  else if ( (lastrow < int(_items.size())-1) && (int(idx) == lastrow) )
    waddch(_win, ACS_DARROW);
  else { waddch(_win, ' '); }

  // Turn off highlight color

  if (colors.turnOff(_win) != 0)
  { 
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
ComboBoxList::ComboBoxList() { _reserved_rows = 0; }
ComboBoxList::ComboBoxList(WINDOW *win) 
{
  _win = win;
  _reserved_rows = 0; 
}

/*******************************************************************************

Edit list

*******************************************************************************/
void ComboBoxList::clearList()
{
  unsigned int i, nitems;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ ) { delete _items[i]; }

  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void ComboBoxList::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols;
  unsigned int i, nitems;

  // Minimum usable height

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Minimum usable width

  width = 0;
  reserved_cols = 4;      // For indicator on right
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

/*******************************************************************************

Draws box (frame, items, etc.) as needed

*******************************************************************************/
void ComboBoxList::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  // Draw list elements

  if (_redraw_type == "all") { wclear(_win); }
  if (_redraw_type != "none") { redrawFrame(); }
  if ( (_redraw_type == "all") || (_redraw_type == "items") ) {
                                                            redrawAllItems(); }
  else if (_redraw_type == "changed") { redrawChangedItems(); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: show display until user hits Enter or Esc

*******************************************************************************/
std::string ComboBoxList::exec()
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
  
      // Enter key: accept selection
  
      case '\n':
      case '\r':
      case KEY_ENTER:
        retval = signals::keyEnter;
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
  
      // Resize signal
  
      case KEY_RESIZE:
        retval = signals::resize;
        _redraw_type = "all";
        getting_input = false;
        break;
  
      // Quit key
  
      case MY_ESC:
        retval = signals::quit;
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
