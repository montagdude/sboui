#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "ListItem.h"
#include "BuildListItem.h"
#include "SelectionBox.h"
#include "TagList.h"

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void TagList::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int rows, cols; 

  getmaxyx(_win, rows, cols);

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
    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else { colors.turnOn(_win, color_settings.fg_popup,
                             color_settings.bg_popup); }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item with selection

  if (_items[idx]->getBoolProp("marked")) { wprintw(_win, "[X] "); }
  else { wprintw(_win, "[ ] "); }

  printToEol(_items[idx]->name());

  // Move to area between brackets to show cursor

  wmove(_win, _highlight-_firstprint+3, 2);

  // Turn off color

  if (colors.turnOff(_win) != 0)
  {
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
TagList::TagList() { _tagged.resize(0); }
TagList::TagList(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _tagged.resize(0);
}

/*******************************************************************************

Edit list items

*******************************************************************************/
void TagList::addItem(ListItem *item)
{
  item->setBoolProp("marked", true);
  _tagged.push_back(item);
}

void TagList::removeItem(ListItem *item)
{
  unsigned int i, ntagged;

  ntagged = _tagged.size();
  for ( i = 0; i < ntagged; i++ )
  {
    if (_tagged[i]->name() == item->name())
    {
      _tagged.erase(_tagged.begin()+i); 
      break;
    }
  }
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
}

void TagList::removeItem(unsigned int idx)
{
  if (idx <= _tagged.size()) { _tagged.erase(_tagged.begin()+idx); }
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
}

void TagList::clearList()
{
  _tagged.resize(0);
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
}

/*******************************************************************************

Get properties

*******************************************************************************/
unsigned int TagList::numTagged() const { return _tagged.size(); }

/*******************************************************************************

Creates _items list to display based on requested action. Returns number of
items in list.

*******************************************************************************/
unsigned int TagList::getDisplayList(const std::string & action)
{
  unsigned int i, ntagged;
  bool add_item;
  BuildListItem *item;

  _items.resize(0);
  
  ntagged = _tagged.size();
  for ( i = 0; i < ntagged; i++ )
  {
    add_item = false;
    item = static_cast<BuildListItem *>(_tagged[i]);
    if (action == "Install")
    {
      if (! item->getBoolProp("installed")) { add_item = true; }
    }
    else if (action == "Upgrade")
    {
      if ( (item->getBoolProp("upgradable")) &&
           (! item->getBoolProp("blacklisted")) )
        add_item = true;
    }
    else if ( (action == "Remove") || (action == "Reinstall") )
    {
      if ( (item->getBoolProp("installed")) &&
           (! item->getBoolProp("blacklisted")) ) { add_item = true; }
    }
       
    if (add_item) { _items.push_back(_tagged[i]); }
  }

  return numItems();
} 

/*******************************************************************************

User interaction: returns key stroke or other signal. getDisplayList must be
called first or nothing will be displayed!

*******************************************************************************/
std::string TagList::exec()
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;

  curs_set(1);

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

    // Resize signal: redraw (may not work with some curses implementations)

    case KEY_RESIZE:
      retval = signals::resize;
      _redraw_type = "all";
      break;

    // Quit key

    case MY_ESC:
      retval = signals::quit;
      _redraw_type = "all";
      break;

    // Space: toggle item

    case ' ':
      retval = " ";
      _items[_highlight]->setBoolProp("marked", 
                                 (! _items[_highlight]->getBoolProp("marked")));
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  curs_set(0);

  return retval;
}

/*******************************************************************************

Returns item from _tagged vector by index. Careful: does not perform bounds
checks!

*******************************************************************************/
ListItem * TagList::taggedByIdx(unsigned int idx) { return _tagged[idx]; }
