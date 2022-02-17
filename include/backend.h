#pragma once

#include <string>
#include <vector>
#include <cmath>   // floor
#include "BuildListItem.h"
#include "Blacklist.h"

extern Blacklist blacklist;

int read_repo(std::vector<std::vector<BuildListItem> > & slackbuilds);
int read_buildopts(std::vector<std::vector<BuildListItem> > & slackbuilds);
int find_slackbuild(const std::string & name,
                    std::vector<std::vector<BuildListItem> > & slackbuilds,
                    int & idx0, int & idx1);
int get_pkg_info(const std::string & pkg, std::string & name,
                 std::string & version, std::string & arch,
                 std::string & build);
bool check_installed(const BuildListItem & build, 
                     const std::vector<std::string> & installedpkgs,
                     std::string & pkg, std::string & version,
                     std::string & arch, std::string & pkgbuild);
int get_reqs(const BuildListItem & build, std::string & reqs);
int get_repo_info(const BuildListItem & build, std::string & available_version,
                  std::string & reqs, std::string & available_buildnum);
void determine_installed(std::vector<std::vector<BuildListItem> > & slackbuilds,
                         std::vector<std::string> & pkg_errors,
                         std::vector<std::string> & missing_info);
std::vector<BuildListItem *> list_installed(
                        std::vector<std::vector<BuildListItem> > & slackbuilds);
std::vector<BuildListItem *> list_nondeps(
                        std::vector<std::vector<BuildListItem> > & slackbuilds);
int install_slackbuild(BuildListItem & build);
int upgrade_slackbuild(BuildListItem & build); 
int reinstall_slackbuild(BuildListItem & build); 
int remove_slackbuild(BuildListItem & build);
int view_readme(const BuildListItem & build);
int view_file(const std::string & path);
int view_notes(const BuildListItem & build);
int sync_repo(bool interactive=true);

/*******************************************************************************

Overloaded template functions to turn reference into pointer
http://stackoverflow.com/questions/14466620/c-template-specialization- \
calling-methods-on-types-that-could-be-pointers-or#14466705

*******************************************************************************/
template<typename T>
T * ptr(T & obj) { return & obj; }

template<typename T>
T * ptr(T * obj) { return obj; }

/*******************************************************************************

Finds an item by name in a sorted list. If whole_word is set to false, it
will only search the first N characters of items in the list, where N is the
length of the input name. Returns 0 if found, 1 if not found.

Note: the implementation is placed in the header file to avoid the need for
explicit instantiation of the template.

*******************************************************************************/
template<typename T>
int find_name_in_list(const std::string & name, std::vector<T> & list,
                      int & idx, int & lbound, int & rbound,
                      bool whole_word=true)
{
  int midbound;
  unsigned int namelen;
  std::string leftcheck, rightcheck, midcheck;

  if (name == "") { return 1; }

  if (whole_word)
  {
    leftcheck = ptr(list[lbound])->name();
    rightcheck = ptr(list[rbound])->name();
  }
  else
  {
    namelen = name.size();
    leftcheck = ptr(list[lbound])->name().substr(0,namelen);
    rightcheck = ptr(list[rbound])->name().substr(0,namelen);
  }

  // Check if outside the bounds

  if ( (name < leftcheck) || (name > rightcheck) ) { return 1; }

  // Check bounds for match

  if (name == leftcheck)
  {
    if ( (whole_word) || (rbound-lbound == 1) )
    {
      idx = lbound;
      return 0;
    }
  }
  else if (name == rightcheck)
  {
    if ( (whole_word) || (rbound-lbound == 1) )
    {
      idx = rbound;
      return 0;
    }
  }
  else { if (rbound-lbound == 1) { return 1; } }
  
  // Cut the list in half and try again

  midbound = std::floor(double(lbound+rbound)/2.);
  if (whole_word) { midcheck = ptr(list[midbound])->name(); }
  else { midcheck = ptr(list[midbound])->name().substr(0,namelen); }

  if (name <= midcheck) { rbound = midbound; }
  else { lbound = midbound; }

  return find_name_in_list(name, list, idx, lbound, rbound, whole_word);
}
