#include <vector>
#include <string>
#include <sstream>
#include <cmath>     // floor
#include "curses.h"
#include "Color.h"
#include "color_settings.h"
#include "signals.h"
#include "backend.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "FilterBox.h"
#include "SearchBox.h"
#include "BuildActionBox.h"
#include "MainWindow.h"

using namespace color;

std::string int2String(int inval)
{
  std::string outstr;
  std::stringstream ss;

  ss << inval;
  ss >> outstr;
  return outstr;
}

std::string string_to_lower(const std::string & instr)
{
  std::string outstr;
  std::string::size_type k, len;

  len = instr.size();
  for ( k = 0; k < len; k++ ) 
  { 
    outstr.push_back(std::tolower(instr[k]));
  }
  return outstr;
}

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
void MainWindow::printStatus(const std::string & msg) const
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  printToEol(msg);
  refresh();
}

void MainWindow::clearStatus() const
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  refresh();
}

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

Redraws windows

*******************************************************************************/
void MainWindow::redrawWindows(bool force)
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

  // Redraw windows

  _clistbox.draw(force);
  _blistboxes[_category_idx].draw(force);
}

/*******************************************************************************

Redraws window

*******************************************************************************/
void MainWindow::redrawAll(bool force)
{
  int rows, cols, i;

  getmaxyx(stdscr, rows, cols);

  // Clear everything except status line

  for ( i = 0; i < rows; i++ )
  {
    if ( i != rows-2 )
    {
      move(i, 0);
      clrtoeol();
    }
  }

  // Draw stuff

  redrawHeaderFooter(); 
  redrawWindows(force);
  refresh();
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
  else { printStatus(int2String(nbuilds) + " SlackBuilds in repository."); }
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
  else { printStatus(int2String(ninstalled) + " installed SlackBuilds."); }
}

/*******************************************************************************

Displays upgradable SlackBuilds

*******************************************************************************/
void MainWindow::filterUpgradable()
{
  unsigned int i, j, ncategories, nfiltered_categories, ninstalled, nupgradable;
  std::size_t len;
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
    len = _installedlist[i]->getProp("available_version").size();
    if (_installedlist[i]->getProp("installed_version").substr(0, len) != 
        _installedlist[i]->getProp("available_version")) 
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
  else { printStatus(int2String(nupgradable) + " upgradable SlackBuilds."); }
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
  else { printStatus(int2String(nnondeps) + " non-dependencies."); }
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
  else { printStatus(int2String(nsearch) + " matches for "
                     + searchterm + "."); }
}

/*******************************************************************************

Sets size and position of popup boxes
FIXME: use templates instead of overloading?

*******************************************************************************/
void MainWindow::popupSize(int & height, int & width, ListBox *popup) const
{
  int minheight, minwidth, prefheight, prefwidth, maxheight, maxwidth;
  int rows, cols;

  getmaxyx(stdscr, rows, cols);

  popup->minimumSize(minheight, minwidth);
  popup->preferredSize(prefheight, prefwidth);
  maxheight = rows-4;
  maxwidth = cols-4;

  if (prefheight < maxheight) { height = prefheight; }
  else { height = minheight; }
  if (prefwidth < maxwidth) { width = prefwidth; }
  else { width = minwidth; }
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
  else { height = minheight; }
  if (prefwidth < maxwidth) { width = prefwidth; }
  else { width = minwidth; }
} 

void MainWindow::placePopup(ListBox *popup, WINDOW *win) const
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

Constructor and destructor

*******************************************************************************/
MainWindow::MainWindow()
{
  _win1 = NULL;
  _win2 = NULL;
  _blistboxes.resize(0);
  _slackbuilds.resize(0);
  _installedlist.resize(0);
  _categories.resize(0);
  _title = "sboui Development Version";
  _filter = "all SlackBuilds";
  _info = "f: Filter | s: Search | o: Options | F1: Help";
  _category_idx = 0;
  _activated_listbox = 0;
}

