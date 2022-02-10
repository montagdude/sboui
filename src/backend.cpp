#include <iostream>
#include <stdlib.h>   // system
#include <sys/wait.h> // WEXITSTATUS, WIFEXITED
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>  // sort
#include <fstream>
#include <atomic>
#include <ctime>      // strftime
#include "DirListing.h"
#include "ListItem.h"
#include "BuildListItem.h"
#include "string_util.h"
#include "ShellReader.h"
#include "settings.h"
#include "Blacklist.h"
#include "backend.h"

#ifndef PACKAGE_DIR
  #define PACKAGE_DIR "/var/lib/pkgtools/packages"
#endif

using namespace settings;

Blacklist package_blacklist;

/*******************************************************************************

Gets list of SlackBuilds by reading repo directory. Returns 0 if successful,
1 if directory cannot be read, 2 if directory is empty.

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
  if (ncategories == 0) { return 2; }
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

Reads build options in /var/lib/sboui/buildopts

*******************************************************************************/
int read_buildopts(std::vector<std::vector<BuildListItem> > & slackbuilds)
{
  DirListing listing;
  std::string buildopts_dir, fpath, buildopts, opt, buildname;
  unsigned int k, nbuilds_opts, stat;
  int i, j;
  std::ifstream file;
  std::size_t ext_idx;

  // Open directory

  buildopts_dir = "/var/lib/sboui/buildopts";
  stat = listing.setFromPath(buildopts_dir);
  if (stat == 1) { return stat; }

  // Read build options for each SlackBuild

  nbuilds_opts = listing.size();
  for ( k = 0; k < nbuilds_opts; k++ )
  {
    fpath = buildopts_dir + "/" + listing(k).name;
    ext_idx = listing(k).name.find(".buildopts");
    if (ext_idx == std::string::npos) { continue; }
    file.open(fpath.c_str());
    buildopts = "";
    while (1)
    { 
      std::getline(file, opt);
      buildopts += opt;
      if (file.eof()) { break; }
      else { buildopts += ";"; }
    }
    buildname = listing(k).name.substr(0,ext_idx);
    find_slackbuild(buildname, slackbuilds, i, j);
    slackbuilds[i][j].setProp("build_options", buildopts);
    file.close();
  }

  return 0;
}

/*******************************************************************************

Finds a SlackBuild by name in the _slackbuilds list. Returns 0 if found, 1 if
not found, and also sets indices in list where it was found.
 
*******************************************************************************/
int find_slackbuild(const std::string & name,
                    std::vector<std::vector<BuildListItem> > & slackbuilds,
                    int & idx0, int & idx1)
{
  int i, ncategories, nbuilds, check, lbound, rbound;
  std::atomic<bool> found(false);

  ncategories = slackbuilds.size();
#pragma omp parallel for private(i,nbuilds,lbound,rbound,check)
  for ( i = 0; i < ncategories; i++ )
  {
    if (found)
      continue;
    nbuilds = slackbuilds[i].size();
    lbound = 0;
    rbound = nbuilds-1;
    check = find_name_in_list(name, slackbuilds[i], idx1, lbound, rbound);
    if (check == 0)
    {
      idx0 = i;
      found = true;
    }
  }

  if (found)
    return 0;
  else
    return 1;
}

/*******************************************************************************

Finds a SlackBuild by name in a list of slackbuilds pointers. Returns 0 if
found, 1 if not found, and also sets indices in list where it was found.
Overloaded version of above.

*******************************************************************************/
int find_slackbuild(const std::string & name,
                    std::vector<std::vector<BuildListItem *> > & slackbuilds,
                    int & idx0, int & idx1)
{
  int i, ncategories, nbuilds, check, lbound, rbound;
  std::atomic<bool> found(false);

  ncategories = slackbuilds.size();
#pragma omp parallel for private(i,nbuilds,lbound,rbound,check)
  for ( i = 0; i < ncategories; i++ )
  {
    if (found)
      continue;
    nbuilds = slackbuilds[i].size();
    lbound = 0;
    rbound = nbuilds-1;
    check = find_name_in_list(name, slackbuilds[i], idx1, lbound, rbound);
    if (check == 0)
    {
      idx0 = i;
      found = true;
    }
  }

  if (found)
    return 0;
  else
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

Gets package info from entry in installed package list. Returns 1 if package
name is badly formed; 0 otherwise.

*******************************************************************************/
int get_pkg_info(const std::string & pkg, std::string & name,
                 std::string & version, std::string & arch,
                 std::string & build)
{
  std::vector<std::string> splitpkg;
  unsigned int i, nsplit;

  name = "";
  version = "";
  arch = "";
  build = "";

  splitpkg = split(pkg, '-');
  nsplit = splitpkg.size();
  if (nsplit < 4) { return 1; }

  name = ""; 
  for ( i = 0; i < nsplit-4; i++ ) { name += splitpkg[i] + "-"; }
  name += splitpkg[nsplit-4];
  version = splitpkg[nsplit-3];
  arch = splitpkg[nsplit-2];
  build = splitpkg[nsplit-1];

  return 0;
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
                     std::string & arch, std::string & pkgbuild)
{
  std::string name;
  unsigned int ninstalled, i;

  ninstalled = installedpkgs.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    get_pkg_info(installedpkgs[i], name, version, arch, pkgbuild);
    if (name == build.name())
    {
      pkg = installedpkgs[i];
      return true;
    }
  }
    
  pkg = "";
  version = "";
  arch = "";
  pkgbuild = "";

  return false;
}

