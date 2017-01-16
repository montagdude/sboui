#include <string>
#include <curses.h>
#include "Color.h"
#include "color_settings.h"
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
  std::size_t len;

  addItem(new ListItem("View README"));
  if (! build.getBoolProp("installed")) { addItem(new ListItem("Install")); }
  else
  {
    addItem(new ListItem ("Remove"));
    len = build.getProp("available_version").size();
    if (build.getProp("installed_version").substr(0, len) !=
        build.getProp("available_version"))
    {
      addItem(new ListItem("Upgrade"));
    }
  }
  addItem(new ListItem("Compute build order"));
  addItem(new ListItem("List inverse reqs"));
  addItem(new ListItem("Browse files"));
}
