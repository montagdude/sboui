#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"
#include "Blacklist.h"

extern Blacklist package_blacklist;

int read_repo(std::vector<std::vector<BuildListItem> > & slackbuilds);
template<typename T>
int find_build_in_list(const std::string & name, std::vector<T> & buildlist,
                       int & idx, int & lbound, int & rbound);
int find_slackbuild(const std::string & name,
                    std::vector<std::vector<BuildListItem> > & slackbuilds,
                    int & idx0, int & idx1);
std::vector<std::string> list_installed_packages();
int get_pkg_info(const std::string & pkg, std::string & name,
                 std::string & version, std::string & arch,
                 std::string & build);
bool check_installed(const BuildListItem & build, 
                     const std::vector<std::string> & installedpkgs,
                     std::string & pkg, std::string & version,
                     std::string & arch, std::string & pkgbuild);
int get_reqs(const BuildListItem & build, std::string & reqs);
void get_repo_info(const BuildListItem & build, std::string & available_version,
                   std::string & reqs);
void list_installed(std::vector<std::vector<BuildListItem> > & slackbuilds,
                    std::vector<BuildListItem *> & installedlist);
void list_nondeps(const std::vector<BuildListItem *> & installedlist,
                        std::vector<BuildListItem *> & nondeplist);
int install_slackbuild(const BuildListItem & build);
int upgrade_slackbuild(const BuildListItem & build); 
int remove_slackbuild(const BuildListItem & build);
int view_readme(const BuildListItem & build);
int view_file(const std::string & path);
int sync_repo();
