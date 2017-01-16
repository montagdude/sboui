#include <string>
#include <curses.h>
#include "Color.h"
#include "color_settings.h"
#include "ListItem.h"
#include "SelectionBox.h"
#include "FilterBox.h"

using namespace color;

/*******************************************************************************

Constructors

*******************************************************************************/
FilterBox::FilterBox()
{
  _name = "Select a filter";
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;

  addItem(new ListItem("All"));
  addItem(new ListItem("Installed"));
  addItem(new ListItem("Upgradable"));
  addItem(new ListItem("Non-dependencies"));
}

FilterBox::FilterBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;

  addItem(new ListItem("All"));
  addItem(new ListItem("Installed"));
  addItem(new ListItem("Upgradable"));
  addItem(new ListItem("Non-dependencies"));
}

/*******************************************************************************

Destructor

*******************************************************************************/
FilterBox::~FilterBox()
{
  unsigned int i, nitems;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ ) { delete _items[i]; }
  _items.resize(0);
}
