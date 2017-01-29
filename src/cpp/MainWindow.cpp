#include <vector>
#include <string>
#include <cmath>     // floor
#include "curses.h"
#include "Color.h"
#include "color_settings.h"
#include "string_util.h"
#include "signals.h"
#include "backend.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "FilterBox.h"
#include "SearchBox.h"
#include "BuildActionBox.h"
#include "BuildOrderBox.h"
#include "InvReqBox.h"
#include "InstallBox.h"
#include "DirListBox.h"
#include "MainWindow.h"

using namespace color;

/*******************************************************************************

Prints to end of line, padding with spaces

*******************************************************************************/
void MainWindow::printToEol(const std::string & msg) const
{
  int i, y, x, rows, cols, nspaces, msglen;

  getmaxyx(stdscr, rows, cols);
  getyx(stdscr, y, x);

  msglen = msg.size();
  if (msglen > cols-x) { printw(msg.substr(0, cols-x).c_str()); }
  else
  {
    nspaces = std::max(cols-x-msglen, 0);
    printw(msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { printw(" "); }
  }
}

/*******************************************************************************

Prints/clears status message

*******************************************************************************/
void MainWindow::printStatus(const std::string & msg)
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  printToEol(msg);
  _status = msg;
  refresh();
}

void MainWindow::clearStatus()
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  _status = "";
  refresh();
}

void MainWindow::refreshStatus() { printStatus(_status); }

/*******************************************************************************

Redraws header and footer

*******************************************************************************/
void MainWindow::redrawHeaderFooter() const
{
  int rows, cols;
  int pair_title, pair_info;

  getmaxyx(stdscr, rows, cols);

  // Draw title

  move(0, 0);
  clrtoeol();
  pair_title = colors.pair(fg_title, bg_title);
  if (pair_title != -1) { attron(COLOR_PAIR(pair_title)); }
  attron(A_BOLD);
  printToEol(_title);
  if (pair_title != -1) { attroff(COLOR_PAIR(pair_title)); }
  attroff(A_BOLD);

  // Print filter selection

  move(2, 0);
  clrtoeol();
  printToEol("Showing: " + _filter);

  // Draw footer

  move(rows-1, 0);
  clrtoeol();
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_info != -1) { attron(COLOR_PAIR(pair_info)); }
  attron(A_BOLD);
  printToEol(_info);
  if (pair_info != -1) { attroff(COLOR_PAIR(pair_info)); }
  attroff(A_BOLD);

  refresh();
}

/*******************************************************************************

Redraws windows in horizontal layout

*******************************************************************************/
void MainWindow::redrawWindowsHorz()
{
  int rows, cols;
  int listrows, leftlistcols, rightlistcols, xrightlist;
  const int minlistcols = 5;
  const int minlistrows = 3;

  getmaxyx(stdscr, rows, cols);

  // Set window dimensions and positions

  listrows = std::max(rows-5, minlistrows);
  leftlistcols = std::max(int(std::floor(double(cols)/2.)), minlistcols);
  rightlistcols = std::max(cols - leftlistcols, minlistcols);
  xrightlist = leftlistcols;

  // Position windows

  mvwin(_win1, 3, 0);
  wresize(_win1, listrows, leftlistcols);
  mvwin(_win2, 3, xrightlist);
  wresize(_win2, listrows, rightlistcols); 
}

/*******************************************************************************

Redraws windows in vertical layout

*******************************************************************************/
void MainWindow::redrawWindowsVert()
{
  int rows, cols;
  int listcols, toplistrows, botlistrows, ybotlist;
  const int minlistcols = 5;
  const int minlistrows = 3;

  getmaxyx(stdscr, rows, cols);

  // Set window dimensions and positions

  listcols = std::max(cols, minlistcols);
  toplistrows = std::max(int(std::floor(double(rows-5)/2.)), minlistrows);
  botlistrows = std::max(rows-5 - toplistrows, minlistrows);
  ybotlist = 3 + toplistrows;

  // Position windows

  mvwin(_win1, 3, 0);
  wresize(_win1, toplistrows, listcols);
  mvwin(_win2, ybotlist, 0);
  wresize(_win2, botlistrows, listcols); 
}

/*******************************************************************************

Redraws windows

*******************************************************************************/
void MainWindow::redrawWindows(bool force)
{
  // "Resetting" the windows like this before placing them seems to help avoid
  // some weird behavior (e.g., with toggleLayout not placing _win2 in the right
  // spot the first time otherwise).

  mvwin(_win1, 0, 0);
  wresize(_win1, 1, 1);
  mvwin(_win2, 0, 0);
  wresize(_win2, 1, 1);

  // Set up windows

  if (_layout == 0) { redrawWindowsHorz(); }
  else { redrawWindowsVert(); }

  // Redraw windows

  _clistbox.draw(force);
  _blistboxes[_category_idx].draw(force);
}

/*******************************************************************************

Redraws window

*******************************************************************************/
void MainWindow::redrawAll(bool force)
{
  clear();

  // Draw stuff

  redrawHeaderFooter(); 
  redrawWindows(force);
  refreshStatus();
}

/*******************************************************************************

Toggles horizontal/vertical layout

*******************************************************************************/
void MainWindow::toggleLayout()
{
  if (_layout == 0) { _layout = 1; }
  else { _layout = 0; }

  redrawAll(true);
}

