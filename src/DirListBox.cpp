#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "DirListing.h"
#include "ListItem.h"
#include "SelectionBox.h"
#include "DirListBox.h"

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void DirListBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;

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
    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Make directories bold

  if (_items[idx]->getProp("type") == "dir") { wattron(_win, A_BOLD); }

  // Add / to identify directories and @ to identify symlinks

  if (_items[idx]->getProp("type") == "dir") { printToEol(
                                                   _items[idx]->name() + "/"); }
  else if (_items[idx]->getProp("type") == "lnk") { printToEol(
                                                   _items[idx]->name() + "@"); }
  else { printToEol(_items[idx]->name()) ; }

  // Turn off bold for directories

  if (_items[idx]->getProp("type") == "dir") { wattroff(_win, A_BOLD); }

  // Turn off highlight color

  if (colors.turnOff(_win) != 0)
  { 
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Navigates one directory up

*******************************************************************************/
int DirListBox::navigateUp()
{
  int check;
  DirListing dir;

  dir.setFromPath(_currentdir);
  check = dir.navigateUp();
  if (check != 0) { return check; }

  return setDirectory(dir.path(), ! _limit_topdir);
}

/*******************************************************************************

Clears directory listing

*******************************************************************************/
void DirListBox::clear()
{
  unsigned int i, numitems;

  numitems = numItems();
  for ( i = 0; i < numitems; i++ ) { delete _items[i]; }
  _items.resize(0);
}

/*******************************************************************************

Constructors and destructor

*******************************************************************************/
DirListBox::DirListBox()
{
  _info = "Enter: View | Esc: Back";
  _reserved_rows = 6;
  _header_rows = 3;
  _topdir = "";
  _currentdir = "";
  _limit_topdir = false;
}

DirListBox::DirListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _info = "Enter: View | Esc: Back";
  _reserved_rows = 6;
  _header_rows = 3;
  _topdir = "";
  _currentdir = "";
  _limit_topdir = false;
}

DirListBox::~DirListBox() { clear(); }

/*******************************************************************************

Setting properties

*******************************************************************************/
void DirListBox::limitTopDir(bool limit) { _limit_topdir = limit; }
int DirListBox::setDirectory(const std::string & directory, bool reset_topdir)
{
  int check;
  unsigned int i, entrycounter, nentries, topdirsize, currentdirsize;
  bool below_topdir;
  DirListing dir;

  check = dir.setFromPath(directory);
  if (check != 0) { return check; }
  _currentdir = dir.path();
  if ( (_topdir == "") || reset_topdir) { _topdir = _currentdir; }

  // Check if we are below or at _topdir. If not, reset _topdir.

  topdirsize = _topdir.size();
  currentdirsize = _currentdir.size();
  if ( (currentdirsize > topdirsize) &&
       (_currentdir.substr(0,topdirsize) == _topdir) )
  {
    below_topdir = true;
  }
  else
  {
    below_topdir = false;
    _topdir = _currentdir;
  } 

  clear();
  nentries = dir.size();

  // Item to allow navigating up

  entrycounter = 0;
  if (_currentdir != "/")
  {
    if ( (! _limit_topdir) || (below_topdir) )
    {
      addItem(new ListItem(".."));
      _items[0]->addProp("type", "dir");
      entrycounter++;
    }
  }

  for ( i = 0; i < nentries; i++ )
  {
    addItem(new ListItem(dir(i).name)); 
    _items[entrycounter]->addProp("type", dir(i).type);
    entrycounter++;
  }
  highlightFirst();

  return check;
}

/*******************************************************************************

Accessing properties

*******************************************************************************/
const std::string & DirListBox::directory() const { return _currentdir; }

/*******************************************************************************

User interaction: display list of files that user can view

*******************************************************************************/
std::string DirListBox::exec()
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
        _redraw_type = "all";
        if (_items[_highlight]->getProp("type") == "dir")
        {
          if (_items[_highlight]->name() == "..") { navigateUp(); }
          else { setDirectory(_currentdir + _items[_highlight]->name()); }
        }
        else 
        { 
          retval = signals::keyEnter;
          getting_input = false;
        }
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
