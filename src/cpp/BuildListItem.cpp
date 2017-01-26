#include <string>
#include "ListItem.h"
#include "BuildListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
BuildListItem::BuildListItem() 
{ 
  _name = ""; 
  addProp("category", "");
  addProp("installed_version", "");
  addProp("available_version", "");
  addProp("requires", "");
  addProp("package_name", "");
  addBoolProp("tagged", false);
  addBoolProp("installed", false);
}

/*******************************************************************************

Checks whether SlackBuild can be upgraded

*******************************************************************************/
bool BuildListItem::upgradable() const
{
  std::size_t len;
  bool test;

  test = false;
  len = getProp("available_version").size();

  // Allow installed version to match even if it has trailing stuff after the
  // available_version string. Happens with some packages like kernel modules.

  if (getBoolProp("installed"))
  {
    if (getProp("installed_version").substr(0,len) != 
        getProp("available_version")) { test = true; }
  }

  return test;
}