/*******************************************************************************

Clears windows, data, etc.

*******************************************************************************/
void MainWindow::clearData()
{
  if (_win1) { delwin(_win1); }
  if (_win2) { delwin(_win2); }
  _blistboxes.resize(0);
  _slackbuilds.resize(0);
  _installedlist.resize(0);
  _nondeplist.resize(0);
  _categories.resize(0);
  _category_idx = 0;
  _activated_listbox = 0;
}

/*******************************************************************************

Displays all SlackBuilds

*******************************************************************************/
void MainWindow::filterAll()
{
  unsigned int i, j, nbuilds, ncategories;
  BuildListBox initlistbox;

  _filter = "all SlackBuilds";
  printStatus("Filtering by all SlackBuilds ...");

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  nbuilds = _slackbuilds.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  _clistbox.clearList();
  _clistbox.setActivated(true);
  _activated_listbox = 0;
  _category_idx = 0;
  for ( j = 0; j < ncategories; j++ )
  {
    _clistbox.addItem(&_categories[j]);
    BuildListBox blistbox;
    blistbox.setWindow(_win2);
    blistbox.setName(_categories[j].name());
    blistbox.setActivated(false);
    _blistboxes.push_back(blistbox);
  }
  for ( i = 0; i < nbuilds; i++ )
  {
    for ( j = 0; j < ncategories; j++ )
    {
      if (_slackbuilds[i].getProp("category") == _categories[j].name())
      {
        _blistboxes[j].addItem(&_slackbuilds[i]);
      }
    }
  }

  // Check whether categories should be tagged

  for ( j = 0; j < ncategories; j++ )
  {
    if (_blistboxes[j].allTagged()) { _clistbox.itemByIdx(j)->
                                                  setBoolProp("tagged", true); }
    else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
  }

  if (nbuilds == 0) 
  { 
    printStatus("No SlackBuilds. Run the sync command first."); 
    initlistbox.setWindow(_win2);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    _blistboxes.push_back(initlistbox);
  }
  else if (nbuilds == 1) { printStatus("1 SlackBuild in repository."); }
  else { printStatus(int2string(nbuilds) + " SlackBuilds in repository."); }
}

/*******************************************************************************

Displays installed SlackBuilds

*******************************************************************************/
void MainWindow::filterInstalled()
{
  unsigned int i, j, ncategories, nfiltered_categories, ninstalled;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  _filter = "installed SlackBuilds";
  printStatus("Filtering by installed SlackBuilds ...");

  if (_installedlist.size() == 0) 
  { 
    list_installed(_slackbuilds, _installedlist); 
  }

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  ninstalled = _installedlist.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  _clistbox.clearList();
  _clistbox.setActivated(true);
  _category_idx = 0;
  _activated_listbox = 0;
  filtered_categories.resize(0);

  for ( i = 0; i < ninstalled; i++ )
  {
    category_found = false;
    nfiltered_categories = filtered_categories.size();
    for ( j = 0; j < nfiltered_categories; j++ )
    {
      if (_installedlist[i]->getProp("category") == filtered_categories[j])
      {
        _blistboxes[j].addItem(_installedlist[i]);
        category_found = true;
        break;
      } 
    }
    if (! category_found)
    {
      for ( j = 0; j < ncategories; j++ )
      {
        if ( _installedlist[i]->getProp("category") == _categories[j].name())
        {
          _clistbox.addItem(&_categories[j]);
          BuildListBox blistbox;
          blistbox.setWindow(_win2);
          blistbox.setName(_categories[j].name());
          blistbox.setActivated(false);
          blistbox.addItem(_installedlist[i]);
          _blistboxes.push_back(blistbox); 
          filtered_categories.push_back(_installedlist[i]->getProp("category"));
          break;
        }
      }
    }
  } 

  // Check whether categories should be tagged

  nfiltered_categories = filtered_categories.size();
  for ( j = 0; j < nfiltered_categories; j++ )
  {
    if (_blistboxes[j].allTagged()) { _clistbox.itemByIdx(j)->
                                                  setBoolProp("tagged", true); }
    else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
  }

  if (ninstalled == 0) 
  { 
    printStatus("No installed SlackBuilds."); 
    initlistbox.setWindow(_win2);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    _blistboxes.push_back(initlistbox);
  }
  else if (ninstalled == 1) { printStatus("1 installed SlackBuild."); }
  else { printStatus(int2string(ninstalled) + " installed SlackBuilds."); }
}

