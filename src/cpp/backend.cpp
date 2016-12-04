#include <stdio.h>
#include <vector>
#include <string>
#include "DirListing.h"
#include "BuildListItem.h"
#include "sorting.h"
#include "backend.h"

std::string repo_dir = "/var/cache/packages/SBo";
//std::string repo_dir = "/data/dprosser/software/sboui_files/SBo";
std::string package_manager = "sbomgr";
std::string sync_cmd = "sbomgr update";
std::string install_cmd = "sbomgr install -n";
std::string upgrade_cmd = "sbomgr upgrade";

// Bash script with functions to query the repo and installed packages
std::string sboutil = "/usr/libexec/sboui/sboutil.sh";
//std::string sboutil = "/data/dprosser/software/sboui_files/sboui/src/shell/sboutil.sh";

/*******************************************************************************

Gets list of SlackBuilds by reading repo directory. Returns 0 if successful,
1 if directory cannot be read.

*******************************************************************************/
int read_repo(std::vector<BuildListItem> & slackbuilds)
{
  DirListing top_dir, category_dir;
  int stat;
  unsigned int i, j, ncategories, nbuilds;
  direntry cat_entry, build_entry;

  // Open top directory

  stat = top_dir.setFromPath(repo_dir);
  if (stat == 1) { return stat; }
  
  // Read SlackBuilds from each category

  slackbuilds.resize(0); 
  ncategories = top_dir.size();
  for ( i = 0; i < ncategories; i++ )
  {
    cat_entry = top_dir(i);
    if (cat_entry.type == "dir")
    {
      category_dir.setFromPath(cat_entry.path + "/" + cat_entry.name);
      nbuilds = category_dir.size();
      for ( j = 0; j < nbuilds; j++ )
      {
        build_entry = category_dir(j);
        if (build_entry.type == "dir");
        {
          BuildListItem build;
          build.setName(build_entry.name);
          build.setProp("category", cat_entry.name);
          slackbuilds.push_back(build);
        }
      }
    }
  }  

  return 0;
} 

/*******************************************************************************

Trims white space, line ending characters, etc. from end of string

*******************************************************************************/
std::string trim(std::string instr)
{
  int i, trimlen, len;

  len = instr.size();
  for ( i = len-1; i >= 0; i-- )
  {
    if ( (instr[i] != ' ') && (instr[i] != '\n') && (instr[i] != '\0') )
    {
      trimlen = i+1;
      break;
    }
  }

  return instr.substr(0, trimlen);
}

/*******************************************************************************

Returns list of installed SlackBuilds' names

*******************************************************************************/
std::vector<std::string> list_installed_names()
{
  std::vector<std::string> pkglist;
  char buffer[128];
  FILE* fp;
  std::string cmd, pkg;

  pkglist.resize(0);
  cmd = sboutil + " list_installed";
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL)
  {
    pkg = buffer;
    pkglist.push_back(trim(pkg));
  } 

  return pkglist;
}

/*******************************************************************************

Checks whether the specified SlackBuild is installed. Returns installed version
if so; otherwise returns "not_installed".

*******************************************************************************/
std::string check_installed(const BuildListItem & build)
{
  char buffer[128];
  FILE* fp;
  std::string cmd, version;

  cmd = sboutil + " check_installed " + build.name();
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL) { version = buffer; }  
  pclose(fp);

  return trim(version);
}

/*******************************************************************************

Gets available version of a SlackBuild from the repository

*******************************************************************************/
std::string get_available_version(const BuildListItem & build)
{
  char buffer[128];
  FILE* fp;
  std::string cmd, version;

  cmd = sboutil + " get_available_version " + build.name() + " "
                                            + build.getProp("category");
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL) { version = buffer; }
  pclose(fp);

  return trim(version);
}

/*******************************************************************************

Populates list of installed SlackBuilds. Also determines installed version
and available version for installed SlackBuilds.

*******************************************************************************/
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist)
{
  std::vector<std::string> installednames;
  unsigned int ninstalled, i, j, nbuilds;
  std::string installed_version, available_version;

  installedlist.resize(0);
  installednames = list_installed_names();
  ninstalled = installednames.size();
  nbuilds = slackbuilds.size();
  for ( j = 0; j < ninstalled; j++ )
  {
    for ( i = 0; i < nbuilds; i++ )
    {
      if (installednames[j] == slackbuilds[i].name())
      {
        slackbuilds[i].setBoolProp("installed", true);
        installed_version = check_installed(slackbuilds[i]);
        available_version = get_available_version(slackbuilds[i]);
        slackbuilds[i].setProp("installed_version", installed_version);
        slackbuilds[i].setProp("available_version", available_version);
        installedlist.push_back(&slackbuilds[i]);
        break;
      }
    }
  } 

  // Sort by name and then by category

  sort_list(installedlist, "name", compare_by_prop);
  sort_list(installedlist, "category", compare_by_prop);
}

/*******************************************************************************

Populates list of installed SlackBuilds that are not required by any other 
installed SlackBuild

*******************************************************************************/
void list_nondeps(const std::vector<BuildListItem *> & installedlist,
                        std::vector<BuildListItem *> & nondeplist)
{
  char buffer[128];
  FILE* fp;
  std::string cmd, check;
  unsigned int i, j, ninstalled;
  bool isdep;

  nondeplist.resize(0);

  // N^2 (max) loop through installed packages to see which are dependencies

  ninstalled = installedlist.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    isdep = false;
    for ( j = 0; j < ninstalled; j++ )
    {
      if (j == i) { continue; }
      cmd = sboutil + " is_dep " + installedlist[i]->name() + " "
                                 + installedlist[j]->name() + " " 
                                 + installedlist[j]->getProp("category");
      fp = popen(cmd.c_str(), "r");
      while (fgets(buffer, sizeof(buffer), fp) != NULL) { check = buffer; }
      pclose(fp);
      if (trim(check) == "1") 
      { 
        isdep = true;
        break; 
      }
    }
    if (! isdep) { nondeplist.push_back(installedlist[i]); }
  } 
}
