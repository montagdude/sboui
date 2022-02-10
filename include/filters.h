#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "BuildListItem.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListBox.h"

bool any_build(const BuildListItem & build);
bool build_is_installed(const BuildListItem & build);
bool build_is_upgradable(const BuildListItem & build);
bool build_is_tagged(const BuildListItem & build);
bool build_is_blacklisted(const BuildListItem & build);
bool build_has_buildoptions(const BuildListItem & build);

void filter_by_func(std::vector<std::vector<BuildListItem *> > & slackbuilds,
                    bool (*func)(const BuildListItem &),
                    std::vector<CategoryListItem *> & categories,
                    WINDOW *blistboxwin, CategoryListBox & clistbox,
                    std::vector<BuildListBox> & blistboxes,
                    unsigned int & nfiltered, bool overwrite=true);
void filter_nondeps(std::vector<std::vector<BuildListItem> > & all_slackbuilds,
                    std::vector<std::vector<BuildListItem *> > & slackbuilds,
                    std::vector<CategoryListItem *> & categories,
                    WINDOW *blistboxwin, CategoryListBox & clistbox,
                    std::vector<BuildListBox> & blistboxes,
                    unsigned int & nnondeps, bool overwrite=true);
void filter_search(std::vector<std::vector<BuildListItem *> > & slackbuilds,
                   std::vector<CategoryListItem *> & categories,
                   WINDOW *blistboxwin, CategoryListBox & clistbox,
                   std::vector<BuildListBox> & blistboxes,
                   unsigned int & nsearch, const std::string & searchterm,
                   bool case_sensitive, bool whole_word, bool search_readmes,
                   bool overwrite=true);
