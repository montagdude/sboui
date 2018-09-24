#include <string>
#include <curses.h>
#include <cmath>     // floor, ceil
#include "ListItem.h"
#include "AbstractListBox.h"

/*******************************************************************************

Redraws right border between header and footer and scroll indicators

*******************************************************************************/
void AbstractListBox::redrawScrollIndicator() const
{
  int rows, cols, i, rowsavail, pos;
  double frac;
  bool need_up, need_dn;

  // Check if a scroll indicator is needed

  getmaxyx(_win, rows, cols);
  rowsavail = rows-_reserved_rows;

  need_up = false;
  need_dn = false;
  if (_firstprint != 0) { need_up = true; }
  if (_items.size() > _firstprint + rows-_reserved_rows) { need_dn = true; }

  // Draw right border

  for ( i = _header_rows; i < int(_header_rows)+rowsavail; i++ )
  {
    mvwaddch(_win, i, cols-1, ACS_VLINE);
  }

  // Draw up and down arrows

  if (need_up) { mvwaddch(_win, _header_rows, cols-1, ACS_UARROW); }
  if (need_dn) { mvwaddch(_win, _header_rows+rowsavail-1, cols-1, ACS_DARROW); }

  // Draw position indicator

  if ( (_items.size() > 0) && ((need_up) || (need_dn)) )
  {
    frac = double(_highlight)/double(_items.size()-1);

    // Make sure we don't indicate the top or bottom before we're really there

    if (frac < 0.5)
      pos = std::ceil(frac*(rowsavail-1));
    else
      pos = std::floor(frac*(rowsavail-1));

    mvwaddch(_win, _header_rows+pos, cols-1, ACS_DIAMOND);
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
AbstractListBox::AbstractListBox()
{
  _name = "";
  _highlight = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
  _items.resize(0);
  _firstprint = 0;
  _reserved_rows = 2;
  _header_rows = 1;
}

AbstractListBox::AbstractListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _highlight = 0;
  _prevhighlight = 0;
  _redraw_type = "all";
  _items.resize(0);
  _firstprint = 0;
  _reserved_rows = 2;
  _header_rows = 1;
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