/*******************************************************************************

Gets SlackBuild requirements (dependencies) as string

*******************************************************************************/
int get_reqs(const BuildListItem & build, std::string & reqs)
{
  ShellReader reader;
  std::string info_file;
  int check;

  info_file = repo_dir + "/" + build.getProp("category") + "/" +
              build.name() + "/" + build.name() + ".info";

  reqs = "";
  check = reader.open(info_file);
  if (check == 0)
  {
    reader.read("REQUIRES", reqs);
    reader.close();
  }

  return check;
}

/*******************************************************************************

Gets SlackBuild version and reqs from repository
Note: this function seems to be the biggest bottleneck for speed.

*******************************************************************************/
int get_repo_info(const BuildListItem & build, std::string & available_version,
                  std::string & reqs, std::string & available_buildnum) 
{
  ShellReader reader;
  std::string info_file, slackbuild_file;
  int check;

  // Read available version and requirements from .info file

  info_file = repo_dir + "/" + build.getProp("category") + "/" +
              build.name() + "/" + build.name() + ".info";
  check = reader.open(info_file);
  if (check == 0)
  { 
    reader.read("VERSION", available_version);
    reader.read("REQUIRES", reqs);
    reader.close();
  }
  else { return check; }

  // Read build number from .SlackBuild file

  slackbuild_file = repo_dir + "/" + build.getProp("category") + "/" +
                    build.name() + "/" + build.name() + ".SlackBuild";
  check = reader.open(slackbuild_file);
  if (check == 0)
  { 
    reader.read("BUILD", available_buildnum, true);
    reader.close();
  }

  return check;
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

Determines which SlackBuilds are installed and sets related properties. Also
checks for invalid names and missing .info files.

*******************************************************************************/
void determine_installed(std::vector<std::vector<BuildListItem> > & slackbuilds,
                         std::vector<std::string> & pkg_errors,
                         std::vector<std::string> & missing_info)
{
  std::vector<std::string> installedpkgs;
  std::string name, version, arch, build, curcategory, response;
  unsigned int ninstalled, k;
  int i, j, check, pkgcheck, infocheck;

  pkg_errors.resize(0);
  missing_info.resize(0);
  installedpkgs = list_installed_packages();
  ninstalled = installedpkgs.size();
#pragma omp parallel for private(k,pkgcheck,name,version,arch,build,check,i,j,\
                                 infocheck)
  for ( k = 0; k < ninstalled; k++ )
  {
    // Check for invalid package names

    pkgcheck = get_pkg_info(installedpkgs[k], name, version, arch, build);
    if (pkgcheck != 0)
    {
#pragma omp critical
      { pkg_errors.push_back(installedpkgs[k]); }
      continue;
    }
    check = find_slackbuild(name, slackbuilds, i, j);
    if (check == 0)
    {
      slackbuilds[i][j].setBoolProp("installed", true);
      slackbuilds[i][j].setProp("installed_version", version);
      slackbuilds[i][j].parseBuildNum(build);
      slackbuilds[i][j].setProp("package_name", installedpkgs[k]);

      slackbuilds[i][j].setBoolProp("blacklisted",
                        package_blacklist.blacklisted(installedpkgs[k], name,
                                                      version, arch, build));

      // Read props, set upgradable status, and check for missing .info file

      infocheck = slackbuilds[i][j].readPropsFromRepo();
      if (infocheck != 0)
#pragma omp critical
        { missing_info.push_back(slackbuilds[i][j].name()); }
    }
  } 
}

/*******************************************************************************

Runs system command and returns exit status

*******************************************************************************/
int run_command(const std::string & cmd)
{
  int check;
  unsigned int i, nsplit;
  std::vector<std::string> splitcmd;
  std::string runcmd;

  // Get rid of extraneous spaces

  splitcmd = split(cmd);
  nsplit = splitcmd.size();
  runcmd = "";
  for ( i = 0; i < nsplit-1; i++ ) { runcmd += splitcmd[i] + " "; }
  runcmd += splitcmd[nsplit-1];
  
  check = system(cmd.c_str());

  // See `man waitpid` for more info on WEXITSTATUS and WIFEXITED

  if (WIFEXITED(check)) { return WEXITSTATUS(check); }
  else { return -1; }
}