/*******************************************************************************

Displays upgradable SlackBuilds

*******************************************************************************/
void MainWindow::filterUpgradable()
{
  unsigned int i, j, ncategories, nfiltered_categories, ninstalled, nupgradable;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  _filter = "upgradable SlackBuilds";
  printStatus("Filtering by upgradable SlackBuilds ...");

  if (_installedlist.size() == 0) 
  { 
    list_installed(_slackbuilds, _installedlist); 
  }

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  ninstalled = _installedlist.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  _clistbox.clearList();
  _clistbox.setActivated(true);
  _category_idx = 0;
  _activated_listbox = 0;
  filtered_categories.resize(0);
  nupgradable = 0;

  for ( i = 0; i < ninstalled; i++ )
  {
    if (_installedlist[i]->upgradable())
    {
      category_found = false;
      nfiltered_categories = filtered_categories.size();
      for ( j = 0; j < nfiltered_categories; j++ )
      {
        if (_installedlist[i]->getProp("category") == filtered_categories[j])
        {
          _blistboxes[j].addItem(_installedlist[i]);
          category_found = true;
          break;
        } 
      }
      if (! category_found)
      {
        for ( j = 0; j < ncategories; j++ )
        {
          if ( _installedlist[i]->getProp("category") == _categories[j].name())
          {
            _clistbox.addItem(&_categories[j]);
            BuildListBox blistbox;
            blistbox.setWindow(_win2);
            blistbox.setName(_categories[j].name());
            blistbox.setActivated(false);
            blistbox.addItem(_installedlist[i]);
            _blistboxes.push_back(blistbox); 
            filtered_categories.push_back(
                                        _installedlist[i]->getProp("category"));
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
    if (_blistboxes[j].allTagged()) { _clistbox.itemByIdx(j)->
                                                  setBoolProp("tagged", true); }
    else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
  }

  if (nupgradable == 0) 
  { 
    printStatus("No upgradable SlackBuilds."); 
    initlistbox.setWindow(_win2);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    _blistboxes.push_back(initlistbox);
  }
  else if (nupgradable == 1) { printStatus("1 upgradable SlackBuild."); }
  else { printStatus(int2string(nupgradable) + " upgradable SlackBuilds."); }
}

/*******************************************************************************

Displays tagged SlackBuilds

*******************************************************************************/
void MainWindow::filterTagged()
{
  unsigned int i, j, ncategories, nfiltered_categories, nbuilds, ntagged;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  _filter = "tagged SlackBuilds";
  printStatus("Filtering by tagged SlackBuilds ...");

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  nbuilds = _slackbuilds.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  _clistbox.clearList();
  _clistbox.setActivated(true);
  _category_idx = 0;
  _activated_listbox = 0;
  filtered_categories.resize(0);
  ntagged = 0;

  for ( i = 0; i < nbuilds; i++ )
  {
    if (_slackbuilds[i].getBoolProp("tagged"))
    {
      category_found = false;
      nfiltered_categories = filtered_categories.size();
      for ( j = 0; j < nfiltered_categories; j++ )
      {
        if (_slackbuilds[i].getProp("category") == filtered_categories[j])
        {
          _blistboxes[j].addItem(&_slackbuilds[i]);
          category_found = true;
          break;
        } 
      }
      if (! category_found)
      {
        for ( j = 0; j < ncategories; j++ )
        {
          if ( _slackbuilds[i].getProp("category") == _categories[j].name())
          {
            _clistbox.addItem(&_categories[j]);
            BuildListBox blistbox;
            blistbox.setWindow(_win2);
            blistbox.setName(_categories[j].name());
            blistbox.setActivated(false);
            blistbox.addItem(&_slackbuilds[i]);
            _blistboxes.push_back(blistbox); 
            filtered_categories.push_back(_slackbuilds[i].getProp("category"));
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
    if (_blistboxes[j].allTagged()) { _clistbox.itemByIdx(j)->
                                                  setBoolProp("tagged", true); }
    else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
  }

  if (ntagged == 0) 
  { 
    printStatus("No tagged SlackBuilds."); 
    initlistbox.setWindow(_win2);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    _blistboxes.push_back(initlistbox);
  }
  else if (ntagged == 1) { printStatus("1 tagged SlackBuild."); }
  else { printStatus(int2string(ntagged) + " tagged SlackBuilds."); }
}

/*******************************************************************************

Displays installed SlackBuilds that are not a dependency of any other installed
SlackBuild

*******************************************************************************/
void MainWindow::filterNonDeps()
{
  unsigned int i, j, ncategories, nfiltered_categories, nnondeps;
  std::vector<std::string> filtered_categories;
  bool category_found;
  BuildListBox initlistbox;

  _filter = "non-dependencies";
  printStatus("Filtering by non-dependencies ...");

  if (_installedlist.size() == 0) 
  { 
    list_installed(_slackbuilds, _installedlist); 
  }
  if (_nondeplist.size() == 0) { list_nondeps(_installedlist, _nondeplist); }

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  nnondeps = _nondeplist.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  _clistbox.clearList();
  _clistbox.setActivated(true);
  _category_idx = 0;
  _activated_listbox = 0;
  filtered_categories.resize(0);

  for ( i = 0; i < nnondeps; i++ )
  {
    category_found = false;
    nfiltered_categories = filtered_categories.size();
    for ( j = 0; j < nfiltered_categories; j++ )
    {
      if (_nondeplist[i]->getProp("category") == filtered_categories[j])
      {
        _blistboxes[j].addItem(_nondeplist[i]);
        category_found = true;
        break;
      } 
    }
    if (! category_found)
    {
      for ( j = 0; j < ncategories; j++ )
      {
        if ( _nondeplist[i]->getProp("category") == _categories[j].name())
        {
          _clistbox.addItem(&_categories[j]);
          BuildListBox blistbox;
          blistbox.setWindow(_win2);
          blistbox.setName(_categories[j].name());
          blistbox.setActivated(false);
          blistbox.addItem(_nondeplist[i]);
          _blistboxes.push_back(blistbox); 
          filtered_categories.push_back(_nondeplist[i]->getProp("category"));
          break;
        }
      }
    }
  } 

  // Check whether categories should be tagged

  nfiltered_categories = filtered_categories.size();
  for ( j = 0; j < nfiltered_categories; j++ )
  {
    if (_blistboxes[j].allTagged()) { _clistbox.itemByIdx(j)->
                                                  setBoolProp("tagged", true); }
    else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
  }

  if (nnondeps == 0) 
  { 
    printStatus("No non-dependencies."); 
    initlistbox.setWindow(_win2);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    _blistboxes.push_back(initlistbox);
  }
  else if (nnondeps == 1) { printStatus("1 non-dependency."); }
  else { printStatus(int2string(nnondeps) + " non-dependencies."); }
}

/*******************************************************************************

Filters SlackBuilds by search term

*******************************************************************************/
void MainWindow::filterSearch(const std::string & searchterm, 
                              bool case_sensitive, bool whole_word)
{
  unsigned int i, j, nbuilds, ncategories, nsearch_categories, nsearch;
  std::vector<std::string> search_categories;
  std::string term, tomatch;
  bool match, category_found;
  BuildListBox initlistbox;

  _filter = "search for " + searchterm;
  printStatus("Searching for " + searchterm + " ...");

  // For case insensitive search, we will convert both to lower case

  if (case_sensitive) { term = searchterm; }
  else { term = string_to_lower(searchterm); }

  // Create list boxes (Careful! If you use push_back, etc. later on the lists,
  // the list boxes must be regenerated because their pointers will become
  // invalid.)

  nbuilds = _slackbuilds.size();
  ncategories = _categories.size();
  _blistboxes.resize(0);
  _clistbox.clearList();
  _clistbox.setActivated(true);
  _activated_listbox = 0;
  _category_idx = 0;
  search_categories.resize(0);
  nsearch = 0;

  for ( i = 0; i < nbuilds; i++ )
  {
    category_found = false;

    // Check for search term in SlackBuild name

    if (case_sensitive) { tomatch = _slackbuilds[i].name(); }
    else { tomatch = string_to_lower(_slackbuilds[i].name()); }
    if (whole_word) { match = (term == tomatch); }
    else { match = (tomatch.find(term) != std::string::npos); }
    if (! match) { continue; }

    nsearch++;
    nsearch_categories = search_categories.size();
    for ( j = 0; j < nsearch_categories; j++ )
    {
      if (_slackbuilds[i].getProp("category") == search_categories[j])
      {
        _blistboxes[j].addItem(&_slackbuilds[i]);
        category_found = true;
        break;
      }
    }
    if (! category_found)
    {
      for ( j = 0; j < ncategories; j++ )
      {
        if (_slackbuilds[i].getProp("category") == _categories[j].name())
        {
          _clistbox.addItem(&_categories[j]);
          BuildListBox blistbox;
          blistbox.setWindow(_win2);
          blistbox.setName(_categories[j].name());
          blistbox.setActivated(false);
          blistbox.addItem(&_slackbuilds[i]);
          _blistboxes.push_back(blistbox);
          search_categories.push_back(_slackbuilds[i].getProp("category"));
          break;
        }
      }
    } 
  }

  // Check whether categories should be tagged

  nsearch_categories = search_categories.size();
  for ( j = 0; j < nsearch_categories; j++ )
  {
    if (_blistboxes[j].allTagged()) { _clistbox.itemByIdx(j)->
                                                  setBoolProp("tagged", true); }
    else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
  }

  if (nsearch== 0)
  {
    printStatus("No matches for " + searchterm + ".");
    initlistbox.setWindow(_win2);
    initlistbox.setActivated(false);
    initlistbox.setName("SlackBuilds");
    _blistboxes.push_back(initlistbox);
  }
  else if (nsearch == 1) { printStatus("1 match for " + searchterm + "."); }
  else { printStatus(int2string(nsearch) + " matches for "
                     + searchterm + "."); }
}

/*******************************************************************************

Installs/upgrades/reinstalls/removes SlackBuild and dependencies. Returns true
if anything was changed, false otherwise.

*******************************************************************************/
bool MainWindow::modifyPackage(BuildListItem & build,
                               const std::string & action)
{
  WINDOW *installerwin;
  int check, nchanged;
  std::string selection;
  bool getting_input, needs_rebuild;
  unsigned int ninstaller;
  InstallBox installer;

  printStatus("Computing dependencies for " + build.name() + " ...");
  check = installer.create(build, _slackbuilds, action);

//FIXME: Make some sort of error message class to show this
  if (check != 0) 
  { 
    printStatus("Some requirements of " + build.name() +
                " not found in repository."); 
    return false;
  }

  ninstaller = installer.numItems();
  if (ninstaller == 2)
  { 
    if (action == "Remove")
      printStatus("1 installed dependency for " + build.name() + ".");
    else
      printStatus("1 dependency for " + build.name() + ".");
  }
  else 
  { 
    if (action == "Remove")
      printStatus(int2string(ninstaller-1) + 
                  " installed dependencies for " + build.name() + ".");
    else
      printStatus(int2string(ninstaller-1) + 
                  " dependencies for " + build.name() + ".");
  }

  installerwin = newwin(1, 1, 0, 0);
  installer.setWindow(installerwin);
  placePopup(&installer, installerwin);

  getting_input = true;
  needs_rebuild = false;
  while (getting_input)
  {
    selection = installer.exec(); 
    if (selection == signals::keyEnter)
    {
//FIXME: Make some sort of warning message class to show this
      if (! installer.installingAllDeps())
      {
        printStatus("Warning: not installing/upgrading some dependencies. "
                    + std::string("Continue anyway?"));
      }
      def_prog_mode();
      endwin();
      check = installer.applyChanges(nchanged);
      if (nchanged > 0) { needs_rebuild = true; }
// FIXME: Error message box if check != 0
      reset_prog_mode();
      redrawAll(true);
      getting_input = false;
    }
    else if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&installer, installerwin);
      redrawAll(true);
      clearStatus();
    }
  }

  clearStatus();
  delwin(installerwin);

  return needs_rebuild;
}

/*******************************************************************************

Shows build order for a SlackBuild

*******************************************************************************/
void MainWindow::showBuildOrder(BuildListItem & build)
{
  WINDOW *buildorderwin;
  int check;
  std::string selection;
  bool getting_input;
  unsigned int nbuildorder;
  BuildOrderBox buildorder;

  printStatus("Computing build order for " + build.name() + " ...");
  check = buildorder.create(build, _slackbuilds);

//FIXME: Make some sort of error message class to show this
  if (check != 0) 
  { 
    printStatus("Some requirements of " + build.name() +
                " not found in repository."); 
    return;
  }

  nbuildorder = buildorder.numItems();

  if (nbuildorder == 1) { printStatus(
                     "1 SlackBuild in build order for " + build.name() + "."); }
  else { printStatus(int2string(nbuildorder) + 
                     " SlackBuilds in build order for " + build.name() + "."); }

  buildorderwin = newwin(1, 1, 0, 0);
  buildorder.setWindow(buildorderwin);
  placePopup(&buildorder, buildorderwin);

  getting_input = true;
  while (getting_input)
  {
    selection = buildorder.exec(); 
    if ( (selection == signals::keyEnter) || 
         (selection == signals::quit) ) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&buildorder, buildorderwin);
      redrawAll(true);
      clearStatus();
    }
  }

  clearStatus();
  delwin(buildorderwin);
}

/*******************************************************************************

Shows installed SlackBuilds depending on a given SlackBuild

*******************************************************************************/
void MainWindow::showInverseReqs(BuildListItem & build)
{
  WINDOW *invreqwin;
  std::string selection;
  bool getting_input;
  unsigned int ninvreqs;
  InvReqBox invreqs;

  printStatus("Computing installed SlackBuilds depending on "
              + build.name() + " ...");
  invreqs.create(build, _installedlist);

  ninvreqs = invreqs.numItems();
  if (ninvreqs == 0) { printStatus("No installed SlackBuilds depend on "
                                   + build.name() + "."); }
  else if (ninvreqs == 1) { printStatus(
          "1 installed SlackBuild depends on " + build.name() + "."); }
  else { printStatus(int2string(ninvreqs) + 
           " installed SlackBuilds depend on " + build.name() + "."); }

  invreqwin = newwin(1, 1, 0, 0);
  invreqs.setWindow(invreqwin);
  placePopup(&invreqs, invreqwin);

  getting_input = true;
  while (getting_input)
  {
    selection = invreqs.exec(); 
    if ( (selection == signals::keyEnter) || 
         (selection == signals::quit) ) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&invreqs, invreqwin);
      redrawAll(true);
      clearStatus();
    }
  }

  clearStatus();
  delwin(invreqwin);
}

/*******************************************************************************

Displays a file browser in the directory of a given SlackBuild

*******************************************************************************/
void MainWindow::browseFiles(const BuildListItem & build)
{
  WINDOW *browserwin;
  std::string selection, builddir, type, fname;
  int check;
  bool getting_input;
  DirListBox browser;

  builddir = repo_dir + "/" + build.getProp("category") + "/" + build.name();
  check = browser.setDirectory(builddir);

//FIXME: Make some sort of error message class to show this
  if (check != 0) 
  { 
    printStatus("Unable to access build directory for " + build.name() + ".");
    return;
  }

  browserwin = newwin(1, 1, 0, 0);
  browser.setName("Browsing " + build.getProp("category") + "/" + build.name());
  browser.setWindow(browserwin);
  placePopup(&browser, browserwin);

  getting_input = true;
  while (getting_input)
  {
    selection = browser.exec(); 
    if (selection == signals::keyEnter)
    {
      type = browser.highlightedItem()->getProp("type");
      if ( (type == "reg") || (type == "lnk") )
      {
        fname = browser.highlightedItem()->name();
        def_prog_mode();
        endwin();
//FIXME: Display an error if the command fails
        check = view_file(builddir + "/" + fname);
        reset_prog_mode();
        redrawAll(true);
      }
//FIXME: Make some sort of error message class to show this
      else { printStatus("Can only view files and symlinks."); }
    }
    else if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&browser, browserwin);
      redrawAll(true);
      clearStatus();
    }
  }

  clearStatus();
  delwin(browserwin);
}

