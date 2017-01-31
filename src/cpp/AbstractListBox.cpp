#include <string>
#include <curses.h>
#include "ListItem.h"
#include "AbstractListBox.h"

/*******************************************************************************

Prints to end of line, padding with spaces and avoiding borders

*******************************************************************************/
void AbstractListBox::printToEol(const std::string & msg) const
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
void AbstractListBox::printSpaces(unsigned int nspaces) const
{
  unsigned int i;

  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
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
  _reserved_rows = 2;
}

AbstractListBox::AbstractListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _redraw_type = "all";
  _items.resize(0);
  _firstprint = 0;
  _reserved_rows = 2;
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
