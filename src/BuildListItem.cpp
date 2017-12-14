#include <string>
#include <vector>
#include <algorithm> 	// count
#include <cctype>	// isdigit
#include "backend.h"
#include "string_util.h"
#include "ListItem.h"
#include "BuildListItem.h"

/*******************************************************************************

Checks if installed version is the same as available version, but with a kernel
tag appended. Returns true if the two match except for a kernel tag appended to
the installed version string. Otherwise returns false.

*******************************************************************************/
bool BuildListItem::differsByKernel(const std::string & installed_version,
                                    const std::string & available_version) const
{
  std::size_t underpos;
  unsigned int lenavail, ndot, i;
  std::vector<std::string> splitstr;
  std::string tag;

  // Check if the version matches

  lenavail = available_version.length();
  if (installed_version.length() < lenavail)
    return false;
  else if (installed_version.substr(0, lenavail) != available_version)
    return false;
  
  // Check if there is a kernel tag appended

  underpos = installed_version.find_first_of('_');
  if (underpos == std::string::npos)
    return false;

  splitstr = split(installed_version, '_');
  tag = splitstr[1];

  ndot = std::count(tag.begin(), tag.end(), '.');
  if (ndot != 2)
    return false;

  splitstr = split(tag, '.');
  for ( i = 0; i < splitstr.size(); i++ )
  {
    if ( ! is_integer(splitstr[i]) )
      return false;
  }
  return true;
}

/*******************************************************************************

Checks whether SlackBuild can be upgraded

*******************************************************************************/
bool BuildListItem::upgradable() const
{
  bool test_version, test_buildnum;
  std::string installed_version, available_version;
  std::string installed_buildnum, available_buildnum;

  test_version = false;
  installed_version = getProp("installed_version");
  available_version = getProp("available_version");

  test_buildnum = false;
  installed_buildnum = getProp("installed_buildnum");
  available_buildnum = getProp("available_buildnum");

  // Check if new VERSION or BUILD is available

  if ( (getBoolProp("installed")) && (! getBoolProp("blacklisted")) )
  {
    if (installed_version != available_version)
    {
      if (! differsByKernel(installed_version, available_version))
        test_version = true;
    }
    if (installed_buildnum != available_buildnum)
      test_buildnum = true;
  }

  return (test_version || test_buildnum);
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
  addProp("installed_buildnum", "");
  addProp("available_buildnum", "");
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
    parseBuildNum(build);
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
  std::string available_version, reqs, available_buildnum;
  int check;

  check = get_repo_info(*this, available_version, reqs, available_buildnum);
  if (check == 0)
  {
    setProp("available_version", available_version);
    setProp("requires", reqs);
    setProp("available_buildnum", available_buildnum);
    if (getBoolProp("installed")) { setBoolProp("upgradable", upgradable()); }
  }

  return check;
}

/*******************************************************************************

Determines BUILD number from last portion of package name and sets it in
installed_buildnum prop

*******************************************************************************/
void BuildListItem::parseBuildNum(std::string & build)
{
  int i, buildlen;
  std::string buildnum;

  buildnum = "";
  buildlen = build.length();
  for ( i = 0; i < buildlen; i++ )
  {
    if (std::isdigit(build[i]))
      buildnum += build[i];
    else
      break;
  }
  if (buildnum.length() == 0) { buildnum = '0'; }
      
  setProp("installed_buildnum", buildnum);
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
