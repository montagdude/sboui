#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "ListItem.h"
#include "BuildListItem.h"
#include "SelectionBox.h"
#include "BuildActionBox.h"

using namespace color;

/*******************************************************************************

Constructors

*******************************************************************************/
BuildActionBox::BuildActionBox()
{
  _name = "Select an action";
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;
}

BuildActionBox::BuildActionBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;
}

BuildActionBox::BuildActionBox(WINDOW *win, const std::string & name,
                               const BuildListItem & build)
{
  _win = win;
  _name = name;
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;
  create(build);
}

/*******************************************************************************

Destructor

*******************************************************************************/
BuildActionBox::~BuildActionBox()
{
  unsigned int i, nitems;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ ) { delete _items[i]; }
  _items.resize(0);
}

/*******************************************************************************

Creates options using info about SlackBuild

*******************************************************************************/
void BuildActionBox::create(const BuildListItem & build)
{
  unsigned int count;

  count = 0;

  addItem(new ListItem("View README"));
  _items[count]->setHotKey(0);
  count++;

  if (! build.getBoolProp("installed"))
  { 
    addItem(new ListItem("Install")); 
    _items[count]->setHotKey(0);
    count++;
  }

  else
  {
    addItem(new ListItem ("Remove"));
    _items[count]->setHotKey(0);
    count++;

    if (build.upgradable())
    {
      addItem(new ListItem("Upgrade"));
      _items[count]->setHotKey(0);
    }
    else
    {
      addItem(new ListItem ("Reinstall"));
      _items[count]->setHotKey(1);
    }
    count++;
  }

  addItem(new ListItem("Compute build order"));
  _items[count]->setHotKey(0);
   count++;

  addItem(new ListItem("List inverse deps"));
  _items[count]->setHotKey(0);
   count++;

  addItem(new ListItem("Browse files"));
  _items[count]->setHotKey(0);
   count++;
}
