#include <vector>
#include <string>
#include <cmath>     // floor
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "string_util.h"
#include "signals.h"
#include "backend.h"
#include "CursesWidget.h"
#include "CategoryListItem.h"
#include "CategoryListBox.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "filters.h"
#include "FilterBox.h"
#include "SearchBox.h"
#include "BuildActionBox.h"
#include "BuildOrderBox.h"
#include "InvReqBox.h"
#include "InstallBox.h"
#include "DirListBox.h"
#include "MessageBox.h"
#include "TagList.h"
#include "OptionsWindow.h"
#include "HelpWindow.h"
#include "MainWindow.h"

#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION ""
#endif

using namespace color_settings;

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

Prints a given number of spaces

*******************************************************************************/
void MainWindow::printSpaces(int nspaces) const
{
  int i;

  for ( i = 0; i < nspaces; i++ ) { addch(' '); }
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
  int rows, cols, namelen, left;
  double mid;

  getmaxyx(stdscr, rows, cols);

  // Draw title

  namelen = _title.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0) + 1;
  move(0, 0);
  clrtoeol();
  colors.turnOn(stdscr, fg_title, bg_title);
  printSpaces(left-1);
  printToEol(_title);
  colors.turnOff(stdscr);

  // Print filter selection

  move(2, 0);
  clrtoeol();
  printToEol("Showing: " + _filter);

  // Draw footer

  namelen = _info.size();
  left = std::floor(mid - double(namelen)/2.0) + 1;
  move(rows-1, 0);
  clrtoeol();
  colors.turnOn(stdscr, fg_info, bg_info);
  printSpaces(left-1);
  printToEol(_info);
  colors.turnOff(stdscr);

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

  if (settings::layout == "vertical") { redrawWindowsVert(); }
  else { redrawWindowsHorz(); }

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
  if (settings::layout == "horizontal") { settings::layout = "vertical"; }
  else { settings::layout = "horizontal"; }

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
  _taglist.clearList();
  _category_idx = 0;
  _activated_listbox = 0;
}

/*******************************************************************************

Displays all SlackBuilds

*******************************************************************************/
void MainWindow::filterAll()
{
  unsigned int nbuilds;
  int choice;

  _filter = "all SlackBuilds";
  printStatus("Filtering by all SlackBuilds ...");

  _activated_listbox = 0;
  _category_idx = 0;
  nbuilds = _slackbuilds.size();

  filter_all(_slackbuilds, _categories, _win2, _clistbox, _blistboxes);

  if (nbuilds == 0)
  {
    clearStatus();
    choice = displayError("Repository is empty. Run the sync command now?",
                          "Error", "Enter: Yes | Esc: No");
    if (choice == 1) { syncRepo(); }
  } 
  else if (nbuilds == 1)
    printStatus("1 SlackBuild in repository.");
  else 
    printStatus(int_to_string(nbuilds) + " SlackBuilds in repository.");
}

/*******************************************************************************

Displays installed SlackBuilds

*******************************************************************************/
void MainWindow::filterInstalled()
{
  unsigned int ninstalled;

  _filter = "installed SlackBuilds";
  printStatus("Filtering by installed SlackBuilds ...");

  if (_installedlist.size() == 0) 
    list_installed(_slackbuilds, _installedlist); 

  _activated_listbox = 0;
  _category_idx = 0;
  ninstalled = _installedlist.size();

  filter_installed(_installedlist, _categories, _win2, _clistbox, _blistboxes);

  if (ninstalled == 0) 
    printStatus("No installed SlackBuilds.");
  else if (ninstalled == 1) 
    printStatus("1 installed SlackBuild.");
  else 
    printStatus(int_to_string(ninstalled) + " installed SlackBuilds.");
}

/*******************************************************************************

Displays upgradable SlackBuilds

*******************************************************************************/
void MainWindow::filterUpgradable()
{
  unsigned int nupgradable;

  _filter = "upgradable SlackBuilds";
  printStatus("Filtering by upgradable SlackBuilds ...");
  if (_installedlist.size() == 0) 
    list_installed(_slackbuilds, _installedlist); 

  _category_idx = 0;
  _activated_listbox = 0;
  nupgradable = 0;

  filter_upgradable(_installedlist, _categories, _win2, _clistbox, _blistboxes,
                    nupgradable);

  if (nupgradable == 0) 
    printStatus("No upgradable SlackBuilds."); 
  else if (nupgradable == 1) 
    printStatus("1 upgradable SlackBuild.");
  else 
    printStatus(int_to_string(nupgradable) + " upgradable SlackBuilds.");
}

