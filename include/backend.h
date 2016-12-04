#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"
#include "ListItem.h"

/* Backend settings */

extern std::string repo_dir;
extern std::string package_manager;
extern std::string sync_cmd, install_cmd, upgrade_cmd;

/* Backend operations */

int read_repo(std::vector<BuildListItem> & slackbuilds);
void list_installed(std::vector<BuildListItem> & slackbuilds,
                    std::vector<BuildListItem *> & installedlist);
void list_nondeps(const std::vector<BuildListItem *> & installedlist,
                        std::vector<BuildListItem *> & nondeplist);
void install_slackbuilds(std::vector<BuildListItem> & slackbuilds, 
                  const std::string & opts = "", const std::string & vars = "");
void upgrade_slackbuilds(std::vector<BuildListItem> & slackbuilds, 
                  const std::string & opts = "", const std::string & vars = "");
void remove_slackbuilds(std::vector<BuildListItem> & slackbuilds);
