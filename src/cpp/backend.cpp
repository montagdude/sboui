#include <stdio.h>    // popen
#include <stdlib.h>   // system
#include <sys/wait.h> // WEXITSTATUS, WIFEXITED
#include <vector>
#include <string>
#include <sstream>
#include "DirListing.h"
#include "BuildListItem.h"
#include "sorting.h"
#include "backend.h"
 
/* Backend settings */
//FIXME: put most of these in a separate settings module/class?

//std::string repo_dir = "/var/cache/packages/SBo";
std::string repo_dir = "/data/dprosser/software/sboui_files/SBo";
std::string package_manager = "sbomgr";
std::string sync_cmd = "sbomgr update";
std::string install_cmd = "sbomgr install -n";
std::string upgrade_cmd = "sbomgr upgrade";
std::string install_vars = "";
std::string install_opts = "";
std::string upgrade_vars = "";
std::string upgrade_opts = "";

// Editor for viewing files 
std::string editor = "vim";

// Bash script with functions to query the repo and installed packages
//FIXME: location should be set by preprocessor macro depending on configure --prefix=
//std::string sboutil = "/usr/libexec/sboui/sboutil.sh";
std::string sboutil = "/data/dprosser/software/sboui_files/sboui/src/shell/sboutil.sh";

// Config variables to always pass to sboutil
std::string env = "REPO_DIR=" + repo_dir + " TAG=SBo ";

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

Trims white space, line ending characters, etc. from end of string

*******************************************************************************/
std::string trim(std::string instr)
{
  int i, trimlen, len;

  len = instr.size();
  trimlen = len;
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

Splits a string into a vector of strings. Adapted from the answer here:
http://stackoverflow.com/questions/236129/split-a-string-in-c#236803

*******************************************************************************/
std::vector<std::string> split(const std::string & instr, char delim)
{
  std::stringstream ss(instr);
  std::vector<std::string> splitstr;
  std::string item;

  while (std::getline(ss, item, delim)) { splitstr.push_back(trim(item)); }

  return splitstr;
}

/*******************************************************************************

Populates list of installed SlackBuilds. Also determines installed version,
available version, and dependencies for installed SlackBuilds.

*******************************************************************************/
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist)
{
  std::vector<std::string> installednames, pkg_info;
  unsigned int ninstalled, i, j, nbuilds;
  std::string installed_version, pkg_name, available_version, reqs;

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
        pkg_info = get_installed_info(slackbuilds[i]);
        installed_version = pkg_info[0];
        pkg_name = pkg_info[1];
        available_version = get_available_version(slackbuilds[i]);
        reqs = get_reqs(slackbuilds[i]);
        slackbuilds[i].setProp("installed_version", installed_version);
        slackbuilds[i].setProp("package_name", pkg_name);
        slackbuilds[i].setProp("available_version", available_version);
        slackbuilds[i].setProp("requires", reqs);
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

  cmd = install_vars + " " + install_cmd + " " + install_opts + build.name();
  return run_command(cmd);
}

/*******************************************************************************

Upgrades a SlackBuild

*******************************************************************************/
int upgrade_slackbuild(const BuildListItem & build)
{
  std::string cmd;

  cmd = upgrade_vars + " " + upgrade_cmd + " " + upgrade_opts + build.name();
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
  std::string cmd;

  cmd = editor + " " + repo_dir + "/" + build.getProp("category") + "/"
                                      + build.name() + "/" + "README";
  return run_command(cmd);
}

/*******************************************************************************

Opens a file in the editor (note: doesn't check for existence of the file)

*******************************************************************************/
int view_file(const std::string & path)
{
  std::string cmd;

  cmd = editor + " " + path;
  return run_command(cmd);
}

/*******************************************************************************

Syncs/updates SlackBuilds repository

*******************************************************************************/
int sync() { return run_command(sync_cmd); }