/*******************************************************************************

Displays tagged SlackBuilds

*******************************************************************************/
void MainWindow::filterTagged()
{
  unsigned int ntagged;

  _filter = "tagged SlackBuilds";
  printStatus("Filtering by tagged SlackBuilds ...");

  _category_idx = 0;
  _activated_listbox = 0;
  ntagged = 0;

  filter_tagged(_slackbuilds, _categories, _win2, _clistbox, _blistboxes, 
                ntagged);

  if (ntagged == 0) 
    printStatus("No tagged SlackBuilds."); 
  else if (ntagged == 1) 
    printStatus("1 tagged SlackBuild.");
  else 
    printStatus(int_to_string(ntagged) + " tagged SlackBuilds.");
}

/*******************************************************************************

Displays installed SlackBuilds that are not a dependency of any other installed
SlackBuild

*******************************************************************************/
void MainWindow::filterNonDeps()
{
  unsigned int nnondeps;

  _filter = "non-dependencies";
  printStatus("Filtering by non-dependencies ...");

  if (_installedlist.size() == 0) 
    list_installed(_slackbuilds, _installedlist); 
  if (_nondeplist.size() == 0)
    list_nondeps(_installedlist, _nondeplist);

  _category_idx = 0;
  _activated_listbox = 0;
  nnondeps = _nondeplist.size();

  filter_nondeps(_nondeplist, _categories, _win2, _clistbox, _blistboxes);

  if (nnondeps == 0) 
    printStatus("No non-dependencies."); 
  else if (nnondeps == 1) 
    printStatus("1 non-dependency.");
  else 
    printStatus(int_to_string(nnondeps) + " non-dependencies.");
}

/*******************************************************************************

Filters SlackBuilds by search term

*******************************************************************************/
void MainWindow::filterSearch(const std::string & searchterm, 
                              bool case_sensitive, bool whole_word)
{
  unsigned int nsearch;

  _filter = "search for " + searchterm;
  printStatus("Searching for " + searchterm + " ...");

  _activated_listbox = 0;
  _category_idx = 0;
  nsearch = 0;

  filter_search(_slackbuilds, _categories, _win2, _clistbox, _blistboxes,
                nsearch, searchterm, case_sensitive, whole_word);

  if (nsearch == 0)
    printStatus("No matches for " + searchterm + ".");
  else if (nsearch == 1) 
    printStatus("1 match for " + searchterm + ".");
  else 
    printStatus(int_to_string(nsearch) + " matches for " + searchterm + ".");
}

/*******************************************************************************

Shows options window

*******************************************************************************/
int MainWindow::showOptions()
{
  WINDOW *optionswin;
  std::string selection;
  bool getting_input;

  clear();
  setInfo("Enter: Apply settings | Esc: Back to main");
  redrawHeaderFooter();

  optionswin = newwin(1, 1, 0, 0);
  _options.setWindow(optionswin);
  _options.placeWindow();
  _options.readSettings();

  getting_input = true;
  while (getting_input)
  {
    selection = _options.exec(); 
    if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      clear();
      redrawHeaderFooter();
      _options.placeWindow();
    }
    else if (selection == "q") { return 1; }
    else if (selection == signals::keyEnter) 
    {
      getting_input = false;
      _options.applySettings();
    }
  }

  clearStatus();
  setInfo("f: Filter | /: Search | o: Options | ?: Help");
  delwin(optionswin);
  redrawAll(true);
  return 0;
}

/*******************************************************************************

Shows help window

*******************************************************************************/
int MainWindow::showHelp()
{
  WINDOW *helpwin;
  std::string selection;
  bool getting_input;

  clear();
  setInfo("Esc: Back to main");
  redrawHeaderFooter();

  helpwin = newwin(1, 1, 0, 0);
  _help.setWindow(helpwin);
  _help.placeWindow();

  getting_input = true;
  while (getting_input)
  {
    selection = _help.exec(); 
    if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      clear();
      redrawHeaderFooter();
      _help.placeWindow();
    }
    else if (selection == "q") { return 1; }
  }

  clearStatus();
  setInfo("f: Filter | /: Search | o: Options | ?: Help");
  delwin(helpwin);
  redrawAll(true);
  return 0;
}

/*******************************************************************************

Sets taglist reference in BuildListBoxes

*******************************************************************************/
void MainWindow::setTagList()
{
  unsigned int nblistboxes, i;

  nblistboxes = _blistboxes.size();
  for ( i = 0; i < nblistboxes; i++ ) { _blistboxes[i].setTagList(&_taglist); }
}

