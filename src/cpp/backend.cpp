#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include "DirListing.h"
#include "BuildListItem.h"
#include "backend.h"

#include <iostream>

std::string repo_dir = "/var/cache/packages/SBo";
std::string package_manager = "sbomgr";
std::string sync_cmd = "sbomgr update";
std::string install_cmd = "sbomgr install -n";
std::string upgrade_cmd = "sbomgr upgrade";

// Bash script with functions to query the repo and installed packages
std::string sboutil = "/usr/libexec/sboui/sboutil.sh";

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
          build.setCategory(cat_entry.name);
          slackbuilds.push_back(build);
        }
      }
    }
  }  

  return 0;
} 

/*******************************************************************************

Checks whether the specified SlackBuild is installed. Returns installed version
if so; otherwise returns "not installed".

*******************************************************************************/
std::string check_installed(const BuildListItem & build)
{
  char buffer[128];
  FILE* fp;
  std::string cmd, version;
  std::stringstream ss;

  cmd = sboutil + " check_installed " + build.name();
  fp = popen(cmd.c_str(), "r");
  while (fgets(buffer, sizeof(buffer), fp) != NULL) { ss << buffer; }
  pclose(fp);

  ss >> version;
  return version;
}
