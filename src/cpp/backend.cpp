#include <iostream>
#include <stdio.h>    // popen
#include <stdlib.h>   // system
#include <sys/wait.h> // WEXITSTATUS, WIFEXITED
#include <vector>
#include <string>
#include <sstream>
#include "DirListing.h"
#include "BuildListItem.h"
#include "sorting.h"
#include "string_util.h"
#include "settings.h"
#include "backend.h"

#ifndef PKGLIBEXECDIR
  #define PKGLIBEXECDIR "/usr/libexec/sboui"
#endif

using namespace settings;
 
// Bash script with functions to query the repo and installed packages
//FIXME: autoconf needs to set INSTALL_PREFIX
std::string sboutil = PKGLIBEXECDIR "/sboutil.sh";
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
        if (build_entry.type == "dir")
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

Returns list of installed SlackBuilds' names

*******************************************************************************/
std::vector<std::string> list_installed_names()
{
  std::vector<std::string> pkglist;
  char buffer[128];
  FILE* fp;
  std::string cmd, pkg;

  pkglist.resize(0);
  cmd = env + sboutil + " list_installed";
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL)
  {
    pkg = buffer;
    pkglist.push_back(trim(pkg));
  } 

  return pkglist;
}

/*******************************************************************************

Gets version and package name for installed SlackBuild

*******************************************************************************/
std::vector<std::string> get_installed_info(const BuildListItem & build)
{
  char buffer[128];
  FILE* fp;
  std::string cmd, pkg_info;
  std::vector<std::string> output;

  cmd = env + sboutil + " get_installed_info " + build.name();
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL) 
  { 
    pkg_info = buffer;
    output.push_back(trim(pkg_info));
  }  
  pclose(fp);

  return output;
}

/*******************************************************************************

Gets available version of a SlackBuild from the repository

*******************************************************************************/
std::string get_available_version(const BuildListItem & build)
{
  char buffer[128];
  FILE* fp;
  std::string cmd, version;

  cmd = env + sboutil + " get_available_version " + build.name() + " "
                                                  + build.getProp("category");
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL) { version = buffer; }
  pclose(fp);

  return trim(version);
}

/*******************************************************************************

Gets SlackBuild requirements (dependencies) as string

*******************************************************************************/
std::string get_reqs(const BuildListItem & build)
{
  char buffer[1024];
  FILE* fp;
  std::string cmd, reqs;

  cmd = env + sboutil + " get_reqs " + build.name() + " "
                                     + build.getProp("category");
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL) { reqs = buffer; }
  pclose(fp);

  return reqs;
}

/*******************************************************************************

Populates list of installed SlackBuilds. Also determines installed version,
available version, and dependencies for installed SlackBuilds.

*******************************************************************************/
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist)
{
  std::vector<std::string> installednames;
  unsigned int ninstalled, i, j, nbuilds;

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
        slackbuilds[i].readPropsFromRepo();
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
  unsigned int i, j, k, ninstalled, ndeps;
  bool isdep;
  std::vector<std::string> deplist;

  nondeplist.resize(0);

  // N^2 (max) loop through installed packages to see which are dependencies

  ninstalled = installedlist.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    isdep = false;
    for ( j = 0; j < ninstalled; j++ )
    {
      if (j == i) { continue; }
      deplist = split(installedlist[j]->getProp("requires"));
      ndeps = deplist.size();
      for ( k = 0; k < ndeps; k++ )
      {
        if (deplist[k] == installedlist[i]->name())
        {
          isdep = true;
          break;
        }
      }
      if (isdep) { break; }
    } 
    if (! isdep) { nondeplist.push_back(installedlist[i]); }
  } 
}

/*******************************************************************************

Runs system command and returns exit status

*******************************************************************************/
int run_command(const std::string & cmd)
{
  int check;

  check = system(cmd.c_str());

  // See `man waitpid` for more info on WEXITSTATUS and WIFEXITED

  if (WIFEXITED(check)) { return WEXITSTATUS(check); }
  else { return -1; }
}

/*******************************************************************************

Installs a SlackBuild

*******************************************************************************/
int install_slackbuild(const BuildListItem & build)
{
  std::string cmd;

  cmd = install_vars + " " + install_cmd + " " + build.name() + " " + 
        install_clos;
  return run_command(cmd);
}

/*******************************************************************************

Upgrades a SlackBuild

*******************************************************************************/
int upgrade_slackbuild(const BuildListItem & build)
{
  std::string cmd;

  cmd = upgrade_vars + " " + upgrade_cmd + " " + build.name() + " " +
        upgrade_clos;
  return run_command(cmd);
}

/*******************************************************************************

Removes a SlackBuild

*******************************************************************************/
int remove_slackbuild(const BuildListItem & build)
{
  std::string cmd;

  cmd = "removepkg " + build.getProp("package_name");
  return run_command(cmd);
}

/*******************************************************************************

Displays README for a SlackBuild

*******************************************************************************/
int view_readme(const BuildListItem & build)
{
  std::string cmd, response;
  int retval;

  cmd = editor + " " + repo_dir + "/" + build.getProp("category") + "/"
                                      + build.name() + "/" + "README";
  retval = run_command(cmd);
  if (retval != 0)
  {
    std::cout << "Unable to view README. Press Enter to continue ...";
    std::getline(std::cin, response);
  }

  return retval;
}

/*******************************************************************************

Opens a file in the editor (note: doesn't check for existence of the file)

*******************************************************************************/
int view_file(const std::string & path)
{
  std::string cmd, response;
  int retval;

  cmd = editor + " " + path;
  retval = run_command(cmd);
  if (retval != 0)
  {
    std::cout << "Unable to view file. Press Enter to continue ...";
    std::getline(std::cin, response);
  }

  return retval;
}

/*******************************************************************************

Syncs/updates SlackBuilds repository

*******************************************************************************/
int sync_repo() 
{ 
  int retval;
  std::string response;

  retval = run_command(sync_cmd);
  std::cout << "Press Enter to return ...";
  std::getline(std::cin, response);

  return retval;
}