MainWindow::~MainWindow()
{
  if (_win1) { delwin(_win1); }
  if (_win2) { delwin(_win2); }
}

/*******************************************************************************

First time window setup

*******************************************************************************/
int MainWindow::initialize()
{
  BuildListBox initlistbox;
  int retval;

  // Create windows (note: geometry gets set in redrawWindows);

  _win1 = newwin(4, 0, 10, 10);
  _win2 = newwin(4, 11,10, 10);

  _clistbox.setWindow(_win1);
  _clistbox.setActivated(true);
  _clistbox.setName("Groups");
  initlistbox.setWindow(_win2);
  initlistbox.setActivated(false);
  initlistbox.setName("SlackBuilds");
  _blistboxes.push_back(initlistbox);

  redrawAll();

  printStatus("Reading SlackBuilds repository ...");
  retval = readLists();
  if (retval != 0) { printStatus("Error reading SlackBuilds repository."); }
  else { filterAll(); }

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

Dialogs

*******************************************************************************/
void MainWindow::selectFilter()
{
  WINDOW *filterwin;
  std::string selection, selected;
  bool getting_selection;

  // Set up window

  filterwin = newwin(10, 10, 4, 4);
  _fbox.setWindow(filterwin);
  placePopup(&_fbox, filterwin);

  // Get filter selection

  getting_selection = true;
  while (getting_selection)
  {
    selection = _fbox.exec();
    getting_selection = false;
    if (selection == signals::keyEnter)
    {
      selected = _fbox.highlightedItem()->name();
      if ( (selected == "All") &&
           (_filter != "all SlackBuilds") ){ filterAll(); } 
      else if ( (selected == "Installed") && 
                (_filter != "installed SlackBuilds") ) { filterInstalled(); }
      else if ( (selected == "Upgradable") && 
                (_filter != "upgradable SlackBuilds") ) { filterUpgradable(); } 
      else if ( (selected == "Non-dependencies") && 
                (_filter != "non-dependencies") ) { filterNonDeps(); } 
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

  // Get rid of window

  wclear(filterwin);
  delwin(filterwin);

  // Redraw

  redrawAll();
}

void MainWindow::search()
{
  WINDOW *searchwin;
  std::string searchterm;
  bool getting_input;

  // Set up window and search box

  searchwin = newwin(10, 10, 4, 4);
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

  // Get rid of window

  wclear(searchwin);
  delwin(searchwin);

  // Redraw

  redrawAll();
}

void MainWindow::showBuildActions(const BuildListItem & build)
{
  WINDOW *actionwin;
  std::string selection;
  bool getting_selection;
  BuildActionBox actionbox;

  // Set up window and dialog

  actionwin = newwin(10, 10, 4, 4);
  actionbox.setWindow(actionwin);
  placePopup(&actionbox, actionwin);

  // Get filter selection

  getting_selection = true;
  while (getting_selection)
  {
    selection = actionbox.exec();
    getting_selection = false;
    if (selection == signals::resize)
    {
      getting_selection = true;
      placePopup(&_fbox, actionwin);
      redrawAll(true);
      clearStatus();
      _fbox.draw(true);
    }
  }

  // Get rid of window

  wclear(actionwin);
  delwin(actionwin);

  // Redraw

  redrawAll();
}

/*******************************************************************************

Shows the main window

*******************************************************************************/
void MainWindow::show()
{
  std::string selection, statusmsg;
  bool getting_input, all_tagged;
  ListItem *build;

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

        build = _blistboxes[_category_idx].highlightedItem();
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

        build = _blistboxes[_category_idx].highlightedItem();
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
    }

    // Key signals with the same action w/ either type of list box

    if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      redrawAll(); 
      clearStatus();
    }
    else if (selection == "f") { selectFilter(); }
    else if (selection == "s") { search(); }
  }
}
