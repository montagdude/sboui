#include <iostream>
#include <stdlib.h>   // system
#include <sys/wait.h> // WEXITSTATUS, WIFEXITED
#include <vector>
#include <string>
#include <sstream>
#include <cmath>      // floor
#include <algorithm>  // sort
#include "DirListing.h"
#include "BuildListItem.h"
#include "string_util.h"
#include "ShellReader.h"
#include "settings.h"
#include "backend.h"

#ifndef PACKAGE_DIR
  #define PACKAGE_DIR "/var/log/packages"
#endif

using namespace settings;

/*******************************************************************************

Gets list of SlackBuilds by reading repo directory. Returns 0 if successful,
1 if directory cannot be read.

*******************************************************************************/
int read_repo(std::vector<std::vector<BuildListItem> > & slackbuilds)
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
      std::vector<BuildListItem> cat_builds;
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
          cat_builds.push_back(build);
        }
        else { break; }   // Directories are listed first, so we're done
      }
      slackbuilds.push_back(cat_builds);
    }
    else { break; }       // Directories are listed first, so we're done
  }  

  return 0;
} 

/*******************************************************************************

Overloaded template functions to turn reference into pointer
http://stackoverflow.com/questions/14466620/c-template-specialization-calling-methods-on-types-that-could-be-pointers-or#14466705

*******************************************************************************/
template<typename T>
T * ptr(T & obj) { return & obj; }

template<typename T>
T * ptr(T * obj) { return obj; }

/*******************************************************************************

Finds a SlackBuild by name in a sorted list. Returns 0 if found, 1 if not found.

*******************************************************************************/
template<typename T>
int find_build_in_list(const std::string & name, std::vector<T> & buildlist,
                       int & idx, int & lbound, int & rbound)
{
  int midbound;

  // Check if outside the bounds

  if ( (name < ptr(buildlist[lbound])->name()) ||
       (name > ptr(buildlist[rbound])->name()) ) { return 1; }

  // Check for match on bounds. Return if not found and bounds are consecutive.

  if (name == ptr(buildlist[lbound])->name())
  {
    idx = lbound;
    return 0;
  }
  else if (name == ptr(buildlist[rbound])->name())
  {
    idx = rbound;
    return 0;
  }
  else { if (rbound-lbound == 1) { return 1; } }

  // Cut the list in half and try again

  midbound = std::floor(double(lbound+rbound)/2.);
  if (name <= ptr(buildlist[midbound])->name()) { rbound = midbound; }
  else { lbound = midbound; }
  return find_build_in_list(name, buildlist, idx, lbound, rbound);
}

/*******************************************************************************

Finds a SlackBuild by name in the _slackbuilds list. Returns 0 if found, 1 if
not found.
 
*******************************************************************************/
int find_slackbuild(const std::string & name,
                    std::vector<std::vector<BuildListItem> > & slackbuilds,
                    int & idx0, int & idx1)
{
  int i, ncategories, nbuilds, check, lbound, rbound;

  ncategories = slackbuilds.size();
  for ( i = 0; i < ncategories; i++ )
  {
    nbuilds = slackbuilds[i].size();
    lbound = 0;
    rbound = nbuilds-1;
    check = find_build_in_list(name, slackbuilds[i], idx1, lbound, rbound);
    if (check == 0)
    {
      idx0 = i;
      return 0;
    }
  }

  return 1;
}

/*******************************************************************************

Gets basename of a file (removes path)

*******************************************************************************/
std::string basename(const std::string & fullpath)
{
  std::vector<std::string> splitpkg;

  splitpkg = split(fullpath, '/');
  return splitpkg[splitpkg.size()-1];
}

/*******************************************************************************

Gets package info from entry in installed package list

*******************************************************************************/
void get_pkg_info(const std::string & pkg, std::string & name,
                  std::string & version, std::string & buildnum)
{
  std::vector<std::string> splitpkg;
  unsigned int i, nsplit;

  splitpkg = split(pkg, '-');
  nsplit = splitpkg.size();
  name = ""; 
  for ( i = 0; i < nsplit-4; i++ ) { name += splitpkg[i] + "-"; }
  name += splitpkg[nsplit-4];
  version = splitpkg[nsplit-3];
  buildnum = splitpkg[nsplit-1];
}

/*******************************************************************************

Returns list of all installed packages (not just the ones from the repo)

*******************************************************************************/
std::vector<std::string> list_installed_packages()
{
  std::vector<std::string> pkglist;
  DirListing packages_dir(std::string(PACKAGE_DIR), false, false);
  unsigned int npackages, i;

  npackages = packages_dir.size();
  for ( i = 0; i < npackages; i++ )
  {
    pkglist.push_back(basename(packages_dir(i).path + packages_dir(i).name));
  }

  return pkglist;
}