/*******************************************************************************

Syncs/updates SlackBuilds repository

*******************************************************************************/
int MainWindow::syncRepo()
{
  int check;

  def_prog_mode();
  endwin();

  check = sync();
  reset_prog_mode();
//FIXME: Display an error if the command fails

  if (check == 0) 
  {
    clearData();
    initialize();
  }

  return check;
}

/*******************************************************************************

Sets size and position of popup boxes
FIXME: use templates instead of overloading?

*******************************************************************************/
void MainWindow::popupSize(int & height, int & width, 
                           AbstractListBox *popup) const
{
  int minheight, minwidth, prefheight, prefwidth, maxheight, maxwidth;
  int rows, cols;

  getmaxyx(stdscr, rows, cols);

  popup->minimumSize(minheight, minwidth);
  popup->preferredSize(prefheight, prefwidth);
  maxheight = rows-4;
  maxwidth = cols-4;

  if (prefheight < maxheight) { height = prefheight; }
  else 
  { 
    if (maxheight-1 > minheight) { height = maxheight-1; }
    else { height = minheight; }
  }
  if (prefwidth < maxwidth) { width = prefwidth; }
  else 
  { 
    if (maxwidth-1 > minwidth) { width = minwidth-1; }
    else { width = minwidth; }
  }
} 

