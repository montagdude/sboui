#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"

/* Backend settings */
//FIXME: put most of these in a separate settings module/class?

extern std::string repo_dir;
extern std::string package_manager;
extern std::string sync_cmd, install_cmd, upgrade_cmd;
extern std::string install_vars, install_opts;
extern std::string upgrade_vars, upgrade_opts;
extern std::string editor;

/* Backend operations */

int read_repo(std::vector<BuildListItem> & slackbuilds);
std::string get_reqs(const BuildListItem & build);
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist);
void list_nondeps(const std::vector<BuildListItem *> & installedlist,
                        std::vector<BuildListItem *> & nondeplist);
int install_slackbuild(const BuildListItem & build);
int upgrade_slackbuild(const BuildListItem & build); 
int remove_slackbuild(const BuildListItem & build);
int view_readme(const BuildListItem & build);
int view_file(const std::string & path);
int sync();
