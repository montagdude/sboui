#include <string>
#include <vector>
#include "ListItem.h"
#include "AbstractListBox.h"

/*******************************************************************************

Constructors

*******************************************************************************/
AbstractListBox::AbstractListBox()
{
  _win = NULL;
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
void AbstractListBox::setWindow(WINDOW *win) { _win = win; }
void AbstractListBox::setName(const std::string & name) { _name = name; }

/*******************************************************************************

Get attributes

*******************************************************************************/
const std::string & AbstractListBox::name() const { return _name; }

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
