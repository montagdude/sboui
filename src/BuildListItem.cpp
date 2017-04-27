#include <string>
#include <vector>
#include "backend.h"
#include "ListItem.h"
#include "BuildListItem.h"

/*******************************************************************************

Checks whether SlackBuild can be upgraded

*******************************************************************************/
bool BuildListItem::upgradable() const
{
  std::size_t ilen, alen;
  bool test;
  std::string installed, available;

  test = false;
  installed = getProp("installed_version");
  available = getProp("available_version");

  // Allow installed version to match even if it has a trailing underscore
  // after the available_version string. Happens with some packages like kernel
  // modules. If there are other patterns that should also be considered here,
  // hopefully someone will tell me.

  if ( (getBoolProp("installed")) && (! getBoolProp("blacklisted")) )
  {
    if (installed != available)
    {
      ilen = installed.size();
      alen = available.size();
      test = true;
      if (ilen > alen)
      {
        if ( (installed.substr(0,alen) == available) &&
             (installed[alen] == '_') ) { test = false; }
      }
    }
  }

  return test;
}

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
  addBoolProp("installed", false);
  addBoolProp("upgradable", false);
  addBoolProp("tagged", false);
  addBoolProp("blacklisted", false);
  addBoolProp("marked", false);
  addProp("package_name", "");
  addProp("action", "");
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

  if (item.checkProp("installed"))
    setBoolProp("installed", item.getBoolProp("installed"));
  else { addBoolProp("installed", false); }

  if (item.checkProp("upgradable"))
    setBoolProp("upgradable", item.getBoolProp("upgradable"));
  else { addBoolProp("upgradable", false); }

  if (item.checkProp("blacklisted"))
    setBoolProp("blacklisted", item.getBoolProp("blacklisted"));
  else { addBoolProp("blacklisted", false); }

  if (item.checkProp("tagged"))
    setBoolProp("tagged", item.getBoolProp("tagged"));
  else { addBoolProp("tagged", false); }

  if (item.checkProp("marked"))
    setBoolProp("marked", item.getBoolProp("marked"));
  else { addBoolProp("marked", false); }

  if (item.checkProp("package_name"))
    setProp("package_name", item.getProp("package_name"));
  else { addProp("package_name", ""); }

  if (item.checkProp("action"))
    setProp("action", item.getProp("action"));
  else { addProp("action", ""); }
}

/*******************************************************************************

Checks whether this BuildListItem is installed and gets information about it
if so. If repo info has been read already, checks whether installed SlackBuild
is also upgradable.

*******************************************************************************/
void BuildListItem::readInstalledProps(std::vector<std::string> & installedpkgs)
{
  std::string pkg, version, arch, build;

  if (check_installed(*this, installedpkgs, pkg, version, arch, build))
  {
    setBoolProp("installed", true);
    setProp("installed_version", version);
    setProp("package_name", pkg);
    setBoolProp("blacklisted", package_blacklist.blacklisted(pkg, _name, 
                               version, arch, build));
    if (getProp("available_version") != "")
      setBoolProp("upgradable", upgradable());
  }
  else { setBoolProp("installed", false); }
}

/*******************************************************************************

Reads properties from repo. If installed, checks whether it is upgradable.

*******************************************************************************/
int BuildListItem::readPropsFromRepo()
{
  std::string available_version, reqs;
  int check;

  check = get_repo_info(*this, available_version, reqs);
  if (check == 0)
  {
    setProp("available_version", available_version);
    setProp("requires", reqs);
    if (getBoolProp("installed")) { setBoolProp("upgradable", upgradable()); }
  }

  return check;
}
