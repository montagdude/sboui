#include <stdio.h>
#include <vector>
#include <string>
#include "DirListing.h"
#include "BuildListItem.h"
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

Returns list of installed SlackBuilds

*******************************************************************************/
std::vector<std::string> list_installed()
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
