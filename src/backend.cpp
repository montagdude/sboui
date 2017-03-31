#include <iostream>
#include <stdlib.h>   // system
#include <sys/wait.h> // WEXITSTATUS, WIFEXITED
#include <vector>
#include <string>
#include <sstream>
#include "DirListing.h"
#include "BuildListItem.h"
#include "sorting.h"
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
    if (name == build.name()) { return true; }
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

Populates list of installed SlackBuilds. Also determines installed version,
available version, and dependencies for installed SlackBuilds.

*******************************************************************************/
void list_installed(std::vector<std::vector<BuildListItem> > & slackbuilds,
                    std::vector<BuildListItem *> & installedlist)
{
  std::vector<std::string> installedpkgs;
  std::string pkgname, pkgversion, pkgbuild, pkgbuildnum;
  unsigned int ninstalled, i, j, k, ncategories, nbuilds;

  installedlist.resize(0);
  installedpkgs = list_installed_packages();
  ninstalled = installedpkgs.size();
  ncategories = slackbuilds.size();
  for ( k = 0; k < ninstalled; k++ )
  {
    get_pkg_info(installedpkgs[k], pkgname, pkgversion, pkgbuildnum);
    for ( i = 0; i < ncategories; i++ )
    {
      nbuilds = slackbuilds[i].size();
      for ( j = 0; j < nbuilds; j++ )
      { 
        if (pkgname == slackbuilds[i][j].name())
        {
          slackbuilds[i][j].setBoolProp("installed", true);
          slackbuilds[i][j].setProp("installed_version", pkgversion);
          slackbuilds[i][j].setProp("package_name", installedpkgs[i]);
          slackbuilds[i][j].setProp("package_build", pkgbuildnum);
          slackbuilds[i][j].readPropsFromRepo();
          installedlist.push_back(&slackbuilds[i][j]);
          break;
        }
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
