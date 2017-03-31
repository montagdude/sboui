#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"

int read_repo(std::vector<std::vector<BuildListItem> > & slackbuilds);
std::vector<std::string> list_installed_packages();
void get_pkg_info(const std::string & pkg, std::string & name,
                  std::string & version, std::string & buildnum);
bool check_installed(const BuildListItem & build, 
                     const std::vector<std::string> & installedpkgs,
                     std::string & pkg, std::string & version,
                     std::string & buildnum);
std::string get_reqs(const BuildListItem & build);
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