/*******************************************************************************

Installs a SlackBuild

*******************************************************************************/
int install_slackbuild(BuildListItem & build)
{
  std::string cmd;
  int check;
  std::vector<std::string> installedpkgs;

  cmd = install_vars + " " + build.buildOptionsEnv() + " " + install_cmd
      + " " + build.name() + " " + install_clos;
  check = run_command(cmd);
  if (check != 0) { return check; }

  // Check to make sure it was actually installed and update properties

  installedpkgs = list_installed_packages();
  build.readInstalledProps(installedpkgs);
  if (build.getBoolProp("installed"))
  {
    build.readPropsFromRepo();
    return 0;
  }
  else { return 1; }
}

/*******************************************************************************

Upgrades a SlackBuild

*******************************************************************************/
int upgrade_slackbuild(BuildListItem & build)
{
  std::string cmd;
  int check;
  std::vector<std::string> installedpkgs;

  cmd = upgrade_vars + " " + build.buildOptionsEnv() + " " + upgrade_cmd
      + " " + build.name() + " " + upgrade_clos;
  check = run_command(cmd);
  if (check != 0) { return check; }

  // If upgrade didn't work (maybe package manager doesn't think it's 
  //  upgradable), reinstall instead

  installedpkgs = list_installed_packages();
  build.readInstalledProps(installedpkgs);
  build.readPropsFromRepo();
  if (build.getBoolProp("upgradable"))
  {
    check = reinstall_slackbuild(build); 
    if (check != 0) { return check; }
  }

  return 0;
}

/*******************************************************************************

Reinstalls a SlackBuild

*******************************************************************************/
int reinstall_slackbuild(BuildListItem & build)
{
  std::string cmd;
  int check;
  std::vector<std::string> installedpkgs;

  cmd = install_vars + " " + build.buildOptionsEnv() + " " + reinstall_cmd
      + " " + build.name() + " " + install_clos;
  check = run_command(cmd);
  if (check != 0) { return check; }

  // Check to make sure it was actually installed and update properties

  installedpkgs = list_installed_packages();
  build.readInstalledProps(installedpkgs);
  if (build.getBoolProp("installed"))
  {
    build.readPropsFromRepo();
    return 0;
  }
  else { return 1; }
}

/*******************************************************************************

Removes a SlackBuild

*******************************************************************************/
int remove_slackbuild(BuildListItem & build)
{
  std::string cmd;
  int check;
  std::vector<std::string> installedpkgs;

  cmd = "removepkg " + build.getProp("package_name");
  check = run_command(cmd);
  if (check != 0) { return check; }

  // Check to make sure it was actually removed

  installedpkgs = list_installed_packages();
  build.readInstalledProps(installedpkgs);
  if (build.getBoolProp("installed")) { return 1; }
  else { return 0; }
}

/*******************************************************************************

Displays README for a SlackBuild using viewer

*******************************************************************************/
int view_readme(const BuildListItem & build)
{
  std::string cmd, response;
  int retval;

  cmd = viewer + " " + repo_dir + "/" + build.getProp("category") + "/"
                                      + build.name() + "/" + "README";
  retval = run_command(cmd);
  if (retval != 0)
  {
    std::cout << "Unable to view README. "
              << "Press Enter to return to main window ...";
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
    std::cout << "Unable to open file. "
              << "Press Enter to return to main window ...";
    std::getline(std::cin, response);
  }

  return retval;
}

/*******************************************************************************

Opens notes for a SlackBuild using the editor

*******************************************************************************/
int view_notes(const BuildListItem & build)
{
  int check;
  DirListing listing;
  std::string notespath, response, fname;

  notespath = "/var/lib/sboui/notes";
  check = listing.createFromPath(notespath);
  if (check != 0)
  { 
    std::cout << "Unable to create directory " << notespath << ". "
              << "Please check permissions." << std::endl;
    std::cout << "Press Enter to return to main window ...";
    std::getline(std::cin, response);
    return 1;
  } 
  fname = listing.path() + build.name() + ".notes";

  return view_file(fname);
}

/*******************************************************************************

Syncs/updates SlackBuilds repository

*******************************************************************************/
int sync_repo(bool interactive)
{ 
  int retval;
  std::string response;

  retval = run_command(sync_cmd);

  // Record update time

  if (retval == 0)
  {
    DirListing listing;
    unsigned int stat;
    std::ofstream file;

    stat = listing.createFromPath("/var/lib/sboui");
    file.open("/var/lib/sboui/last-sync.txt");
    if ( (stat == 0) && file.is_open() )
    {
      time_t raw_time;
      struct tm * timeinfo;
      char buffer[100];

      time(&raw_time);
      timeinfo = gmtime(&raw_time);
      strftime(buffer,sizeof(buffer),"%a, %d %b %Y %H:%M:%S %Z", timeinfo);
      std::string timestr(buffer);
      file << timestr << std::endl;
      file.close();
    }
    else
    {
      std::cout << "Warning: unable to save update time to "
                << "/var/lib/sboui/last-sync.txt." << std::endl;
    }
  }

  if (interactive)
  {
    std::cout << "Press Enter to return to main window ...";
    std::getline(std::cin, response);
  }

  return retval;
}
