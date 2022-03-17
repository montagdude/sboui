#include <string>
#include <vector>
#include <curses.h>
#include <algorithm>    // max
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "MenubarListItem.h"
#include "MenubarList.h"
#include "Menubar.h"
#include "MouseEvent.h"

/*******************************************************************************

Determines column of first character in MenubarList lidx

*******************************************************************************/
unsigned int Menubar::menuColumn(unsigned int lidx) const
{
  unsigned int i, col;

  if (lidx >= _lists.size())
    return 0;

  col = 0;
  for ( i = 0; i < lidx; i++ )
  {
    col += _lists[i].name().size() + 2*_pad;
  }

  return col;
}

/*******************************************************************************

Places and sizes list box

*******************************************************************************/
void Menubar::placeListBox()
{
  int width, height;
  unsigned int left;

  // Get position and size

  left = menuColumn(_highlight);
  _lists[_highlight].preferredSize(height, width);

  // Set position and size

  mvwin(_listwins[_highlight], 1, left);
  wresize(_listwins[_highlight], height, width);
}

/*******************************************************************************

Highlights first menu label

*******************************************************************************/
void Menubar::highlightFirst()
{
  if (_lists.size() == 0)
    return;

  _prevhighlight = _highlight;
  _highlight = 0;
}

/*******************************************************************************

Highlights last menu label

*******************************************************************************/
void Menubar::highlightLast()
{
  if (_lists.size() == 0)
    return;

  _prevhighlight = _highlight;
  _highlight = std::max(int(_lists.size()) - 1, 0);
}

/*******************************************************************************

Highlights previous menu label

*******************************************************************************/
void Menubar::highlightPrevious()
{
  if (_lists.size() == 0)
    return;

  if (_highlight == 0)
    return;
  else
  {
    _prevhighlight = _highlight;
    _highlight -= 1;
  }
}

/*******************************************************************************

Highlights next menu label

*******************************************************************************/
void Menubar::highlightNext()
{
  if (_lists.size() == 0)
    return;

  if (_highlight == int(_lists.size())-1)
    return;
  else
  {
    _prevhighlight = _highlight;
    _highlight += 1;
  }
}

void Menubar::setHighlight(int highlight)
{
  if ( (highlight >= 0) && (highlight < int(_lists.size())) )
  {
    _prevhighlight = _highlight;
    _highlight = highlight;
  }
  else if (highlight >= int(_lists.size()))
    highlightLast();
}

/*******************************************************************************

Redraws a single menu label

*******************************************************************************/
void Menubar::redrawSingleItem(unsigned int idx)
{
  unsigned int nlists, pos, i, len, hidx;
  int color_pair1, color_pair2;

  nlists = _lists.size();
  if (idx >= nlists)
    return;

  // Get drawing position and color

  pos = menuColumn(idx);
  if ( (int(idx) == _highlight) && (_activated) )
  {
    color_pair1 = colors.getPair("fg_highlight_active", "bg_highlight_active");
    color_pair2 = colors.getPair("hotkey", "bg_highlight_active");
  }
  else
  {
    color_pair1 = colors.getPair(_fg_color, _bg_color);
    color_pair2 = colors.getPair("hotkey", _bg_color);
  }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.

  if (int(idx) == _highlight)
    _prevhighlight = _highlight;

  // Print item

  len = _lists[idx].name().size();
  hidx = _lists[idx].hotKey();
  wmove(_win, 0, pos);
  if (colors.turnOn(_win, color_pair1) != 0)
  {
    if ( (int(idx) == _highlight) && (_activated) )
      wattron(_win, A_REVERSE);
  }
  for ( i = 0; i < _pad; i++ )
  {
    waddch(_win, ' ');
  }
  for ( i = 0; i < len; i++ )
  {
    if ( (i == hidx) && _activated )
    {
      colors.turnOff(_win);
      if (colors.turnOn(_win, color_pair2) != 0) { wattron(_win, A_BOLD); }
      wprintw(_win, "%s", _lists[idx].name().substr(i,1).c_str());
      if (colors.turnOff(_win) != 0) { wattroff(_win, A_BOLD); }
      colors.turnOn(_win, color_pair1);
    }
    else { wprintw(_win, "%s", _lists[idx].name().substr(i,1).c_str()); }
  }
  for ( i = 0; i < _pad; i++ )
  {
    waddch(_win, ' ');
  }
  if (colors.turnOff(_win) != 0)
  {
    if ( (int(idx) == _highlight) && (_activated) )
      wattroff(_win, A_REVERSE);
  }
}

