#pragma once

#include <vector>
#include <curses.h>
#include "BuildListItem.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListBox.h"

void filter_all(std::vector<BuildListItem> & slackbuilds,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes);
void filter_installed(std::vector<BuildListItem *> & installedlist,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes);
void filter_upgradable(std::vector<BuildListItem *> & installedlist,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes,
                       unsigned int & nupgradable);
void filter_tagged(std::vector<BuildListItem> & slackbuilds,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes,
                       unsigned int & ntagged);
void filter_nondeps(std::vector<BuildListItem *> & nondeplist,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes);
void filter_search(std::vector<BuildListItem> & slackbuilds,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes,
                       unsigned int & nsearch, const std::string & searchterm,
                       bool case_sensitive, bool whole_word);
