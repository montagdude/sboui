#include <string>
#include <curses.h>
#include "ListItem.h"
#include "BuildListItem.h"
#include "SelectionBox.h"
#include "BuildActionBox.h"

/*******************************************************************************

Constructor

*******************************************************************************/
BuildActionBox::BuildActionBox()
{
  _name = "Select an action";
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
void BuildActionBox::create(BuildListItem & build, bool limited_actions)
{
  unsigned int count;

  clearList();
  count = 0;

  addItem(new ListItem("View README"));
  _items[count]->setHotKey(0);
  count++;

  addItem(new ListItem("Browse files"));
  _items[count]->setHotKey(0);
   count++;

  addItem(new ListItem("Set build options"));
  _items[count]->setHotKey(0);
   count++;

  addItem(new ListItem("Show package info"));
  _items[count]->setHotKey(1);
   count++;

  if (! limited_actions)
  {
    if (! build.getBoolProp("blacklisted"))
    {
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
        if (build.getBoolProp("upgradable"))
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
    }

    addItem(new ListItem("Compute build order"));
    _items[count]->setHotKey(0);
     count++;
  }

  addItem(new ListItem("List inverse deps"));
  _items[count]->setHotKey(0);
   count++;

  addItem(new ListItem("View/edit notes"));
  _items[count]->setHotKey(3);
  count++;
}