/*******************************************************************************

Redraws the previously and currently highlighted menu labels

*******************************************************************************/
void Menubar::redrawChangedItems()
{
  if (_prevhighlight < int(_lists.size()))
    redrawSingleItem(_prevhighlight);

  if (_highlight < int(_lists.size()))
    redrawSingleItem(_highlight);
}

/*******************************************************************************

Redraws all menu labels

*******************************************************************************/
void Menubar::redrawAllItems()
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ ) { redrawSingleItem(i); }
}

/*******************************************************************************

User interaction with highlighted list

*******************************************************************************/
std::string Menubar::execList(MouseEvent * mevent)
{
  std::string retval;

  placeListBox();
  retval = _lists[_highlight].exec(mevent);

  wresize(_listwins[_highlight], 0, 0);
  wrefresh(_listwins[_highlight]);

  if (_parent)
    _parent->draw(true);

  return retval;
}

/*******************************************************************************

Constructors and destructor

*******************************************************************************/
Menubar::Menubar()
{
  _parent = NULL;
  _highlight = 0;
  _prevhighlight = 0;
  _activated = false;
  _lists.resize(0);
  _listwins.resize(0);
  _fg_color = "fg_title";
  _bg_color = "bg_title";
  _redraw_type = "all";
  _pad = 0;
  _outerpad = 0;
  _innerpad = 0;
}
Menubar::Menubar(WINDOW *win)
{
  _parent = NULL;
  _win = win;
  _highlight = 0;
  _prevhighlight = 0;
  _activated = false;
  _lists.resize(0);
  _listwins.resize(0);
  _fg_color = "fg_title";
  _bg_color = "bg_title";
  _redraw_type = "all";
  _pad = 0;
  _outerpad = 0;
  _innerpad = 0;
}
Menubar::~Menubar() { clearLists(); }

/*******************************************************************************

Set attributes

*******************************************************************************/
void Menubar::setActivated(bool activated) { _activated = activated; }
void Menubar::setParent(CursesWidget * parent) { _parent = parent; }
void Menubar::setPad(unsigned int pad) { _pad = pad; }
void Menubar::setListPad(unsigned int outerpad, unsigned int innerpad)
{
  unsigned int i, nlists;

  _outerpad = outerpad;
  _innerpad = innerpad;
  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    _lists[i].setPad(outerpad, innerpad);
  }
}

/*******************************************************************************

Adds a list and returns 0, or does nothing and returns 1 if a list already
exists by that name.

*******************************************************************************/
int Menubar::addList(const std::string & lname, int hotkey)
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    if (lname == _lists[i].name())
      return 1;
  }

  MenubarList list;
  _listwins.push_back(newwin(1, 1, 0, 0));
  list.setName(lname);
  list.setWindow(_listwins[nlists]);
  list.setPad(_outerpad, _innerpad);
  list.setHotKey(hotkey);
  _lists.push_back(list);

  return 0;
}

/*******************************************************************************

Adds an item to a list by list idx. Returns 0 if successful. Returns 1 if list
doesn't exist, or returns 2 if list exists but already has an item by that name.

*******************************************************************************/
int Menubar::addListItem(unsigned int lidx, const std::string & text,
                         const std::string & shortcut, int hotkey)
{
  unsigned int nlists, j, nitems;

  nlists = _lists.size();
  if (lidx >= nlists)
    return 1;

  nitems = _lists[lidx].numItems();
  for ( j = 0; j < nitems; j++ )
  {
    if (_lists[lidx].itemByIdx(j)->name() == text)
      return 2;
  }
  _lists[lidx].addItem(new MenubarListItem(text, shortcut, hotkey));

  return 0;
}

/*******************************************************************************

Adds an item to a list by list name. Returns 0 if successful. Returns 1 if list
doesn't exist, or returns 2 if list exists but already has an item by that name.

*******************************************************************************/
int Menubar::addListItem(const std::string & lname, const std::string & text,
                         const std::string & shortcut, int hotkey)
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    if (_lists[i].name() == lname)
      return addListItem(i, text, shortcut, hotkey);
  }

  return 1;
}

