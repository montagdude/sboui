#pragma once

#include <string>
#include <vector>
#include "BuildListItem.h"
#include "DirListing.h"

// Sorting directory listings

int compare_by_name(const direntry & entry1, const direntry & entry2);
int compare_by_type(const direntry & entry1, const direntry & entry2);
void sort_direntries(std::vector<direntry> & entries, int (*comparison_func)(
                     const direntry & entry1, const direntry & entry2));

// Sorting lists

int compare_by_prop(const BuildListItem *item1, const BuildListItem *item2,
                    const std::string & prop);
void sort_list(std::vector<BuildListItem *> & list, const std::string & prop,
               int (*comparison_func)(const BuildListItem *item1, 
               const BuildListItem *item2, const std::string & prop));
