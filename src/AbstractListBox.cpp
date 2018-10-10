#include <string>
#include <curses.h>
#include <cmath>     // floor, ceil
#include "settings.h"
#include "Color.h"
#include "ListItem.h"
#include "AbstractListBox.h"

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void AbstractListBox::redrawFrame()
{
  int rows, cols, namelen, i, left, color_pair;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  wmove(_win, 1, 1);
  wclrtoeol(_win);
  color_pair = colors.getPair("fg_title", "bg_title");
  if (colors.turnOn(_win, color_pair) != 0)
    wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_name);
  if (colors.turnOff(_win) != 0)
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
  for ( i = 1; int(i) < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Right border for header (footer only if there are buttons - below)

  mvwaddch(_win, 1, cols-1, ACS_VLINE);

  // Divider for header (footer only if there are buttons - below)

  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Button area

  if (_buttons.size() > 0)
  {
    wmove(_win, rows-3, 1);
    for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
    mvwaddch(_win, rows-3, 0, ACS_LTEE);
    mvwaddch(_win, rows-3, cols-1, ACS_RTEE);
    mvwaddch(_win, rows-2, cols-1, ACS_VLINE);
    redrawButtons();
  }
}

/******************************************************************************

Redraws all items

*******************************************************************************/
void AbstractListBox::redrawAllItems()
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
AbstractListBox::AbstractListBox()
{
  _name = "";
  _redraw_type = "all";
  _items.resize(0);
  _firstprint = 0;
  _header_rows = 3;
  _reserved_rows = 4;   // Assumes no buttons
}

AbstractListBox::AbstractListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _redraw_type = "all";
  _items.resize(0);
  _firstprint = 0;
  _header_rows = 3;
  _reserved_rows = 4;   // Assumes no buttons
}

/*******************************************************************************

Edit list items

*******************************************************************************/
void AbstractListBox::addItem(ListItem *item) { _items.push_back(item); }
void AbstractListBox::removeItem(unsigned int idx)
{
  if (idx <= _items.size()) { _items.erase(_items.begin()+idx); }
  _firstprint = 0;
  _redraw_type = "all";
}

void AbstractListBox::clearList()
{
  _items.resize(0);
  _firstprint = 0;
  _redraw_type = "all";
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void AbstractListBox::setName(const std::string & name) { _name = name; }

/*******************************************************************************

Get attributes

*******************************************************************************/
const std::string & AbstractListBox::name() const { return _name; }
unsigned int AbstractListBox::numItems() const { return _items.size(); }

void AbstractListBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols;
  unsigned int i, nitems, nbuttons;

  // Minimum usable height

  height = _reserved_rows + 2;

  // Minimum usable width

  reserved_cols = 2;
  width = _name.size();
  nbuttons = _buttons.size();
  if (nbuttons > 0)
  {
    namelen = 0;
    for ( i = 0; i < nbuttons; i++ )
    {
      namelen += _buttons[i].size();
    }
    if (namelen > width) { width = namelen; }
  }
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void AbstractListBox::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols, widthpadding;
  unsigned int i, nitems, nbuttons;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size();
  nbuttons = _buttons.size();
  if (nbuttons > 0)
  {
    namelen = 0;
    for ( i = 0; i < nbuttons; i++ )
    {
      namelen += _buttons[i].size();
    }
    if (namelen > width) { width = namelen; }
  }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

const std::string & AbstractListBox::fgColor() const { return _fg_color; }
const std::string & AbstractListBox::bgColor() const { return _bg_color; }

/*******************************************************************************

Returns pointer to item

*******************************************************************************/
ListItem * AbstractListBox::itemByIdx(unsigned int idx)
{
  unsigned int nitems;

  nitems = _items.size();
  if (idx >= nitems) { return NULL; }
  else { return _items[idx]; }
}

/*******************************************************************************

Draws list box (frame, items, etc.) as needed

*******************************************************************************/
void AbstractListBox::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  // Draw list elements

  if (_redraw_type == "all")
  { 
    clearWindow();
    colors.setBackground(_win, _fg_color, _bg_color);
  }
  if (_redraw_type == "buttons") { redrawButtons(); }
  else if (_redraw_type != "none")
  {
    redrawFrame();
    redrawScrollIndicator();
  }
  if ( (_redraw_type == "all") || (_redraw_type == "items")) { 
                                                            redrawAllItems(); }
  wrefresh(_win);
}