/*******************************************************************************

Removes all lists

*******************************************************************************/
void Menubar::clearLists()
{
  unsigned int i, nlists;

  nlists = _lists.size();
  for ( i = 0; i < nlists; i++ )
  {
    _lists[i].clearList();
    delwin(_listwins[i]);
  }

  _lists.resize(0);
  _listwins.resize(0);
  _highlight = 0;
  _prevhighlight = 0;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
unsigned int Menubar::numLists() const { return _lists.size(); }
unsigned int Menubar::highlightedList() const { return _highlight; }
std::string Menubar::highlightedListName() const
{
  if ( (_highlight >= 0) && (_highlight < int(_lists.size())) )
    return _lists[_highlight].name();
  else
    return "";
}
std::string Menubar::highlightedListItem() const
{
  if ( (_highlight >= 0) && (_highlight < int(_lists.size())) )
    return _lists[_highlight].highlightedName();
  else
    return "";
}
void Menubar::minimumSize(int & height, int & width) const
{
  int rows, cols;

  getmaxyx(_win, rows, cols);
  height = 1;
  width = cols;
}

void Menubar::preferredSize(int & height, int & width) const
{
  minimumSize(height, width);
}

void Menubar::bounds(int & xmin, int & xmax) const
{
  unsigned int lastmenu;

  lastmenu = _lists.size()-1;
  xmin = 0;
  xmax = menuColumn(lastmenu) + _lists[lastmenu].name().size()
       + 2*_pad;
}

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string Menubar::handleMouseEvent(MouseEvent * mevent)
{
  int xmin, xmax, begy, begx, ycurs, xcurs;
  unsigned int i, nlists;

  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;
  xcurs = mevent->x() - begx;

  if ( (mevent->button() == 1) || (mevent->button() == 3) )
  {
    // Check for clicking on menu labels

    if (ycurs != 0) { return signals::mouseEvent; }

    nlists = _lists.size();
    if (nlists > 0)
    {
      for ( i = 0; i < nlists; i++ )
      {
        xmin = menuColumn(i);
        xmax = xmin + _lists[i].name().size() + 2*_pad;
        if ( (xcurs >= xmin) && (xcurs <= xmax) )
        {
          setHighlight(i);
          return exec(mevent);
        }
      }
      return signals::mouseEvent;   // To be handled by parent
    }
    else { return signals::nullEvent; }
  }
  else
    return signals::nullEvent;
}

/*******************************************************************************

Draws menubar

*******************************************************************************/
void Menubar::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  if (_redraw_type == "all")
  {
    int rows, cols;

    getmaxyx(_win, rows, cols);
    wmove(_win, 0, 0);
    colors.turnOn(_win, _fg_color, _bg_color);
    printToEol("", cols);
    colors.turnOff(_win);
    redrawAllItems();
  }
  else
    redrawChangedItems();
  wrefresh(_win);
}

/*******************************************************************************

User interaction

*******************************************************************************/
std::string Menubar::exec(MouseEvent * mevent)
{
  int hotkey;
  std::string retval;
  bool getting_input, needs_selection;
  unsigned int i, nlists;
  char hotcharN, hotcharL;
  std::vector<char> hotkey_vec;

  nlists = _lists.size();
  hotkey_vec.resize(0);
  for ( i = 0; i < nlists; i++ )
  {
    hotkey = _lists[i].hotKey();
    if (hotkey != -1)
      hotkey_vec.push_back(_lists[i].name()[hotkey]);
  }
  for ( i = 0; i < nlists; i++ )
  {
    _lists[i].setExternalHotKeys(hotkey_vec);
  }

  // Highlight first entry on first display

  if ( (_highlight == 0) && (_prevhighlight == 0) ) { highlightFirst(); }

  getting_input = true;
  needs_selection = true;
  while (getting_input)
  {
    // Draw list elements

    draw();
    _redraw_type = "all";

    // Get user input

    if (needs_selection)
      retval = execList(mevent);
    else
      needs_selection = true;

    // Enter/Esc/F9: leave menu

    if (retval == signals::keyF9)
      retval = signals::quit;

    if ( (retval == signals::keyEnter) || (retval == signals::quit) )
      getting_input = false;

    // Right/left: change highlighted menu label

    else if (retval == signals::keyRight)
    {
      _redraw_type = "changed";
      highlightNext();
    }

    else if (retval == signals::keyLeft)
    {
      _redraw_type = "changed";
      highlightPrevious();
    }

    // Mouse

    else if (retval == signals::mouseEvent)
    {
      needs_selection = false;
      retval = handleMouseEvent(mevent);
      if (retval == signals::mouseEvent)
        getting_input = false;    // To be handled by parent
    }

    // Check hotkeys

    else if (retval.size() == 1)
    {
      for ( i = 0; i < nlists; i++ )
      {
        hotkey = _lists[i].hotKey();
        if (hotkey != -1)
        {
          hotcharN = _lists[i].name()[hotkey];
          hotcharL = std::tolower(_lists[i].name()[hotkey]);
          if ( (retval[0] == hotcharN) || (retval[0] == hotcharL) )
          {
            _redraw_type = "changed";
            setHighlight(i);
          }
        }
      }
    }
  }

  return retval;
}
