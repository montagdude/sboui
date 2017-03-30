#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"

int read_repo(std::vector<BuildListItem> & slackbuilds);
int get_installed_info(const BuildListItem & build, std::string & version,
                       std::string & name, bool & foreign);
std::string get_reqs(const BuildListItem & build);
void get_repo_info(const BuildListItem & build, std::string & available_version,
                   std::string & reqs);
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist);
void list_nondeps(const std::vector<BuildListItem *> & installedlist,
                        std::vector<BuildListItem *> & nondeplist);
int install_slackbuild(const BuildListItem & build);
int upgrade_slackbuild(const BuildListItem & build); 
int remove_slackbuild(const BuildListItem & build);
int view_readme(const BuildListItem & build);
int view_file(const std::string & path);
int sync_repo();
