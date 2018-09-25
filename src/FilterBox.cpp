#include <string>
#include <curses.h>
#include "ListItem.h"
#include "SelectionBox.h"
#include "FilterBox.h"

/*******************************************************************************

Constructors and destructor

*******************************************************************************/
FilterBox::FilterBox()
{
  unsigned int i;

  _name = "Select a filter";
  _buttons.resize(2);
  _buttons[0] = "    Ok    ";
  _buttons[1] = "  Cancel  ";
  _highlighted_button = 0;

  addItem(new ListItem("All"));
  addItem(new ListItem("Installed"));
  addItem(new ListItem("Upgradable"));
  addItem(new ListItem("Tagged"));
  addItem(new ListItem("Blacklisted"));
  addItem(new ListItem("Non-dependencies"));
  addItem(new ListItem("Build options set"));

  for ( i = 0; i < 6; i++ ) { _items[i]->setHotKey(0); }
  _items[6]->setHotKey(3);
}

FilterBox::FilterBox(WINDOW *win, const std::string & name)
{
  unsigned int i;

  _win = win;
  _name = name;
  _buttons.resize(2);
  _buttons[0] = "    Ok    ";
  _buttons[1] = "  Cancel  ";
  _highlighted_button = 0;

  addItem(new ListItem("All"));
  addItem(new ListItem("Installed"));
  addItem(new ListItem("Upgradable"));
  addItem(new ListItem("Tagged"));
  addItem(new ListItem("Blacklisted"));
  addItem(new ListItem("Non-dependencies"));
  addItem(new ListItem("Build options set"));

  for ( i = 0; i < 6; i++ ) { _items[i]->setHotKey(0); }
  _items[6]->setHotKey(3);
}

FilterBox::~FilterBox()
{
  unsigned int i, nitems;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ ) { delete _items[i]; }
  _items.resize(0);
}