void MainWindow::popupSize(int & height, int & width, InputBox *popup) const
{
  int minheight, minwidth, prefheight, prefwidth, maxheight, maxwidth;
  int rows, cols;

  getmaxyx(stdscr, rows, cols);

  popup->minimumSize(minheight, minwidth);
  popup->preferredSize(prefheight, prefwidth);
  maxheight = rows-4;
  maxwidth = cols-4;

  if (prefheight < maxheight) { height = prefheight; }
  else 
  { 
    if (maxheight-1 > minheight) { height = maxheight-1; }
    else { height = minheight; }
  }
  if (prefwidth < maxwidth) { width = prefwidth; }
  else 
  { 
    if (maxwidth-1 > minwidth) { width = minwidth-1; }
    else { width = minwidth; }
  }
} 

void MainWindow::placePopup(AbstractListBox *popup, WINDOW *win) const
{
  int rows, cols, height, width, top, left;

  getmaxyx(stdscr, rows, cols);
  popupSize(height, width, popup);
  left = std::floor(double(cols)/2. - double(width)/2.);
  top = std::floor(double(rows)/2. - double(height)/2.);
  mvwin(win, top, left);
  wresize(win, height, width);
}

void MainWindow::placePopup(InputBox *popup, WINDOW *win) const
{
  int rows, cols, height, width, top, left;

  getmaxyx(stdscr, rows, cols);
  popupSize(height, width, popup);
  left = std::floor(double(cols)/2. - double(width)/2.);
  top = std::floor(double(rows)/2. - double(height)/2.);
  mvwin(win, top, left);
  wresize(win, height, width);
}

