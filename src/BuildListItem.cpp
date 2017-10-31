#include <string>
#include <vector>
#include "backend.h"
#include "string_util.h"
#include "ListItem.h"
#include "BuildListItem.h"

/* The following can't use the simple installed_version != available_version
   check, because the SlackBuild appends something to $VERSION in the package
   name */
std::vector<std::string> special_upgrade_check {
  {"WireGuard"},
  {"acpi_call"},
  {"broadcom-sta"},
  {"broffice.org"},
  {"dahdi-complete"},
  {"klibc"},
  {"libreoffice-helppack"},
  {"libreoffice-langpack"},
  {"lirc"},
  {"netatop"},
  {"nvidia-kernel"},
  {"nvidia-legacy304-kernel"},
  {"nvidia-legacy340-kernel"},
  {"openoffice.org"},
  {"openoffice-langpack"},
  {"pentadactyl"},
  {"r8168"},
  {"rtl8188eu"},
  {"spl-solaris"},
  {"sysdig"},
  {"steamos-xpad"},
  {"vhba-module"},
  {"virtualbox-kernel-addons"},
  {"virtualbox-kernel"},
  {"xtables-addons"},
  {"zfs-on-linux"}
};

/*******************************************************************************

Determines whether a special upgrade check is needed

*******************************************************************************/
bool BuildListItem::requiresSpecialUpgradeCheck() const
{
  unsigned int i, nspecial;

  nspecial = special_upgrade_check.size();
  for ( i = 0; i < nspecial; i++ )
  {
    if (_name == special_upgrade_check[i]) { return true; }
  }

  return false;
}

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

  // For SlackBuilds in the special_upgrade_check list, allow installed version
  // to match even if it has a trailing underscore after the available_version
  // string.

  if ( (getBoolProp("installed")) && (! getBoolProp("blacklisted")) )
  {
    if (installed != available)
    {
      if (requiresSpecialUpgradeCheck())
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
      else { test = true; }
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
  addProp("build_options", "");
  addProp("package_name", "");
  addProp("action", "");
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
  else
  {
    setBoolProp("installed", false);
    setProp("installed_version", "");
    setProp("package_name", "");
    setBoolProp("blacklisted", false);
    setBoolProp("upgradable", false);
  }
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

/*******************************************************************************

Returns build options as string of environment variables

*******************************************************************************/
std::string BuildListItem::buildOptionsEnv() const
{
  int i, noptions;
  std::vector<std::string> build_options;
  std::string build_options_string;

  build_options = split(getProp("build_options"), ';');
  build_options_string = "";
  noptions = build_options.size();
  for ( i = 0; i < noptions-1; i++ )
  {
    build_options_string += build_options[i] + " ";
  }
  if (noptions > 0) { build_options_string += build_options[noptions-1]; }

  return build_options_string;
}
