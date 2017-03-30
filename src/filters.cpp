#include <vector>
#include <string>
#include <curses.h>
#include "BuildListItem.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListBox.h"
#include "string_util.h"

/*******************************************************************************

Filters lists by all SlackBuilds

*******************************************************************************/
void filter_all(std::vector<BuildListItem> & slackbuilds,
                std::vector<CategoryListItem> & categories,
                WINDOW *blistboxwin, CategoryListBox & clistbox,
                std::vector<BuildListBox> & blistboxes)
{
  unsigned int i, j, nbuilds, ncategories;
  BuildListBox initlistbox;

  nbuilds = slackbuilds.size();
  ncategories = categories.size();

  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  for ( j = 0; j < ncategories; j++ )
  {
    clistbox.addItem(&categories[j]);
    BuildListBox blistbox;
    blistbox.setWindow(blistboxwin);
    blistbox.setName(categories[j].name());
    blistbox.setActivated(false); 
    blistboxes.push_back(blistbox);
  }
  for ( i = 0; i < nbuilds; i++ )
  {
    for ( j = 0; j < ncategories; j++ )
    {
      if (slackbuilds[i].getProp("category") == categories[j].name())
        blistboxes[j].addItem(&slackbuilds[i]);
    }
  }

  // Check wheteher categories should be tagged

  for ( j = 0; j < ncategories; j++ )
  {
    if (blistboxes[j].allTagged())
      clistbox.itemByIdx(j)->setBoolProp("tagged", true);
    else
      clistbox.itemByIdx(j)->setBoolProp("tagged", false);
  }

  // Initialize with empty lists if repo is empty

  if (nbuilds == 0)
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

Filters lists by tagged SlackBuilds

*******************************************************************************/
void filter_tagged(std::vector<BuildListItem> & slackbuilds,
                   std::vector<CategoryListItem> & categories,
                   WINDOW *blistboxwin, CategoryListBox & clistbox,
                   std::vector<BuildListBox> & blistboxes,
                   unsigned int & ntagged)
{
  unsigned int i, j, ncategories, nfiltered_categories, nbuilds;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  nbuilds = slackbuilds.size();
  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  filtered_categories.resize(0);
  ntagged = 0; 

  for ( i = 0; i < nbuilds; i++ )
  {
    if (slackbuilds[i].getBoolProp("tagged"))
    {
      category_found = false;
      nfiltered_categories = filtered_categories.size();
      for ( j = 0; j < nfiltered_categories; j++ )
      {
        if (slackbuilds[i].getProp("category") == filtered_categories[j])
        {
          blistboxes[j].addItem(&slackbuilds[i]);
          category_found = true;
          break;
        }
      }
      if (! category_found)
      {
        for ( j = 0; j < ncategories; j++ )
        {
          if ( slackbuilds[i].getProp("category") == categories[j].name())
          {
            clistbox.addItem(&categories[j]);
            BuildListBox blistbox;
            blistbox.setWindow(blistboxwin);
            blistbox.setName(categories[j].name());
            blistbox.setActivated(false);
            blistbox.addItem(&slackbuilds[i]);
            blistboxes.push_back(blistbox);
            filtered_categories.push_back(slackbuilds[i].getProp("category"));
            break;
          }
        }
      }
      ntagged++;
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

  if (ntagged == 0)
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
void filter_search(std::vector<BuildListItem> & slackbuilds,
                   std::vector<CategoryListItem> & categories,
                   WINDOW *blistboxwin, CategoryListBox & clistbox,
                   std::vector<BuildListBox> & blistboxes,
                   unsigned int & nsearch, const std::string & searchterm,
                   bool case_sensitive, bool whole_word)
{
  unsigned int i, j, nbuilds, ncategories, nsearch_categories;
  std::vector<std::string> search_categories;
  std::string term, tomatch;
  bool match, category_found;
  BuildListBox initlistbox;

  // For case insensitive search, convert both to lower case

  if (case_sensitive) { term = searchterm; }
  else { term = string_to_lower(searchterm); }

  nbuilds = slackbuilds.size();
  ncategories = categories.size();
  blistboxes.resize(0);
  clistbox.clearList();
  clistbox.setActivated(true);
  search_categories.resize(0);
  nsearch = 0;

  for ( i = 0; i < nbuilds; i++ )
  {
    category_found = false;

    // Check for search term in SlackBuild name

    if (case_sensitive) { tomatch = slackbuilds[i].name(); }
    else { tomatch = string_to_lower(slackbuilds[i].name()); }
    if (whole_word) { match = (term == tomatch); }
    else { match = (tomatch.find(term) != std::string::npos); }
    if (! match) { continue; }

    nsearch++;
    nsearch_categories = search_categories.size();
    for ( j = 0; j < nsearch_categories; j++ )
    {
      if (slackbuilds[i].getProp("category") == search_categories[j])
      {
        blistboxes[j].addItem(&slackbuilds[i]);
        category_found = true;
        break;
      }
    }
    if (! category_found)
    {
      for ( j = 0; j < ncategories; j++ )
      {
        if (slackbuilds[i].getProp("category") == categories[j].name())
        {
          clistbox.addItem(&categories[j]);
          BuildListBox blistbox;
          blistbox.setWindow(blistboxwin);
          blistbox.setName(categories[j].name());
          blistbox.setActivated(false);
          blistbox.addItem(&slackbuilds[i]);
          blistboxes.push_back(blistbox);
          search_categories.push_back(slackbuilds[i].getProp("category"));
          break;
        }
      }
    }
  }

  // Check whether categories should be tagged

  nsearch_categories = search_categories.size();
  for ( j = 0; j < nsearch_categories; j++ )
  {
    if (blistboxes[j].allTagged()) 
      clistbox.itemByIdx(j)->setBoolProp("tagged", true);
    else 
      clistbox.itemByIdx(j)->setBoolProp("tagged", false);
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
