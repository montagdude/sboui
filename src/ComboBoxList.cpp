#include <string>
#include <curses.h>
#include <chrono>    // sleep_for
#include <thread>    // this_thread
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "ListItem.h"
#include "ComboBoxList.h"
#include "MouseEvent.h"

/*******************************************************************************

There is no frame, and scroll arrows are handled by redrawSingleItem, so
do nothing here!

*******************************************************************************/
void ComboBoxList::redrawFrame() {}

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

  wmove(_win, idx-_firstprint+_header_rows, 0);

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
    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else { colors.turnOn(_win, "fg_combobox", "bg_combobox"); }

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
ComboBoxList::ComboBoxList()
{ 
  _reserved_rows = 0;
  _header_rows = 0;
  clearButtons();
  setModal(false);
}

ComboBoxList::ComboBoxList(WINDOW *win) 
{
  _win = win;
  _reserved_rows = 0; 
  _header_rows = 0;
  clearButtons();
  setModal(false);
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

  if (_redraw_type == "all") { clearWindow(); }
  if (_redraw_type != "none") { redrawFrame(); }
  if ( (_redraw_type == "all") || (_redraw_type == "items") ) {
                                                            redrawAllItems(); }
  else if (_redraw_type == "changed") { redrawChangedItems(); }
  wrefresh(_win);
}

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string ComboBoxList::handleMouseEvent(MouseEvent * mevent)
{
  std::string retval;

  // Use method from ListBox, but return keyEnter when item is selected

  retval = ListBox::handleMouseEvent(mevent);
  if ( (retval == signals::tag) || (retval == signals::highlight) )
  {
    // Redraw and pause for .1 seconds to make button selection visible

    _redraw_type = "all";
    draw();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    retval = signals::keyEnter;
  }

  return retval;
}
