#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListBox.h"
#include "string_util.h"
#include "settings.h"   // repo_dir
#include "filters.h"

/*******************************************************************************

Filtering functions that may be passed to filter_by_func

*******************************************************************************/
bool any_build(const BuildListItem & build) { return true; }
bool build_is_installed(const BuildListItem & build)
{
  return build.getBoolProp("installed");
}
bool build_is_upgradable(const BuildListItem & build)
{
  return build.getBoolProp("upgradable");
}
bool build_is_tagged(const BuildListItem & build)
{
  return build.getBoolProp("tagged");
}
bool build_is_blacklisted(const BuildListItem & build)
{
  return build.getBoolProp("blacklisted");
}
bool build_has_buildoptions(const BuildListItem & build)
{
  if (build.getProp("build_options") != "") { return true; }
  else { return false; }
}

/*******************************************************************************

Creates a list of installed packages sorted by name within each category

*******************************************************************************/
std::vector<BuildListItem *> list_installed(
                         std::vector<std::vector<BuildListItem> > & slackbuilds)
{
  std::vector<BuildListItem *> installedlist;
  unsigned int i, j, ncategories, nbuilds;

  ncategories = slackbuilds.size();
  for ( i = 0; i < ncategories; i++ )
  {
    nbuilds = slackbuilds[i].size();
    for ( j = 0; j < nbuilds; j++ )
    {
      if (slackbuilds[i][j].getBoolProp("installed"))
        installedlist.push_back(&slackbuilds[i][j]);
    }
  }

  return installedlist;
} 

/*******************************************************************************

Creates list of installed SlackBuilds that are not required by any other
installed SlackBuild

*******************************************************************************/
std::vector<BuildListItem *> list_nondeps(
                         std::vector<std::vector<BuildListItem> > & slackbuilds)
{
  std::vector<BuildListItem *> installedlist, nondeplist;
  unsigned int i, j, k, ninstalled, ndeps;
  bool isdep;
  std::vector<std::string> deplist;

  installedlist = list_installed(slackbuilds);

  // N^2 (max) loop through installed packages to see which are dependencies

  ninstalled = installedlist.size();
  for ( i = 0; i < ninstalled; i++ )
  {
    isdep = false;
    for ( j = 0; j < ninstalled; j++ )
    {
      if (j == i) { continue; }
      deplist = split(installedlist[j]->getProp("requires"));
      ndeps = deplist.size();
      for ( k = 0; k < ndeps; k++ )
      {
        if (deplist[k] == installedlist[i]->name())
        {
          isdep = true;
          break;
        }
      }
      if (isdep) { break; }
    } 
    if (! isdep) { nondeplist.push_back(installedlist[i]); }
  } 

  return nondeplist;
}

/*******************************************************************************

Filters lists by bool function taking BuildListItem as parameter

*******************************************************************************/
void filter_by_func(std::vector<std::vector<BuildListItem> > & slackbuilds,
                    bool (*func)(const BuildListItem &),
                    std::vector<CategoryListItem> & categories,
                    WINDOW *blistboxwin, CategoryListBox & clistbox,
                    std::vector<BuildListBox> & blistboxes,
                    unsigned int & nfiltered)
{
  unsigned int i, j, ncategories, nbuilds, nfiltered_categories;
  bool category_found;
  BuildListBox initlistbox;

  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  nfiltered = 0; 
  nfiltered_categories = 0;

  for ( i = 0; i < ncategories; i++ )
  {
    category_found = false;
    nbuilds = slackbuilds[i].size();
    for ( j = 0; j < nbuilds; j++ )
    {
      if (func(slackbuilds[i][j]))
      {
        if (! category_found)
        {
          category_found = true;
          clistbox.addItem(&categories[i]);
          BuildListBox blistbox;
          blistbox.setWindow(blistboxwin);
          blistbox.setName(categories[i].name());
          blistbox.setActivated(false);
          nfiltered_categories++;
          blistboxes.push_back(blistbox);
        }
        blistboxes[nfiltered_categories-1].addItem(&slackbuilds[i][j]);
        nfiltered++;
      }
    }
  } 

  // Check whether categories should be tagged

  for ( i = 0; i < nfiltered_categories; i++ )
  {
    if (blistboxes[i].allTagged()) 
      clistbox.itemByIdx(i)->setBoolProp("tagged", true);
    else 
      clistbox.itemByIdx(i)->setBoolProp("tagged", false);
  }

  // Initialize with empty lists if filter is empty

  if (nfiltered == 0)
  {
    initlistbox.setWindow(blistboxwin);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    blistboxes.push_back(initlistbox);
  }
}