/*******************************************************************************

Installs/upgrades/reinstalls/removes SlackBuild and dependencies. Returns true
if anything was changed, false otherwise.

*******************************************************************************/
bool MainWindow::modifyPackage(BuildListItem & build,
                               const std::string & action, bool recheck)
{
  WINDOW *installerwin;
  int check, nchanged, response;
  std::string selection;
  bool getting_input, needs_rebuild;
  unsigned int ninstaller;
  InstallBox installer;

  if (settings::resolve_deps)
    printStatus("Computing dependencies for " + build.name() + " ...");
  check = installer.create(build, _slackbuilds, action, settings::resolve_deps,
                           recheck);

  if (check != 0) 
  { 
    clearStatus();
    displayError("Unable to find one or more dependencies of " + build.name() +
                 std::string(" in repository. Disable dependency resolution ") +
                 std::string("to ignore this error."));
    return false;
  }

  ninstaller = installer.numItems();
  if (settings::resolve_deps)
  {
    if (ninstaller == 2)
    { 
      if (action == "Remove")
        printStatus("1 installed dependency for " + build.name() + ".");
      else
        printStatus("1 dependency for " + build.name() + ".");
    }
    else if (ninstaller >= 1)
    { 
      if (action == "Remove")
        printStatus(int_to_string(ninstaller-1) + 
                    " installed dependencies for " + build.name() + ".");
      else
        printStatus(int_to_string(ninstaller-1) + 
                    " dependencies for " + build.name() + ".");
    }
    else
    {
      /* The list can possibly be empty when removing tagged SlackBuilds.
         In this case, there's nothing left to do. */

      clearStatus();
      return false;
    }
  }

  // Show list of changes to apply and/or apply changes

  needs_rebuild = false;
  response = 0;
  if (settings::confirm_changes)
  {
    installerwin = newwin(1, 1, 0, 0);
    installer.setWindow(installerwin);
    placePopup(&installer, installerwin);

    getting_input = true;
    while (getting_input)
    {
      selection = installer.exec(); 
      if (selection == signals::keyEnter)
      {
        getting_input = false;
        response = 1;
        if (! installer.installingAllDeps())
        {
          response = displayError("You have chosen to skip some dependencies." +
                                  std::string(" Continue anyway? "), "Warning",
                                  "Enter: Yes | Esc: No"); 
        }
      }
      else if (selection == signals::quit) { getting_input = false; }
      else if (selection == signals::resize) 
      { 
        placePopup(&installer, installerwin);
        redrawAll(true);
      }
    }
    delwin(installerwin);
  }
  else { response = 1; }

  // Apply changes

  if (response == 1)
  {
    def_prog_mode();
    endwin();
    check = installer.applyChanges(nchanged, settings::confirm_changes);
    if (nchanged > 0) { needs_rebuild = true; }
    reset_prog_mode();
    redrawAll(true);
    if (check != 0)
      displayError("One or more requested changes was not applied.");
  }

  clearStatus();

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
  buildorder.setTagList(&_taglist);

  if (check != 0) 
  { 
    clearStatus();
    displayError("Unable to find one or more dependencies of " + build.name() +
                 std::string(" in repository. Build order will be incomplete."),
                 "Warning");
  }

  nbuildorder = buildorder.numItems();

  if (nbuildorder == 1) { printStatus(
                     "1 SlackBuild in build order for " + build.name() + "."); }
  else { printStatus(int_to_string(nbuildorder) + 
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
  invreqs.setTagList(&_taglist);

  ninvreqs = invreqs.numItems();
  if (ninvreqs == 0) { printStatus("No installed SlackBuilds depend on "
                                   + build.name() + "."); }
  else if (ninvreqs == 1) { printStatus(
          "1 installed SlackBuild depends on " + build.name() + "."); }
  else { printStatus(int_to_string(ninvreqs) + 
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

  builddir = settings::repo_dir + "/" + build.getProp("category") + "/" +
             build.name();
  check = browser.setDirectory(builddir);

  if (check != 0) 
  { 
    displayError("Unable to access build directory for " + build.name() + ".");
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
        check = view_file(builddir + "/" + fname);
        reset_prog_mode();
        redrawAll(true);
      }
      else { displayError("Can only view files and symlinks."); }
    }
    else if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&browser, browserwin);
      redrawAll(true);
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

  check = sync_repo();
  reset_prog_mode();

  if (check == 0) 
  {
    clearData();
    initialize();
  }
  else 
    displayError("Sync command failed. Ensure package manager is "
                 + std::string("installed and network is connected."));

  return check;
}

/*******************************************************************************

Applies action to tagged SlackBuilds

*******************************************************************************/
void MainWindow::applyTags(const std::string & action)
{
  WINDOW *tagwin;
  unsigned int ndisplay, i;
  bool getting_input, apply_changes, any_modified, needs_rebuild;
  std::string selection;
  BuildListItem item;

  ndisplay = _taglist.getDisplayList(action);
  if (ndisplay == 0)
  {
    displayError("No tagged SlackBuilds to " + string_to_lower(action) + ".");
    return;
  }
  else if (ndisplay == 1)
    printStatus("1 tagged SlackBuild to " + string_to_lower(action) + ".");
  else
    printStatus(int_to_string(ndisplay) +
                " tagged SlackBuild to " + string_to_lower(action) + ".");

  tagwin = newwin(1, 1, 0, 0);
  _taglist.setWindow(tagwin);
  _taglist.setName("SlackBuilds to " + string_to_lower(action));
  placePopup(&_taglist, tagwin);

  // Show list of tagged SlackBuilds to install/upgrade/remove/reinstall

  getting_input = true;
  while (getting_input)
  {
    selection = _taglist.exec(); 
    if (selection == signals::keyEnter) 
    { 
      apply_changes = true; 
      getting_input = false;
    }
    else if (selection == signals::quit) 
    { 
      apply_changes = false; 
      getting_input = false;
    }
    else if (selection == signals::resize) 
    { 
      placePopup(&_taglist, tagwin);
      redrawAll(true);
    }
  }

  clearStatus();
  delwin(tagwin);
  redrawAll(true);

  // Apply changes

  needs_rebuild = false;
  if (apply_changes)
  {
    for ( i = 0; i < ndisplay; i++ ) 
    {
      item = _taglist.itemByIdx(i);
      if (item.getBoolProp("tagged"))
      { 
        any_modified = modifyPackage(item, action, true);
        if (! needs_rebuild) { needs_rebuild = any_modified; }
      }
      redrawAll(true);
    }

    // Rebuild lists if SlackBuilds were installed/upgraded/reinstalled/removed

    if (needs_rebuild)
    { 
      clearData();
      initialize();
    }
  } 
}

/*******************************************************************************

Sets size and position of popup boxes

*******************************************************************************/
void MainWindow::popupSize(int & height, int & width, CursesWidget *popup) const
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

void MainWindow::placePopup(CursesWidget *popup, WINDOW *win) const
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

Displays an error message. Returns 1 for Enter/Ok, 0 for Esc/Cancel.

*******************************************************************************/
int MainWindow::displayError(const std::string & msg, const std::string & name,
                             const std::string & info)
{
  std::string selection;
  bool getting_selection;
  int response;
  MessageBox errbox;
  WINDOW *errwin;

  // Place message box

  errwin = newwin(1, 1, 0, 0);
  errbox.setWindow(errwin);
  errbox.setName(name);
  errbox.setMessage(msg);
  errbox.setInfo(info);
  placePopup(&errbox, errwin);
  redrawAll(true);

  // Get user input

  getting_selection = true;
  while (getting_selection)
  {
    selection = errbox.exec();
    getting_selection = false;
    if (selection == signals::resize)
    {
      getting_selection = true;
      placePopup(&errbox, errwin);
      redrawAll(true);
    }
    else if (selection == signals::keyEnter) { response = 1; }
    else { response = 0; }
  }

  // Get rid of window

  delwin(errwin);
  redrawAll(true);

  return response;
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
  _title = "sboui " PACKAGE_VERSION;
  _filter = "all SlackBuilds";
  _info = "f: Filter | /: Search | o: Options | ?: Help";
  _status = "";
  _category_idx = 0;
  _activated_listbox = 0;
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

  if (retval != 0) 
  { 
    clearStatus();
    displayError("Error reading SlackBuilds repository."); 
  }
  else 
  { 
    if (_filter == "installed SlackBuilds") { filterInstalled(); }
    else if (_filter == "upgradable SlackBuilds") { filterUpgradable(); }
    else if (_filter == "tagged SlackBuilds") { filterTagged(); }
    else if (_filter == "non-dependencies") { filterNonDeps(); }
    else { filterAll(); }
  }
  redrawAll(true);
  setTagList();

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
      _fbox.draw(true);
    }
  }
  setTagList();

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
  std::string selection;
  bool getting_input;

  // Set up window and search box

  searchwin = newwin(1, 1, 0, 0);
  _searchbox.setWindow(searchwin);
  placePopup(&_searchbox, searchwin);

  // Get search term from user

  getting_input = true;
  while (getting_input)
  {
    _searchbox.clearSearch();
    selection = _searchbox.exec();
    if (selection == signals::resize)
    {
      placePopup(&_searchbox, searchwin);
      redrawAll(true);
      _searchbox.draw(true);
    }
    else if (selection == signals::keyEnter)
    { 
      getting_input = false;
      if (_searchbox.searchString().size() > 0)
      {
        filterSearch(_searchbox.searchString(), _searchbox.caseSensitive(),
                     _searchbox.wholeWord());
      }
      setTagList();
    }
    else if (selection == signals::quit) { getting_input = false; }
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
  BuildActionBox actionbox;

  // Set up windows and dialog

  actionwin = newwin(1, 1, 0, 0);
  actionbox.setWindow(actionwin);
  actionbox.setName("Actions for " + build.name());
  actionbox.create(build);
  placePopup(&actionbox, actionwin);

  // Get selection

  getting_selection = true;
  needs_rebuild = false;
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
      view_readme(build); 
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
      actionbox.draw(true);
    }
    else { getting_selection = false; }
  }

  // Get rid of window (redraw happens in MainWindow::show())

  delwin(actionwin);

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
  int check_quit;
  unsigned int i, ncategories;
  BuildListItem build;
  ListItem *item;

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

      // Tab signal or right key

      else if ( (selection == signals::keyTab) ||
                (selection == signals::keyRight) )
      {
        _clistbox.setActivated(false);
        _clistbox.draw();
        _blistboxes[_category_idx].setActivated(true);
        _activated_listbox = 1;

        // Display status message for installed SlackBuild

        build = *_blistboxes[_category_idx].highlightedItem();
        if (build.getBoolProp("installed"))
        {
          statusmsg = "Installed: " + build.getProp("installed_version") +
            " -> Available: " + build.getProp("available_version");
          printStatus(statusmsg);
        }
        else { clearStatus(); }
      }

      // Tag signal: tag/untag all items in category

      else if ( (selection == "t") || (selection == "T") )
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

        build = *_blistboxes[_category_idx].highlightedItem();
        if (build.getBoolProp("installed"))
        {
          statusmsg = "Installed: " + build.getProp("installed_version") +
            " -> Available: " + build.getProp("available_version");
          printStatus(statusmsg);
        }
        else { clearStatus(); }
      }

      // Tab signal or left key
      
      else if ( (selection == signals::keyTab) ||
                (selection == signals::keyLeft) )
      {
        _blistboxes[_category_idx].setActivated(false);
        _blistboxes[_category_idx].draw();
        _clistbox.setActivated(true);
        _activated_listbox = 0;
        clearStatus();
      }

      // Tag signal: see if we need to change tag for category

      else if ( (selection == "t") || (selection == "T") )
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
        build = *_blistboxes[_category_idx].highlightedItem();
        showBuildActions(build);

        // Update tag setting since we are working with a local copy

        if ( (item = _blistboxes[_category_idx].highlightedItem()) )
          item->setBoolProp("tagged", build.getBoolProp("tagged"));

        // Determine if categories should be tagged and redraw

        ncategories = _clistbox.numItems();
        for ( i = 0; i < ncategories; i++ )
        {
          if (_blistboxes[i].allTagged())
            _clistbox.itemByIdx(i)->setBoolProp("tagged", true);
        }
        redrawAll(true);
      }
    }

    // Key signals with the same action w/ either type of list box

    if (selection == "q") { getting_input = false; }
    else if (selection == signals::resize) { redrawAll(true); }
    else if (selection == "f") { selectFilter(); }
    else if (selection == "/") { search(); }
    else if (selection == "s") { syncRepo(); }
    else if (selection == "o") 
    {
      check_quit = showOptions();
      if (check_quit == 1) { getting_input = false; }
    }
    else if (selection == "?") 
    {
      check_quit = showHelp();
      if (check_quit == 1) { getting_input = false; }
    }
    else if (selection == "l") { toggleLayout(); }
    else if (selection == "i") { applyTags("Install"); }
    else if (selection == "u") { applyTags("Upgrade"); }
    else if (selection == "r") { applyTags("Remove"); }
    else if (selection == "e") { applyTags("Reinstall"); }
  }
}
