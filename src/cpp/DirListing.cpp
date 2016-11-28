#include <algorithm>  // std::min
#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include "DirListing.h"

#ifdef MINGW
  std::string DirListing::separator = "\\";
#else
  std::string DirListing::separator = "/";
#endif

std::string DirListing::name_order = "0123456789" + 
                         std::string(" !@#$%^&*()+-=_~`[]\\;',/{}|:\"<>?") +
                         std::string("ABCDEFGHIJKLMNOPQRSTUVWXYZ") +
                         std::string("abcdefghijklmnopqrstuvwxyz") + 
                         std::string(".");
std::vector<std::string> DirListing::type_order(9,"");

/*******************************************************************************

Gets name from dirent and returns as string

*******************************************************************************/
std::string DirListing::nameFromDirent(dirent *pent) const
{
  std::stringstream ss;

  ss << pent->d_name;
  return ss.str();
}

/*******************************************************************************

Gets type from dirent and returns as string

*******************************************************************************/
std::string DirListing::typeFromDirent(dirent *pent) const
{
  switch (pent->d_type) {
    case DT_UNKNOWN:
      return "unknown";
      break;
    case DT_FIFO:
      return "fifo";
      break;
    case DT_CHR:
      return "chr";
      break;
    case DT_DIR:
      return "dir";
      break;
    case DT_BLK:
      return "blk";
      break;
    case DT_REG:
      return "reg";
      break;
    case DT_LNK:
      return "lnk";
      break;
    case DT_SOCK:
      return "sock";
      break;
#ifndef MINGW
    case DT_WHT:           // Not defined in MinGW
      return "wht";
      break;
#endif
    default:
      return "unknown";
      break;
  }
};

/*******************************************************************************

Compares two direntries by name. Returns the argument of the one that comes
first in the sort order, or -1 if they are the same.

*******************************************************************************/
int DirListing::compareByName(const direntry & entry1, const direntry & entry2)
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
int DirListing::compareByType(const direntry & entry1, const direntry & entry2)
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

Bubble sorting algorithm

