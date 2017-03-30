#include <algorithm>  // std::min
#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include "sorting.h"
#include "DirListing.h"

#ifdef MINGW
  std::string DirListing::separator = "\\";
#else
  std::string DirListing::separator = "/";
#endif

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

Constructors

*******************************************************************************/
DirListing::DirListing()
{
  _entries.resize(0);
  _path = "";
  setFromCwd();
}

DirListing::DirListing(bool sort_listing)
{
  _entries.resize(0);
  _path = "";
  setFromCwd(sort_listing);
}

DirListing::DirListing(bool sort_listing, bool show_hidden)
{
  _entries.resize(0);
  _path = "";
  setFromCwd(sort_listing, show_hidden);
}

DirListing::DirListing(const std::string & path)
{
  _entries.resize(0);
  _path = path;
  if (setFromPath(path) == 1) { _path = ""; }
}

DirListing::DirListing(const std::string & path, bool sort_listing)
{
  _entries.resize(0);
  _path = path;
  if (setFromPath(path, sort_listing) == 1) { _path = ""; }
}

DirListing::DirListing(const std::string & path, bool sort_listing,
                       bool show_hidden)
{
  _entries.resize(0);
  _path = path;
  if (setFromPath(path, sort_listing, show_hidden) == 1) { _path = ""; }
}

/*******************************************************************************

Creates listing from path, optionally showing or hiding hidden entries

*******************************************************************************/
int DirListing::setFromPath(const std::string & path, bool sort_listing,
                            bool show_hidden)
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

  if (sort_listing) { sort(); }
  
  return 0;
} 

/*******************************************************************************

Creates listing from current working directory, optionally showing or hiding
hidden entries

*******************************************************************************/
int DirListing::setFromCwd(bool sort_listing, bool show_hidden)
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

  if (sort_listing) { sort(); }
  
  return 0;
} 

/*******************************************************************************

Navigates up from currently set directory, optionally showing or hiding hidden
entries.

*******************************************************************************/
int DirListing::navigateUp(bool sort_listing, bool show_hidden)
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

  if (setFromPath(newpath, sort_listing, show_hidden) == 0) { return 0; }
  else { return setFromPath(_path, sort_listing, show_hidden); }
}

/*******************************************************************************

Sorts entries by name and then by type 

*******************************************************************************/
void DirListing::sort()
{
  sort_direntries(_entries, compare_by_name);
  sort_direntries(_entries, compare_by_type);
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
