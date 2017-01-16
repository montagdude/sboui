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

Creates options using info about SlackBuild

*******************************************************************************/
void BuildActionBox::create(const BuildListItem & build)
{
  std::size_t len;
  unsigned int i, nchoices;

  ListItem item1("View README");
  _choices.push_back(item1);
  if (! build.getBoolProp("installed"))
  {
    ListItem item2("Install");
    _choices.push_back(item2);
  }
  else
  {
    ListItem item3("Remove");
    _choices.push_back(item3);
    len = build.getProp("available_version").size();
    if (build.getProp("installed_version").substr(0, len) !=
        build.getProp("available_version"))
    {
      ListItem item4("Upgrade");
      _choices.push_back(item4);
    }
  }
  ListItem item5("Compute build order");
  _choices.push_back(item5);
  ListItem item6("List inverse reqs");
  _choices.push_back(item6);
  ListItem item7("Browse files");
  _choices.push_back(item7);
  
  nchoices = _choices.size();
  for ( i = 0; i < nchoices; i++ ) { addItem(&_choices[i]); } 
}
