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
  addBoolProp("installed", false);
  addBoolProp("tagged", false);
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
}

/*******************************************************************************

Checks whether this BuildListItem is installed and gets information about it
if so

*******************************************************************************/
void BuildListItem::readInstalledProps(std::vector<std::string> & installedpkgs)
{
  std::string pkg, version;

  if (check_installed(*this, installedpkgs, pkg, version))
  {
    setBoolProp("installed", true);
    setProp("installed_version", version);
    setProp("package_name", pkg);
  }
  else { setBoolProp("installed", false); }
}

/*******************************************************************************

Reads properties from repo

*******************************************************************************/
void BuildListItem::readPropsFromRepo()
{
  std::string available_version, reqs;

  get_repo_info(*this, available_version, reqs);
  setProp("available_version", available_version);
  setProp("requires", reqs);
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
