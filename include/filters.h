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
