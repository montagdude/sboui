#include <string>
#include <vector>
#include "DirListing.h"
#include "BuildListItem.h"

std::string name_order = "0123456789" +
                         std::string(" !@#$%^&*()+-=_~`[]\\;',/{}|:\"<>?") +
                         std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ") +
                         std::string("abcdefghijklmnopqrstuvwxyz") +
                         std::string(".");
std::vector<std::string> type_order(9,"");

/*******************************************************************************

Compares two direntries by name. Returns the argument of the one that comes
first in the sort order, or -1 if they are the same.

*******************************************************************************/
int compare_by_name(const direntry & entry1, const direntry & entry2)
{ 
  unsigned int i, len, rank1, rank2;
  char c1, c2;
  
  if (entry1.name == entry2.name) { return -1; }
  
  // Loop through each string until a difference is found
  
  len = std::min(entry1.name.size(), entry2.name.size());
  for ( i = 0; i < len; i++ )
  { 
    c1 = entry1.name[i];
    rank1 = name_order.find(c1);
    if (rank1 == std::string::npos) { rank1 = 95; }
    c2 = entry2.name[i];
    rank2 = name_order.find(c2);
    if (rank2 == std::string::npos) { rank2 = 95; }
    if (rank1 < rank2) { return 0; } 
    else if (rank1 > rank2) { return 1; }
  }
  
  // If we got this far, that means the the two strings are the same up until
  // the length of the shorter one, but the lengths are not equal. The shorter
  // one is ranked first.
  
  if (entry1.name.size() < entry2.name.size()) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Compares two direntries by type. Returns the argument of the one that comes
first in the sort order, or -1 if they are the same.

*******************************************************************************/
int compare_by_type(const direntry & entry1, const direntry & entry2)
{
  unsigned int i;
  int rank1, rank2;

  if (type_order[0] == "")
  {
    type_order[0] = "dir";
    type_order[1] = "reg";
    type_order[2] = "lnk";
    type_order[3] = "fifo";
    type_order[4] = "chr";
    type_order[5] = "blk";
    type_order[6] = "sock";
    type_order[7] = "wht";
    type_order[8] = "unknown";
  }

  if (entry1.type == entry2.type) { return -1; }

  // Get position of each type in type_order vector

  rank1 = -1;
  for ( i = 0; i < type_order.size(); i++ )
  {
    if (entry1.type == type_order[i])
    {
      rank1 = i;
      break;
    }
  }

  rank2 = -1;
  for ( i = 0; i < type_order.size(); i++ )
  {
    if (entry2.type == type_order[i])
    {
      rank2 = i;
      break;
    }
  }

  // Handle edge cases (that shouldn't ever occur, actually)

  if (rank1 == -1)
  {
    if (rank2 == -1) { return -1; }
    else { return 1; }
  }
  if (rank2 == -1) { return 0; }

  // Decide which comes first in normal circumstances

  if (rank1 < rank2) { return 0; }
  else { return 1; }
};

/*******************************************************************************

Bubble sorting algorithm for directory listings

*******************************************************************************/
void sort_direntries(std::vector<direntry> & entries, int (*comparison_func)(
                     const direntry & entry1, const direntry & entry2))
{
  std::vector<direntry> tempentries;
  bool sorted;
  int swapcount, i, nentries;

  // Bubble sort: loop through the list repeatedly until no pairs need to be
  // swapped

  sorted = false;
  tempentries = entries;
  nentries = entries.size();
  while (! sorted)
  {
    swapcount = 0;
    for ( i = 0; i < nentries-1; i++ )
    {
      if (comparison_func(entries[i+1], entries[i]) == 0)
      {
        // Flip order. temp vector preserves values.

        tempentries[i] = entries[i+1];
        tempentries[i+1] = entries[i];
        entries[i] = tempentries[i];
        entries[i+1] = tempentries[i+1];
        swapcount += 1;
      }
    }

    if (swapcount == 0) { sorted = true; }
  }
}

/*******************************************************************************

Compares entries by given string property. Returns the argument of the one that
comes first in the sort order, or -1 if they are the same.

*******************************************************************************/
int compare_by_prop(const BuildListItem *item1, const BuildListItem *item2,
                    const std::string & prop)
{
  unsigned int i, len, rank1, rank2;
  char c1, c2;

  if (item1->getProp(prop) == item2->getProp(prop)) { return -1; }

  // Loop through each string until a difference is found

  len = std::min(item1->getProp(prop).size(), item2->getProp(prop).size());
  for ( i = 0; i < len; i++ )
  {
    c1 = item1->getProp(prop)[i];
    rank1 = name_order.find(c1);
    if (rank1 == std::string::npos) { rank1 = 95; }
    c2 = item2->getProp(prop)[i];
    rank2 = name_order.find(c2);
    if (rank2 == std::string::npos) { rank2 = 95; }
    if (rank1 < rank2) { return 0; }
    else if (rank1 > rank2) { return 1; }
  }

  // If we got this far, that means the the two strings are the same up until
  // the length of the shorter one, but the lengths are not equal. The shorter
  // one is ranked first.

  if (item1->getProp(prop).size() < item2->getProp(prop).size()) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Bubble sorting algorithm for SlackBuild lists

*******************************************************************************/
void sort_list(std::vector<BuildListItem *> & list, const std::string & prop,
               int (*comparison_func)(const BuildListItem *item1, 
               const BuildListItem *item2, const std::string & prop))
{
  std::vector<BuildListItem *> templist;
  bool sorted;
  int swapcount, i, nitems;

  // Bubble sort: loop through the list repeatedly until no pairs need to be
  // swapped

  sorted = false;
  templist = list;
  nitems = list.size();
  while (! sorted)
  {
    swapcount = 0;
    for ( i = 0; i < nitems-1; i++ )
    {
      if (comparison_func(list[i+1], list[i], prop) == 0)
      {
        // Flip order. temp vector preserves values.

        templist[i] = list[i+1];
        templist[i+1] = list[i];
        list[i] = templist[i];
        list[i+1] = templist[i+1];
        swapcount += 1;
      }
    }

    if (swapcount == 0) { sorted = true; }
  }
}
