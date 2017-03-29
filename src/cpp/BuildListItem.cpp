#include <string>
#include <vector>
#include "backend.h"
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
  addBoolProp("foreign", false);
}

/*******************************************************************************

Converts ListItem to BuildListItem

*******************************************************************************/
void BuildListItem::operator = (const ListItem & item)
{
  _name = item.name();

  if (item.checkProp("category"))
    setProp("category", item.getProp("category"));
  else { addProp("category", ""); }

  if (item.checkProp("installed_version"))
    setProp("installed_version", item.getProp("installed_version"));
  else { addProp("installed_version", ""); }

  if (item.checkProp("available_version"))
    setProp("available_version", item.getProp("available_version"));
  else { addProp("available_version", ""); }

  if (item.checkProp("requires"))
    setProp("requires", item.getProp("requires"));
  else { addProp("requires", ""); }

  if (item.checkProp("package_name"))
    setProp("package_name", item.getProp("package_name"));
  else { addProp("package_name", ""); }

  if (item.checkProp("tagged"))
    setBoolProp("tagged", item.getBoolProp("tagged"));
  else { addBoolProp("tagged", false); }

  if (item.checkProp("installed"))
    setBoolProp("installed", item.getBoolProp("installed"));
  else { addBoolProp("installed", false); }

  if (item.checkProp("foreign"))
    setBoolProp("foreign", item.getBoolProp("foreign"));
  else { addBoolProp("foreign", false); }
}

/*******************************************************************************

Reads properties from repo

*******************************************************************************/
void BuildListItem::readPropsFromRepo()
{
  std::vector<std::string> pkg_info;
  std::vector<std::string> repo_info;

  pkg_info = get_installed_info(*this);
  if (pkg_info[0] != "not_installed")
  {
    setBoolProp("installed", true);
    setProp("installed_version", pkg_info[0]);
    setProp("package_name", pkg_info[1]);
    if (pkg_info[2] == "0") { setBoolProp("foreign", false); }
    else { setBoolProp("foreign", true); }
  }
  else { setBoolProp("installed", false); }

  repo_info = get_repo_info(*this);
  setProp("available_version", repo_info[0]);
  setProp("requires", repo_info[1]);
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
