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

Filters lists by all SlackBuilds

*******************************************************************************/
void filter_all(std::vector<std::vector<BuildListItem> > & slackbuilds,
                std::vector<CategoryListItem> & categories,
                WINDOW *blistboxwin, CategoryListBox & clistbox,
                std::vector<BuildListBox> & blistboxes)
{
  unsigned int i, j, nbuilds, ncategories, totalbuilds;
  BuildListBox initlistbox;

  totalbuilds = 0;
  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  for ( i = 0; i < ncategories; i++ )
  {
    clistbox.addItem(&categories[i]);
    BuildListBox blistbox;
    blistbox.setWindow(blistboxwin);
    blistbox.setName(categories[i].name());
    blistbox.setActivated(false); 
    nbuilds = slackbuilds[i].size();
    totalbuilds += nbuilds;
    for ( j = 0; j < nbuilds; j++ ) { blistbox.addItem(&slackbuilds[i][j]); }
    blistboxes.push_back(blistbox);
  }

  // Check whether categories should be tagged

  for ( i = 0; i < ncategories; i++ )
  {
    if (blistboxes[i].allTagged())
      clistbox.itemByIdx(i)->setBoolProp("tagged", true);
    else
      clistbox.itemByIdx(i)->setBoolProp("tagged", false);
  }

  // Initialize with empty lists if repo is empty

  if (totalbuilds == 0)
  {
    initlistbox.setWindow(blistboxwin);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    blistboxes.push_back(initlistbox);
  }
} 

/*******************************************************************************

Filters lists by installed SlackBuilds

*******************************************************************************/
void filter_installed(std::vector<BuildListItem *> & installedlist,
                      std::vector<CategoryListItem> & categories,
                      WINDOW *blistboxwin, CategoryListBox & clistbox,
                      std::vector<BuildListBox> & blistboxes)
{
  unsigned int i, j, ncategories, nfiltered_categories, ninstalled;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  ninstalled = installedlist.size();
  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  filtered_categories.resize(0);

  for ( i = 0; i < ninstalled; i++ )
  {
    category_found = false;
    nfiltered_categories = filtered_categories.size();
    for ( j = 0; j < nfiltered_categories; j++ )
    {
      if (installedlist[i]->getProp("category") == filtered_categories[j])
      {
        blistboxes[j].addItem(installedlist[i]);
        category_found = true;
        break;
      }
    }
    if (! category_found)
    {
      for ( j = 0; j < ncategories; j++ )
      {
        if (installedlist[i]->getProp("category") == categories[j].name())
        {
          clistbox.addItem(&categories[j]);
          BuildListBox blistbox;
          blistbox.setWindow(blistboxwin);
          blistbox.setName(categories[j].name());
          blistbox.setActivated(false);
          blistbox.addItem(installedlist[i]);
          blistboxes.push_back(blistbox);
          filtered_categories.push_back(installedlist[i]->getProp("category"));
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

  if (ninstalled == 0)
  {
    initlistbox.setWindow(blistboxwin);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    blistboxes.push_back(initlistbox);
  }
}

/*******************************************************************************

Filters lists by upgradable SlackBuilds

*******************************************************************************/
void filter_upgradable(std::vector<BuildListItem *> & installedlist,
                       std::vector<CategoryListItem> & categories,
                       WINDOW *blistboxwin, CategoryListBox & clistbox,
                       std::vector<BuildListBox> & blistboxes,
                       unsigned int & nupgradable)
{
  unsigned int i, j, ncategories, nfiltered_categories, ninstalled;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  ninstalled = installedlist.size();
  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  filtered_categories.resize(0);
  nupgradable = 0;

  for ( i = 0; i < ninstalled; i++ )
  {
    if (installedlist[i]->upgradable())
    {
      category_found = false;
      nfiltered_categories = filtered_categories.size();
      for ( j = 0; j < nfiltered_categories; j++ )
      {
        if (installedlist[i]->getProp("category") == filtered_categories[j])
        {
          blistboxes[j].addItem(installedlist[i]);
          category_found = true;
          break;
        }
      }
      if (! category_found)
      {
        for ( j = 0; j < ncategories; j++ )
        {
          if ( installedlist[i]->getProp("category") == categories[j].name())
          {
            clistbox.addItem(&categories[j]);
            BuildListBox blistbox;
            blistbox.setWindow(blistboxwin);
            blistbox.setName(categories[j].name());
            blistbox.setActivated(false);
            blistbox.addItem(installedlist[i]);
            blistboxes.push_back(blistbox);
            filtered_categories.push_back(
                                         installedlist[i]->getProp("category"));
            break;
          }
        }
      }
      nupgradable++;
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

  if (nupgradable == 0)
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
void filter_nondeps(std::vector<BuildListItem *> & nondeplist,
                    std::vector<CategoryListItem> & categories,
                    WINDOW *blistboxwin, CategoryListBox & clistbox,
                    std::vector<BuildListBox> & blistboxes)
{
  unsigned int i, j, ncategories, nfiltered_categories, nnondeps;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

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

/*******************************************************************************

Filters lists by BoolProp

*******************************************************************************/
void filter_by_prop(std::vector<std::vector<BuildListItem> > & slackbuilds,
                    const std::string & propname,
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
      if (slackbuilds[i][j].getBoolProp(propname))
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