*******************************************************************************/
void DirListing::bubbleSort(int (*comparison_func)(const direntry & entry1,
                                                   const direntry & entry2))
{
  std::vector<direntry> tempentries;
  bool sorted;
  int swapcount, i;

  // Bubble sort: loop through the list repeatedly until no pairs need to be
  // swapped

  sorted = false;
  tempentries = _entries;
  while (! sorted)
  {
    swapcount = 0;
    for ( i = 0; i < int(_entries.size())-1; i++ )
    {
      if (comparison_func(_entries[i+1], _entries[i]) == 0)
      {
        // Flip order. temp vector preserves values.

        tempentries[i] = _entries[i+1];
        tempentries[i+1] = _entries[i];
        _entries[i] = tempentries[i];
        _entries[i+1] = tempentries[i+1];
        swapcount += 1;
      }
    }

    if (swapcount == 0) { sorted = true; }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
DirListing::DirListing()
{
  _entries.resize(0);
  _path = "";
  setFromCwd();
}

DirListing::DirListing(bool show_hidden)
{
  _entries.resize(0);
  _path = "";
  setFromCwd(show_hidden);
}

DirListing::DirListing(const std::string & path)
{
  _entries.resize(0);
  _path = path;
  if (setFromPath(path) == 1) { _path = ""; }
}

DirListing::DirListing(const std::string & path, bool show_hidden)
{
  _entries.resize(0);
  _path = path;
  if (setFromPath(path, show_hidden) == 1) { _path = ""; }
}

/*******************************************************************************

Creates listing from path, optionally showing or hiding hidden entries

*******************************************************************************/
int DirListing::setFromPath(const std::string & path, bool show_hidden)
{
  DIR *pdir = NULL;
  struct dirent *pent = NULL;
  direntry entry;
  std::size_t len;
  std::string temppath;

  _entries.resize(0);

  // Try to open directory

  pdir = opendir(path.c_str());
  if (pdir == NULL) { return 1; }

  // Make sure path ends in separator

  len = path.size();
  if (path[len-1] == separator[0]) { temppath = path; }
  else { temppath = path + separator; }

  // Read contents

  while ((pent = readdir(pdir)))
  {
    if (pent == NULL) { return 1; }
    entry.name = nameFromDirent(pent);
    entry.type = typeFromDirent(pent);
    entry.path = temppath;
    if (entry.type == "unknown") { continue; }
    if ( (entry.name == ".") || (entry.name == "..") ) { continue; }
    if (! show_hidden) 
    {
      if (entry.name[0] == '.') { continue; }
    }
    _entries.push_back(entry);
  } 

  // Close directory

  closedir(pdir);
  _path = temppath; // Directory path

  // Sort entries

  sort();
  
  return 0;
} 

/*******************************************************************************

Creates listing from current working directory, optionally showing or hiding
hidden entries

*******************************************************************************/
int DirListing::setFromCwd(bool show_hidden)
{
  char cpath[1024];   // Hopefully there won't be any path longer than this
  DIR *pdir = NULL;
  struct dirent *pent = NULL;
  direntry entry;
  std::size_t len;
  std::string temppath;

  _entries.resize(0);

  // Try to open directory

  pdir = opendir(getcwd(cpath, sizeof(cpath)));
  if (pdir == NULL) { return 1; }

  // Make sure path ends with separator

  temppath = std::string(cpath);
  len = temppath.size();
  if (temppath[len-1] != separator[0]) { temppath = temppath + separator; }

  // Read contents

  while ((pent = readdir(pdir)))
  {
    if (pent == NULL) { return 1; }
    entry.name = nameFromDirent(pent);
    entry.type = typeFromDirent(pent);
    entry.path = temppath;
    if (entry.type == "unknown") { continue; }
    if ( (entry.name == ".") || (entry.name == "..") ) { continue; }
    if (! show_hidden)
    {
      if (entry.name[0] == '.') { continue; }
    }
    _entries.push_back(entry);
  } 

  // Close directory

  closedir(pdir);
  _path = temppath; // Directory path

  // Sort entries

  sort();
  
  return 0;
} 

/*******************************************************************************

Navigates up from currently set directory, optionally showing or hiding hidden
entries.

*******************************************************************************/
int DirListing::navigateUp(bool show_hidden)
{
  std::size_t lastsep, firstsep;
  std::string newpath;

  lastsep = _path.find_last_of(separator);
  firstsep = _path.find_first_of(separator);

  // Root directory handling

  if (lastsep == firstsep)
  {
    newpath = _path;
    return 1;
  }
  else
  {
    if (lastsep == std::string::npos) 
    { 
      newpath = ""; 
      return 1;
    }
    else
    {
      // Parse one directory up

      newpath = _path.substr(0,lastsep);
      lastsep = newpath.find_last_of(separator);
      newpath = _path.substr(0, lastsep+1);
    }
  }

  // Try to use new directory, or revert to the old one in case of failure

  if (setFromPath(newpath, show_hidden) == 0) { return 0; }
  else 
  { 
    return setFromPath(_path, show_hidden);
    return 1;
  }
}

/*******************************************************************************

Sorts entries by name and then by type 

*******************************************************************************/
void DirListing::sort()
{
  bubbleSort(compareByName);
  bubbleSort(compareByType);
}

/*******************************************************************************

Returns direntry by index

*******************************************************************************/
const direntry DirListing::operator () (unsigned int idx) const
{
  direntry defaultentry;

  defaultentry.name = "";
  defaultentry.type = "";
  defaultentry.path = "";
  if (idx < _entries.size()) { return _entries[idx]; }
  else { return defaultentry; }
}

/*******************************************************************************

Returns direntry by name

*******************************************************************************/
const direntry DirListing::operator () (const std::string & name) const
{
  direntry defaultentry;
  unsigned int i;

  defaultentry.name = "";
  defaultentry.type = "";
  defaultentry.path = "";
  
  for ( i = 0; i < _entries.size(); i++ )
  {
    if (_entries[i].name == name) { return _entries[i]; }
  }
 return defaultentry;
}

/*******************************************************************************

Returns path

*******************************************************************************/
const std::string & DirListing::path() const { return _path; }

/*******************************************************************************

Returns number of entries

*******************************************************************************/
unsigned int DirListing::size() const { return _entries.size(); }