/*******************************************************************************

Filters lists by non-dependencies 

*******************************************************************************/
void filter_nondeps(std::vector<std::vector<BuildListItem> > & slackbuilds,
                    std::vector<CategoryListItem> & categories,
                    WINDOW *blistboxwin, CategoryListBox & clistbox,
                    std::vector<BuildListBox> & blistboxes,
                    unsigned int & nnondeps)
{
  unsigned int i, j, ncategories, nfiltered_categories;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;
  std::vector<BuildListItem *> nondeplist;

  nondeplist = list_nondeps(slackbuilds);
  nnondeps = nondeplist.size();
  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  filtered_categories.resize(0);

  for ( i = 0; i < nnondeps; i++ )
  {
    category_found = false;
    nfiltered_categories = filtered_categories.size();
    for ( j = 0; j < nfiltered_categories; j++ )
    {
      if (nondeplist[i]->getProp("category") == filtered_categories[j])
      {
        blistboxes[j].addItem(nondeplist[i]);
        category_found = true;
        break;
      }
    }
    if (! category_found)
    {
      for ( j = 0; j < ncategories; j++ )
      {
        if ( nondeplist[i]->getProp("category") == categories[j].name())
        {
          clistbox.addItem(&categories[j]);
          BuildListBox blistbox;
          blistbox.setWindow(blistboxwin);
          blistbox.setName(categories[j].name());
          blistbox.setActivated(false);
          blistbox.addItem(nondeplist[i]);
          blistboxes.push_back(blistbox);
          filtered_categories.push_back(nondeplist[i]->getProp("category"));
          break;
        }
      }
    }
  }

  // Check whether categories should be tagged

  nfiltered_categories = filtered_categories.size();
  for ( j = 0; j < nfiltered_categories; j++ )
  {
    if (blistboxes[j].allTagged()) 
      clistbox.itemByIdx(j)->setBoolProp("tagged", true);
    else 
      clistbox.itemByIdx(j)->setBoolProp("tagged", false);
  }

  // Initialize with empty lists if filter is empty

  if (nnondeps == 0)
  {
    initlistbox.setWindow(blistboxwin);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    blistboxes.push_back(initlistbox);
  }
}

/*******************************************************************************

Filters lists by search term

*******************************************************************************/
void filter_search(std::vector<std::vector<BuildListItem> > & slackbuilds,
                   std::vector<CategoryListItem> & categories,
                   WINDOW *blistboxwin, CategoryListBox & clistbox,
                   std::vector<BuildListBox> & blistboxes,
                   unsigned int & nsearch, const std::string & searchterm,
                   bool case_sensitive, bool whole_word,
                   bool search_readmes)
{
  unsigned int i, j, nbuilds, ncategories, nsearch_categories;
  std::string term, tomatch, readme_file;
  bool match, category_found;
  BuildListBox initlistbox;

  // For case insensitive search, convert both to lower case

  if (case_sensitive) { term = searchterm; }
  else { term = string_to_lower(searchterm); }

  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  nsearch = 0;
  nsearch_categories = 0;

  for ( i = 0; i < ncategories; i++ )
  {
    category_found = false;
    nbuilds = slackbuilds[i].size();
    for ( j = 0; j < nbuilds; j++ )
    {
      // Check for search term in SlackBuild name

      if (case_sensitive) { tomatch = slackbuilds[i][j].name(); }
      else { tomatch = string_to_lower(slackbuilds[i][j].name()); }
      if (whole_word) { match = (term == tomatch); }
      else { match = (tomatch.find(term) != std::string::npos); }

      // Check for search term in README

      if ( (! match) && (search_readmes) )
      {
        readme_file = settings::repo_dir + "/" + 
                    slackbuilds[i][j].getProp("category") + "/"  +
                    slackbuilds[i][j].name() + "/README";
        match = find_in_file(searchterm, readme_file, whole_word,
                             case_sensitive);
      }

      if (! match) { continue; }

      if (! category_found)
      {
        category_found = true;
        clistbox.addItem(&categories[i]);
        BuildListBox blistbox; 
        blistbox.setWindow(blistboxwin);
        blistbox.setName(categories[i].name());
        blistbox.setActivated(false);
        nsearch_categories++;
        blistboxes.push_back(blistbox);
      }
      blistboxes[nsearch_categories-1].addItem(&slackbuilds[i][j]);
      nsearch++;
    } 
  }

  // Check whether categories should be tagged

  for ( i = 0; i < nsearch_categories; i++ )
  {
    if (blistboxes[i].allTagged()) 
      clistbox.itemByIdx(i)->setBoolProp("tagged", true);
    else 
      clistbox.itemByIdx(i)->setBoolProp("tagged", false);
  }

  // Initialize with empty lists if filter is empty

  if (nsearch == 0)
  {
    initlistbox.setWindow(blistboxwin);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    blistboxes.push_back(initlistbox);
  }
}