/*******************************************************************************

Checks if a SlackBuild is installed and sets info if so.

*******************************************************************************/
bool check_installed(const BuildListItem & build,
                     const std::vector<std::string> & installedpkgs,
                     std::string & pkg, std::string & version,
                     std::string & buildnum)
{
  std::string name;
  unsigned int ninstalled, i;

  ninstalled = installedpkgs.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    get_pkg_info(installedpkgs[i], name, version, buildnum);
    if (name == build.name())
    {
      pkg = installedpkgs[i];
      return true;
    }
  }
    
  pkg = "";
  version = "";
  buildnum = "";
  return false;
}

/*******************************************************************************

Gets SlackBuild requirements (dependencies) as string

*******************************************************************************/
std::string get_reqs(const BuildListItem & build)
{
  ShellReader reader;
  std::string info_file, reqs;

  info_file = repo_dir + "/" + build.getProp("category") + "/" +
              build.name() + "/" + build.name() + ".info";

  reader.open(info_file);
  reader.read("REQUIRES", reqs);
  reader.close();

  return reqs;
}

/*******************************************************************************

Gets SlackBuild version and reqs from repository

*******************************************************************************/
void get_repo_info(const BuildListItem & build, std::string & available_version,
                   std::string & reqs)
{
  ShellReader reader;
  std::string info_file;

  info_file = repo_dir + "/" + build.getProp("category") + "/" +
              build.name() + "/" + build.name() + ".info";

  reader.open(info_file);
  reader.read("VERSION", available_version);
  reader.read("REQUIRES", reqs);
  reader.close();
}

/*******************************************************************************

Compares entries by name. Returns true if the first argument is less than the
second.

*******************************************************************************/
bool compare_builds_by_name(const BuildListItem *item1,
                            const BuildListItem *item2)
{
  return item1->name() < item2->name();
}

/*******************************************************************************

Compares entries by category. Returns true if the first argument is less than
the second.

*******************************************************************************/
bool compare_builds_by_category(const BuildListItem *item1,
                                const BuildListItem *item2)
{
  return item1->getProp("category") < item2->getProp("category");
}
 
/*******************************************************************************

Populates list of installed SlackBuilds. Also determines installed version,
available version, and dependencies for installed SlackBuilds.

*******************************************************************************/
void list_installed(std::vector<std::vector<BuildListItem> > & slackbuilds,
                    std::vector<BuildListItem *> & installedlist)
{
  std::vector<std::string> installedpkgs;
  std::string pkgname, pkgversion, pkgbuild, pkgbuildnum, curcategory;
  unsigned int ninstalled, k, lbound, rbound;
  int i, j, check;

  installedlist.resize(0);
  installedpkgs = list_installed_packages();
  ninstalled = installedpkgs.size();
  for ( k = 0; k < ninstalled; k++ )
  {
    get_pkg_info(installedpkgs[k], pkgname, pkgversion, pkgbuildnum);
    check = find_slackbuild(pkgname, slackbuilds, i, j);
    if (check == 0)
    {
      slackbuilds[i][j].setBoolProp("installed", true);
      slackbuilds[i][j].setProp("installed_version", pkgversion);
      slackbuilds[i][j].setProp("package_name", installedpkgs[k]);
      slackbuilds[i][j].setProp("package_build", pkgbuildnum);
      slackbuilds[i][j].readPropsFromRepo();
      installedlist.push_back(&slackbuilds[i][j]);
    }
  } 

  // Sort by category and then by name within each category
  
  std::sort(installedlist.begin(), installedlist.end(),
            compare_builds_by_category);
  
  lbound = 0;
  rbound = 0;
  ninstalled = installedlist.size();
  while (rbound < ninstalled)
  {
    curcategory = installedlist[lbound]->getProp("category");
    for ( k = lbound; k < ninstalled; k++ )
    {
      rbound = k;
      if (installedlist[k]->getProp("category") != curcategory) { break; }
    }
    if (rbound == ninstalled-1) { rbound++; }

    // Don't sort the last element if it is the only one of its type
    
    if ( (rbound == ninstalled) &&
         (installedlist[rbound-1]->getProp("category") != curcategory) )
      std::sort(installedlist.begin()+lbound, installedlist.begin()+rbound-1,
                compare_builds_by_name);
    else
      std::sort(installedlist.begin()+lbound, installedlist.begin()+rbound,
                compare_builds_by_name);
    lbound = rbound;
  }
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