/*******************************************************************************

Hides a window by putting it at the center of the screen and giving it 0 size

*******************************************************************************/
void MainWindow::hideWindow(WINDOW *win) const
{
  int rows, cols, left, top;

  getmaxyx(stdscr, rows, cols);
  left = std::floor(double(cols)/2.);
  top = std::floor(double(rows)/2.);
  mvwin(win, top, left);
  wresize(win, 0, 0);
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
MainWindow::MainWindow()
{
  _win1 = NULL;
  _win2 = NULL;
  _blistboxes.resize(0);
  _slackbuilds.resize(0);
  _installedlist.resize(0);
  _nondeplist.resize(0);
  _categories.resize(0);
  _title = "sboui Development Version";
  _filter = "all SlackBuilds";
  _info = "f: Filter | /: Search | o: Options | ?: Help";
  _status = "";
  _category_idx = 0;
  _activated_listbox = 0;
  _layout = 0;
}

MainWindow::~MainWindow() { clearData(); }

/*******************************************************************************

First time window setup

*******************************************************************************/
int MainWindow::initialize()
{
  BuildListBox initlistbox;
  int retval;

  // Create windows (note: geometry gets set in redrawWindows);

  _win1 = newwin(1, 1, 0, 0);
  _win2 = newwin(1, 1, 0, 0);

  _clistbox.clearList();
  _clistbox.setWindow(_win1);
  _clistbox.setActivated(true);
  _clistbox.setName("Groups");
  initlistbox.setWindow(_win2);
  initlistbox.setActivated(false);
  initlistbox.setName("SlackBuilds");
  _blistboxes.push_back(initlistbox);

  redrawAll(true);

  // Read SlackBuilds repository

  printStatus("Reading SlackBuilds repository ...");
  retval = readLists();

  // Set filter 

//FIXME: Make error message class for this
  if (retval != 0) { printStatus("Error reading SlackBuilds repository."); }
  else 
  { 
    if (_filter == "installed SlackBuilds") { filterInstalled(); }
    else if (_filter == "upgradable SlackBuilds") { filterUpgradable(); }
    else if (_filter == "non-dependencies") { filterNonDeps(); }
    else { filterAll(); }
  }
  redrawAll(true);

  return retval;
}

/*******************************************************************************

Creates master list of SlackBuilds

*******************************************************************************/
int MainWindow::readLists()
{
  int check;
  unsigned int i, j, nbuilds, ncategories; 
  bool new_category;

  // Get list of SlackBuilds

  check = read_repo(_slackbuilds); 
  if (check != 0) { return check; }

  // Create list of categories

  nbuilds = _slackbuilds.size();
  for ( i = 0; i < nbuilds; i++ )
  { 
    ncategories = _categories.size();
    new_category = true;
    for ( j = 0; j < ncategories; j++ )
    {
      if (_categories[j].name() == _slackbuilds[i].getProp("category"))
      {
        new_category = false;
        break;
      }
    }
    if (new_category)
    {
      CategoryListItem citem;
      citem.setName(_slackbuilds[i].getProp("category"));
      citem.setProp("category", _slackbuilds[i].getProp("category"));
      _categories.push_back(citem);
    } 
  }

  // Determine which are installed and their versions

  list_installed(_slackbuilds, _installedlist);

  return 0;
}

/*******************************************************************************

Sets properties

*******************************************************************************/
void MainWindow::setTitle(const std::string & title) { _title = title; }
void MainWindow::setInfo(const std::string & info) { _info = info; }

/*******************************************************************************

Filter dialog

*******************************************************************************/
void MainWindow::selectFilter()
{
  WINDOW *filterwin;
  std::string selection, selected;
  bool getting_selection;

  // Set up window

  filterwin = newwin(1, 1, 0, 0);
  _fbox.setWindow(filterwin);
  placePopup(&_fbox, filterwin);

  // Get filter selection

  getting_selection = true;
  while (getting_selection)
  {
    selected = "None";
    selection = _fbox.exec();
    getting_selection = false;
    if (selection == signals::keyEnter) { 
                                   selected = _fbox.highlightedItem()->name(); }

    // Note that upper/lower case checking for hotkeys has already happened in
    // SelectionBox, so just check the actual character here.

    if ( (selected == "All") || (selection == "A") )
    {
      if (_filter != "all SlackBuilds") { filterAll(); } 
    }
    else if ( (selected == "Installed") || (selection == "I") )
    {
      if (_filter != "installed SlackBuilds") { filterInstalled(); }
    }
    else if ( (selected == "Upgradable") || (selection == "U") )
    {
      if (_filter != "upgradable SlackBuilds") { filterUpgradable(); } 
    }
    // Tagged items could have changed, so allow this one to be re-selected
    else if ( (selected == "Tagged") || (selection == "T") ) { filterTagged(); }
    else if ( (selected == "Non-dependencies") || (selection == "N") )
    {
      if (_filter != "non-dependencies") { filterNonDeps(); } 
    }
    else if (selection == signals::resize)
    {
      getting_selection = true;
      placePopup(&_fbox, filterwin);
      redrawAll(true);
      clearStatus();
      _fbox.draw(true);
    }
  }

  // Get rid of window and redraw

  delwin(filterwin);
  redrawAll(true);
}

/*******************************************************************************

Search dialog

*******************************************************************************/
void MainWindow::search()
{
  WINDOW *searchwin;
  std::string searchterm;
  bool getting_input;

  // Set up window and search box

  searchwin = newwin(1, 1, 0, 0);
  _searchbox.setWindow(searchwin);
  _searchbox.setMessage("Search SlackBuilds");
  placePopup(&_searchbox, searchwin);

  // Get search term from user

  getting_input = true;
  while (getting_input)
  {
    getting_input = false;
    searchterm = _searchbox.exec();
    if (searchterm == signals::resize)
    {
      getting_input = true;
      placePopup(&_searchbox, searchwin);
      redrawAll(true);
      clearStatus();
      _searchbox.draw(true);
    }
    else if ( (searchterm != signals::quit) )
    { 
      if (_searchbox.searchString().size() > 0)
      {
        filterSearch(_searchbox.searchString(), _searchbox.caseSensitive(),
                     _searchbox.wholeWord());
      }
    }
  }

  // Get rid of window and redraw

  delwin(searchwin);
  redrawAll(true);
}

/*******************************************************************************

Dialog for actions pertaining to selected SlackBuild

*******************************************************************************/
void MainWindow::showBuildActions(BuildListItem & build)
{
  WINDOW *actionwin;
  std::string selection, selected, action;
  bool getting_selection, needs_rebuild;
  int check;
  BuildActionBox actionbox;

  // Set up windows and dialog

  actionwin = newwin(1, 1, 0, 0);
  actionbox.setWindow(actionwin);
  actionbox.setName("Actions for " + build.name());
  actionbox.create(build);
  placePopup(&actionbox, actionwin);

  // Get selection

  getting_selection = true;
  while (getting_selection)
  {
    selected = "None";
    selection = actionbox.exec();
    if (selection == signals::keyEnter) { 
                               selected = actionbox.highlightedItem()->name(); }

    // Note that upper/lower case checking for hotkeys has already happened in
    // SelectionBox, so just check the actual character here.

    if ( (selected == "View README") || (selection == "V") )
    {
      def_prog_mode();
      endwin();
//FIXME: display error if the command fails
      check = view_readme(build); 
      reset_prog_mode();
      redrawAll(true);
    }
    else if ( (selected == "Install") || (selection == "I") ||
              (selected == "Upgrade") || (selection == "U") ||
              (selected == "Reinstall") || (selection == "e") ||
              (selected == "Remove") || (selection == "R") )
    { 
      if ( (selected == "Install") || (selection == "I") ) 
        action = "Install";
      else if ( (selected == "Upgrade") || (selection == "U") )
        action = "Upgrade";
      else if ( (selected == "Reinstall") || (selection == "e") ) 
        action = "Reinstall";
      else
        action = "Remove";

      hideWindow(actionwin);
      redrawAll(true);
      needs_rebuild = modifyPackage(build, action);
      if (needs_rebuild) { getting_selection = false; }
      else
      {
        placePopup(&actionbox, actionwin);
        redrawAll(true);
      }
    }                                              
    else if ( (selected == "Compute build order") || (selection == "C") )
    { 
      hideWindow(actionwin);
      redrawAll(true);
      showBuildOrder(build);
      placePopup(&actionbox, actionwin);
      redrawAll(true);
    }                                              
    else if ( (selected == "List inverse deps") || (selection == "L") )
    { 
      hideWindow(actionwin);
      redrawAll(true);
      showInverseReqs(build);
      placePopup(&actionbox, actionwin);
      redrawAll(true);
    }                                              
    else if ( (selected == "Browse files") || (selection == "B") )
    {
      hideWindow(actionwin);
      redrawAll(true);
      browseFiles(build);
      placePopup(&actionbox, actionwin);
      redrawAll(true);
    }
    else if (selection == signals::resize)
    {
      placePopup(&actionbox, actionwin);
      redrawAll(true);
      clearStatus();
      actionbox.draw(true);
    }
    else { getting_selection = false; }
  }

  // Get rid of window and redraw

  delwin(actionwin);
  redrawAll(true);

  // Rebuild lists if SlackBuilds were installed/upgraded/reinstalled/removed

  if (needs_rebuild)
  { 
    clearData();
    initialize();
  }
}

/*******************************************************************************

Shows the main window

*******************************************************************************/
void MainWindow::show()
{
  std::string selection, statusmsg;
  bool getting_input, all_tagged;
  BuildListItem *build;

  redrawAll();

  // Main event loop

  getting_input = true;
  while (getting_input)
  {
    // Get input from Categories list box

    if (_activated_listbox == 0)
    {
      selection = _clistbox.exec();

      // Highlighted item changed

      if (selection == signals::highlight)
      {
        _category_idx = _clistbox.highlight(); 
        _blistboxes[_category_idx].draw(true);
      }

      // Tab signal

      else if (selection == signals::keyTab)
      {
        _clistbox.setActivated(false);
        _clistbox.draw();
        _blistboxes[_category_idx].setActivated(true);
        _activated_listbox = 1;

        // Display status message for installed SlackBuild
        /* Note: use static_cast because it is certain that build is actually a
           BuildListItem, and the base class (ListItem) has no virtual members
           (not polymorphic). Otherwise, should use dynamic_cast.
           http://stackoverflow.com/questions/332030/when-should-static-cast-dynamic-cast-const-cast-and-reinterpret-cast-be-used 
           http://stackoverflow.com/questions/6322949/downcasting-using-the-static-cast-in-c */

        build = static_cast<BuildListItem*>(
                                  _blistboxes[_category_idx].highlightedItem());
        if (build->getBoolProp("installed"))
        {
          statusmsg = "Installed: " + build->getProp("installed_version") +
            " -> Available: " + build->getProp("available_version");
          printStatus(statusmsg);
        }
        else { clearStatus(); }
      }

      // Tag signal: tag/untag all items in category

      else if (selection == "t")
      {
        _blistboxes[_category_idx].tagAll();
        _category_idx = _clistbox.highlight();
        _blistboxes[_category_idx].draw();
      }
    }

    // Get input from SlackBuilds list box

    else if (_activated_listbox == 1)
    {
      selection = _blistboxes[_category_idx].exec();

      // Highlighted item changed

      if (selection == signals::highlight)
      {
        // Display status message for installed SlackBuild

        build = static_cast<BuildListItem*>(
                                  _blistboxes[_category_idx].highlightedItem());
        if (build->getBoolProp("installed"))
        {
          statusmsg = "Installed: " + build->getProp("installed_version") +
            " -> Available: " + build->getProp("available_version");
          printStatus(statusmsg);
        }
        else { clearStatus(); }
      }

      // Tab signal
      
      else if (selection == signals::keyTab)
      {
        _blistboxes[_category_idx].setActivated(false);
        _blistboxes[_category_idx].draw();
        _clistbox.setActivated(true);
        _activated_listbox = 0;
        clearStatus();
      }

      // Tag signal: see if we need to change tag for category

      else if (selection == "t")
      {
        all_tagged = _blistboxes[_category_idx].allTagged();
        if (_clistbox.highlightedItem()->getBoolProp("tagged"))
        {
          if (! all_tagged) 
          { 
            _clistbox.highlightedItem()->setBoolProp("tagged", false);
            _clistbox.draw();
          }
        }
        else 
        {
          if (all_tagged) 
          { 
            _clistbox.highlightedItem()->setBoolProp("tagged", true);
            _clistbox.draw();
          }
        }
      }
     
      // Enter signal: show action dialog

      else if (selection == signals::keyEnter)
      {
        build = static_cast<BuildListItem*>(
                                  _blistboxes[_category_idx].highlightedItem());
        showBuildActions(*build);
      }
    }

    // Key signals with the same action w/ either type of list box

    if (selection == "q") { getting_input = false; }
    else if (selection == signals::resize) { redrawAll(true); }
    else if (selection == "f") { selectFilter(); }
    else if (selection == "/") { search(); }
    else if (selection == "s") { syncRepo(); }
    else if (selection == "l") { toggleLayout(); }
  }
}
