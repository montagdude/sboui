#include <vector>
#include <string>
#include <iostream>
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
#include "BuildOptionsBox.h"
#include "BuildOrderBox.h"
#include "InvReqBox.h"
#include "InstallBox.h"
#include "DirListBox.h"
#include "MessageBox.h"
#include "TagList.h"
#include "OptionsWindow.h"
#include "HelpWindow.h"
#include "QuickSearch.h"
#include "PackageInfoBox.h"
#include "MainWindow.h"

/*******************************************************************************

Prints/clears status message

*******************************************************************************/
void MainWindow::printStatus(const std::string & msg, bool bold)
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  if (bold) { attron(A_BOLD); }
  printToEol(msg, cols);
  if (bold) { attroff(A_BOLD); }
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
  colors.turnOn(stdscr, "fg_title", "bg_title");
  printSpaces(left-1);
  printToEol(_title, cols-(left-1));
  colors.turnOff(stdscr);

  // Print filter selection

  move(2, 0);
  clrtoeol();
  printToEol("Showing: " + _filter, cols);

  // Draw footer

  namelen = _info.size();
  left = std::floor(mid - double(namelen)/2.0) + 1;
  move(rows-1, 0);
  clrtoeol();
  colors.turnOn(stdscr, "fg_info", "bg_info");
  printSpaces(left-1);
  printToEol(_info, cols-(left-1));
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

Toggles horizontal/vertical layout

*******************************************************************************/
void MainWindow::toggleLayout()
{
  if (settings::layout == "horizontal") { settings::layout = "vertical"; }
  else { settings::layout = "horizontal"; }

  draw(true);
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
  _categories.resize(0);
  _taglist.clearList();
  _category_idx = 0;
  _activated_listbox = 0;
}

/*******************************************************************************

Creates master list of SlackBuilds

*******************************************************************************/
int MainWindow::readLists()
{
  int check;
  unsigned int i, ncategories, npkgerr, nmissing; 
  std::vector<std::string> pkg_errors, missing_info;
  std::string errmsg;

  // Get list of SlackBuilds

  check = read_repo(_slackbuilds); 
  if (check != 0) { return check; }

  // Create list of categories

  ncategories = _slackbuilds.size();
  for ( i = 0; i < ncategories; i++ )
  { 
    CategoryListItem citem;
    citem.setName(_slackbuilds[i][0].getProp("category"));
    citem.setProp("category", _slackbuilds[i][0].getProp("category"));
    _categories.push_back(citem);
  }

  // Determine which are installed and get other info

  determine_installed(_slackbuilds, pkg_errors, missing_info);

  // Read build options

  if (settings::save_buildopts)
    read_buildopts(_slackbuilds);

  // Warning for invalid package names

  npkgerr = pkg_errors.size();
  if ( (npkgerr > 0) && (settings::warn_invalid_pkgnames) )
  {
    errmsg = "The following installed packages have invalid names "
           + std::string("and were ignored:\n");
    for ( i = 0; i < npkgerr; i++ ) { errmsg += "\n" + pkg_errors[i]; }
    errmsg += "\n\nThis warning may be disabled by setting "
           +  std::string("warn_invalid_pkgnames = false.");
    displayError(errmsg, true, "Warning");
  }

  // Warning for missing info files

  nmissing = missing_info.size();
  if (nmissing > 0)
  {
    errmsg = "The following installed SlackBuilds are missing .info files:\n";
    for ( i = 0; i < nmissing; i++ ) { errmsg += "\n" + missing_info[i]; }
    errmsg += "\n\nYou should run the sync command to fix this problem.";
    displayError(errmsg);
  }

  return 0;
}

/*******************************************************************************

Rebuilds lists after applying changes

*******************************************************************************/
void MainWindow::rebuild()
{
  unsigned int k, ncategories, list_highlight, prev_activated;
  int ntagged;
  std::vector<std::string> pkg_errors, missing_info;
  std::string errmsg, category;
  BuildListItem *build;

  // Clear tags

  ntagged = _taglist.numTagged();
  for ( k = 0; int(k) < ntagged; k++ )
  {
    build = static_cast<BuildListItem *>(_taglist.taggedByIdx(k));
    build->setBoolProp("tagged", false);
  }
  _taglist.clearList();

  // Un-tag all categories

  ncategories = _clistbox.numItems();
  for ( k = 0; k < ncategories; k++ )
  {
    _clistbox.itemByIdx(k)->setBoolProp("tagged", false);
  }

  // Save original highlight info

  category = _clistbox.highlightedName();
  list_highlight = _blistboxes[_category_idx].highlight();
  prev_activated = _activated_listbox;

  // Re-filter (data, tags could have changed), unless filtered by search

  if (_filter == "all SlackBuilds") { filterAll(); }
  else if (_filter == "installed SlackBuilds") { filterInstalled(); }
  else if (_filter == "upgradable SlackBuilds") { filterUpgradable(); } 
  else if (_filter == "tagged SlackBuilds") { filterTagged(); } 
  else if (_filter == "blacklisted SlackBuilds") { filterBlacklisted(); }
  else if (_filter == "non-dependencies") { filterNonDeps(); } 
  else if (_filter == "SlackBuilds with build options set")
    filterBuildOptions();

  // Reset original highlight if possible

  if (_clistbox.setHighlight(category) == 0)
  {
    _category_idx = _clistbox.highlight();
    _blistboxes[_category_idx].setHighlight(list_highlight);
    if (prev_activated == 0)
    {
      _clistbox.setActivated(true);
      _blistboxes[_category_idx].setActivated(false);
      _activated_listbox = 0;
    }
    else
    {
      _clistbox.setActivated(false);
      _blistboxes[_category_idx].setActivated(true);
      _activated_listbox = 1;
    }
  }

  draw(true);
}

/*******************************************************************************

Displays all SlackBuilds

*******************************************************************************/
void MainWindow::filterAll()
{
  unsigned int nbuilds;
  std::string choice;
  bool (*func)(const BuildListItem &);

  _filter = "all SlackBuilds";
  printStatus("Filtering by all SlackBuilds ...");

  _activated_listbox = 0;
  _category_idx = 0;

  func = &any_build;
  filter_by_func(_slackbuilds, func, _categories, _win2, _clistbox,
                 _blistboxes, nbuilds);

  if (nbuilds == 0)
  {
    clearStatus();
    choice = displayError("Repository is empty. Run the sync command now?",
                          true, "Error", "Enter: Yes | Esc: No");
    if (choice == signals::keyEnter) { syncRepo(); }
  } 
  else if (nbuilds == 1)
    printStatus("1 SlackBuild in repository.");
  else 
    printStatus(int_to_string(nbuilds) + " SlackBuilds in repository.");

  setTagList();
}

/*******************************************************************************

Displays installed SlackBuilds

*******************************************************************************/
void MainWindow::filterInstalled()
{
  unsigned int ninstalled;
  std::vector<std::string> pkg_errors, missing_info;
  bool (*func)(const BuildListItem &);

  _filter = "installed SlackBuilds";
  printStatus("Filtering by installed SlackBuilds ...");

  _activated_listbox = 0;
  _category_idx = 0;

  func = &build_is_installed;
  filter_by_func(_slackbuilds, func, _categories, _win2, _clistbox,
                 _blistboxes, ninstalled);

  if (ninstalled == 0) 
    printStatus("No installed SlackBuilds.");
  else if (ninstalled == 1) 
    printStatus("1 installed SlackBuild.");
  else 
    printStatus(int_to_string(ninstalled) + " installed SlackBuilds.");

  setTagList();
}

/*******************************************************************************

Displays upgradable SlackBuilds

*******************************************************************************/
void MainWindow::filterUpgradable()
{
  unsigned int nupgradable;
  std::vector<std::string> pkg_errors, missing_info;
  bool (*func)(const BuildListItem &);

  _filter = "upgradable SlackBuilds";
  printStatus("Filtering by upgradable SlackBuilds ...");

  _category_idx = 0;
  _activated_listbox = 0;
  nupgradable = 0;

  func = &build_is_upgradable;
  filter_by_func(_slackbuilds, func, _categories, _win2, _clistbox,
                 _blistboxes, nupgradable);

  if (nupgradable == 0) 
    printStatus("No upgradable SlackBuilds."); 
  else if (nupgradable == 1) 
    printStatus("1 upgradable SlackBuild.");
  else 
    printStatus(int_to_string(nupgradable) + " upgradable SlackBuilds.");

  setTagList();
}

/*******************************************************************************

Displays tagged SlackBuilds

*******************************************************************************/
void MainWindow::filterTagged()
{
  unsigned int ntagged;
  bool (*func)(const BuildListItem &);

  _filter = "tagged SlackBuilds";
  printStatus("Filtering by tagged SlackBuilds ...");

  _category_idx = 0;
  _activated_listbox = 0;
  ntagged = 0;

  func = &build_is_tagged;
  filter_by_func(_slackbuilds, func, _categories, _win2, _clistbox,
                 _blistboxes, ntagged);

  if (ntagged == 0) 
    printStatus("No tagged SlackBuilds."); 
  else if (ntagged == 1) 
    printStatus("1 tagged SlackBuild.");
  else 
    printStatus(int_to_string(ntagged) + " tagged SlackBuilds.");

  setTagList();
}

/*******************************************************************************

Displays blacklisted SlackBuilds

*******************************************************************************/
void MainWindow::filterBlacklisted()
{
  unsigned int nblacklisted;
  bool (*func)(const BuildListItem &);

  _filter = "blacklisted SlackBuilds";
  printStatus("Filtering by blacklisted SlackBuilds ...");

  _category_idx = 0;
  _activated_listbox = 0;
  nblacklisted = 0;

  func = &build_is_blacklisted;
  filter_by_func(_slackbuilds, func, _categories, _win2, _clistbox,
                 _blistboxes, nblacklisted);

  if (nblacklisted == 0) 
    printStatus("No blacklisted SlackBuilds."); 
  else if (nblacklisted == 1) 
    printStatus("1 blacklisted SlackBuild.");
  else 
    printStatus(int_to_string(nblacklisted) + " blacklisted SlackBuilds.");

  setTagList();
}

/*******************************************************************************

Displays installed SlackBuilds that are not a dependency of any other installed
SlackBuild

*******************************************************************************/
void MainWindow::filterNonDeps()
{
  unsigned int nnondeps;
  std::vector<std::string> pkg_errors, missing_info;

  _filter = "non-dependencies";
  printStatus("Filtering by non-dependencies ...");

  _category_idx = 0;
  _activated_listbox = 0;
  nnondeps = 0;

  filter_nondeps(_slackbuilds, _categories, _win2, _clistbox, _blistboxes,
                 nnondeps);

  if (nnondeps == 0) 
    printStatus("No non-dependencies."); 
  else if (nnondeps == 1) 
    printStatus("1 non-dependency.");
  else 
    printStatus(int_to_string(nnondeps) + " non-dependencies.");

  setTagList();
}

/*******************************************************************************

Displays SlackBuilds with build options set

*******************************************************************************/
void MainWindow::filterBuildOptions()
{
  unsigned int nbuildsopts;
  bool (*func)(const BuildListItem &);

  _filter = "SlackBuilds with build options set";
  printStatus("Filtering by SlackBuilds with build options set ...");

  _category_idx = 0;
  _activated_listbox = 0;
  nbuildsopts = 0;

  func = &build_has_buildoptions;
  filter_by_func(_slackbuilds, func, _categories, _win2, _clistbox,
                 _blistboxes, nbuildsopts);

  if (nbuildsopts == 0) 
    printStatus("No SlackBuilds with build options set."); 
  else if (nbuildsopts == 1) 
    printStatus("1 SlackBuild with build options set.");
  else 
    printStatus(int_to_string(nbuildsopts) +
                " SlackBuilds with build options set.");

  setTagList();
}

/*******************************************************************************

Filters SlackBuilds by search term

*******************************************************************************/
void MainWindow::filterSearch(const std::string & searchterm, 
                              bool case_sensitive, bool whole_word,
                              bool search_readmes)
{
  unsigned int nsearch;

  _filter = "search for " + searchterm;
  if (search_readmes)
    printStatus("Searching for " + searchterm + " in name and README ...");
  else
    printStatus("Searching for " + searchterm + " ...");

  _activated_listbox = 0;
  _category_idx = 0;
  nsearch = 0;

  filter_search(_slackbuilds, _categories, _win2, _clistbox, _blistboxes,
                nsearch, searchterm, case_sensitive, whole_word,
                search_readmes);

  if (nsearch == 0)
    printStatus("No matches for " + searchterm + ".");
  else if (nsearch == 1) 
    printStatus("1 match for " + searchterm + ".");
  else 
    printStatus(int_to_string(nsearch) + " matches for " + searchterm + ".");

  setTagList();
}

/*******************************************************************************

Shows options window

*******************************************************************************/
int MainWindow::showOptions()
{
  WINDOW *optionswin;
  std::string selection, errmsg, old_repo_dir, msg;
  bool getting_input;
  int check_color, check_write;

  clear();
  setInfo("Enter: Apply settings | Esc: Back to main");
  redrawHeaderFooter();

  optionswin = newwin(1, 1, 0, 0);
  _options.setWindow(optionswin);
  _options.placeWindow();
  _options.readSettings();

  getting_input = true;
  check_color = 0;
  check_write = 0;
  old_repo_dir = settings::repo_dir;
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
      _options.applySettings(check_color, check_write);
    }
  }

  if (check_color != 0)
  {
    switch (check_color) {
      case 1:
        errmsg = "Unable to read color theme file. Using default color theme.";
        break;
      case 2:
        errmsg = "Parse error in color theme file. Using default color theme.";
        break;
      case 3:
        errmsg = "Missing field in color theme file. "
               + std::string("Using default color theme.");
        break;
      case 4:
        errmsg = "Color is not supported in this terminal.";
        break;
    }
    displayError(errmsg);
  }
  if (check_write != 0)
  {
    switch (check_write) {
      case 1:
        errmsg = "Error reading $HOME environment variable.";
        break;
      case 2:
        errmsg = "Error writing ~/.sboui.conf.";
        break;
    }
    displayError(errmsg);
  }
  if ( (check_color == 0) && (check_write == 0) )
  {
    if (selection == signals::keyEnter)
    {
      if (settings::repo_dir != old_repo_dir)
        msg = "Settings were successfully applied.\n\nSince repo_dir was "
            + std::string("changed, please sync the repo using the s key ")
            + std::string("before continuing.");
      else
        msg = "Settings were successfully applied.";
      displayMessage(msg);
    }
  }

  clearStatus();
  setInfo("s: Sync | f: Filter | /: Search | o: Options | ?: Keys");
  delwin(optionswin);
  draw(true);
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
  setInfo("s: Sync | f: Filter | /: Search | o: Options | ?: Keys");
  delwin(helpwin);
  draw(true);
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
                               const std::string & action, int & ninstalled,
                               int & nupgraded, int & nreinstalled,
                               int & nremoved, bool & cancel_all, bool batch)
{
  WINDOW *installerwin;
  int check, nchanged_orig, nchanged_new, response;
  std::string selection, msg, choice;
  bool getting_input, needs_rebuild;
  unsigned int i, ndeps, nforeign;
  std::vector<const BuildListItem *> foreign;
  InstallBox installer;
  BuildListItem *subbuild;

  if (settings::resolve_deps)
    printStatus("Computing dependencies for " + build.name() + " ...");
  check = installer.create(build, _slackbuilds, action, settings::resolve_deps,
                           batch, settings::rebuild_inv_deps);
  installer.setTagList(&_taglist);

  if (check == 1) 
  { 
    clearStatus();
    displayError("Unable to find one or more dependencies of " + build.name() +
                 std::string(" in repository. Disable dependency resolution ") +
                 std::string("to ignore this error."));
    return false;
  }
  else if (check == 2)
  { 
    clearStatus();
    displayError("A .info file seems to be missing from the repository, so " +
                 std::string("build order is incomplete. Syncing may fix ") +
                 std::string("this problem."));
    return false;
  }

  ndeps = installer.numDeps();
  if (settings::resolve_deps)
  {
    if (ndeps == 1)
    { 
      if (action == "Remove")
        printStatus("1 installed dependency for " + build.name() + ".");
      else
        printStatus("1 dependency for " + build.name() + ".");
    }
    else if (ndeps >= 0)
    { 
      if (action == "Remove")
        printStatus(int_to_string(ndeps) + 
                    " installed dependencies for " + build.name() + ".");
      else
        printStatus(int_to_string(ndeps) + 
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
  cancel_all = false;
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
        // Warn about changing foreign packages

        foreign = installer.checkForeign();
        nforeign = foreign.size();
        if (nforeign > 0)
        {
          msg = "The following packages are being changed but appear to have "
              + std::string("been installed from a different repository:\n\n");
          for ( i = 0; i < nforeign; i++ )
          {
            msg += foreign[i]->getProp("package_name") + "\n";
          }
          msg += "\nContinue anyway?";
          choice = displayError(msg, true, "Warning",
                                "Enter: Continue | Esc: Edit");
          if (choice != signals::keyEnter) { continue; }
        }

        // Warn about skipping dependencies

        getting_input = false;
        response = 1;
        if ( (! installer.installingAllDeps()) &&
             (installer.installingRequested()) )
        {
          choice = displayError("You have chosen to skip some dependencies." +
                                std::string(" Continue anyway? "), true,
                                "Warning", "Enter: Yes | Esc: No"); 
          if (choice == signals::keyEnter) { response = 1; }
          else { response = 0; }
        }
      }
      else if (selection == signals::quit) { getting_input = false; }
      else if ( (selection == "c") && (batch) )
      {
        getting_input = false;
        cancel_all = true;
      }
      else if (selection == "a")
      {
        hideWindow(installerwin);
        draw(true);
        subbuild = static_cast<BuildListItem *>(installer.highlightedItem());
        showBuildActions(*subbuild, true);
        placePopup(&installer, installerwin);
        draw(true);
      }
      else if (selection == signals::resize) 
      { 
        placePopup(&installer, installerwin);
        draw(true);
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
    nchanged_orig = ninstalled + nupgraded + nreinstalled + nremoved;
    check = installer.applyChanges(ninstalled, nupgraded, nreinstalled,
                                   nremoved);
    nchanged_new = ninstalled + nupgraded + nreinstalled + nremoved;
    if (nchanged_new > nchanged_orig) { needs_rebuild = true; }
    reset_prog_mode();
    draw(true);
    if (check != 0)
      displayError("One or more requested changes was not applied.");
    else
      if ( (! batch) && (needs_rebuild) )
        displayMessage("All changes were successfully applied. Summary:\n\n"
             + std::string("Installed: ") + int_to_string(ninstalled) + "\n"
             + std::string("Upgraded: ") + int_to_string(nupgraded) + "\n"
             + std::string("Reinstalled: ") + int_to_string(nreinstalled) + "\n"
             + std::string("Removed: ") + int_to_string(nremoved), false);
  }
  clearStatus();

  return needs_rebuild;
}

/*******************************************************************************

Allows user to set build options for SlackBuild

*******************************************************************************/
void MainWindow::setBuildOptions(BuildListItem & build)
{
  WINDOW *buildoptionswin;
  std::string selection, msg;
  bool getting_input;
  BuildOptionsBox buildoptions;
  int check;

  printStatus("Setting build options for " + build.name() + " ...");
  buildoptions.setBuild(build);

  buildoptionswin = newwin(1, 1, 0, 0);
  buildoptions.setWindow(buildoptionswin);
  placePopup(&buildoptions, buildoptionswin);

  getting_input = true;
  while (getting_input)
  {
    selection = buildoptions.exec(); 
    if (selection == signals::keyEnter)
    {
      getting_input = false;
      build.setProp("build_options", buildoptions.entries());
      if (settings::save_buildopts)
      {
        check = buildoptions.write(build);
        if (check != 0)
        {
          msg = "Unable to save build options: "
              + std::string("cannot write to /var/lib/sboui/buildopts. ")
              + std::string("Please check permissions.");
          displayError(msg, true, "Warning", "Enter: Dismiss");
        }
      }
    }
    else if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&buildoptions, buildoptionswin);
      draw(true);
    }
  }

  clearStatus();
  delwin(buildoptionswin);
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
  BuildListItem *subbuild;

  printStatus("Computing build order for " + build.name() + " ...");
  check = buildorder.create(build, _slackbuilds);
  buildorder.setTagList(&_taglist);

  if (check == 1) 
  { 
    clearStatus();
    displayError("Unable to find one or more dependencies of " + build.name() +
                 std::string(" in repository. Build order will be incomplete."),
                 true, "Warning");
  }
  else if (check == 2)
  { 
    clearStatus();
    displayError("A .info file seems to be missing from the repository, so " +
                 std::string("build order will be incomplete. Syncing may ") +
                 std::string("fix this problem."), true, "Warning");
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
    else if (selection == "a")
    {
      hideWindow(buildorderwin);
      draw(true);
      subbuild = static_cast<BuildListItem *>(buildorder.highlightedItem());
      showBuildActions(*subbuild, true);
      placePopup(&buildorder, buildorderwin);
      draw(true);
      buildorder.draw(true);
    }
    else if (selection == signals::resize) 
    { 
      placePopup(&buildorder, buildorderwin);
      draw(true);
    }
  }

  clearStatus();
  delwin(buildorderwin);
}

/*******************************************************************************

Shows installed SlackBuilds depending on a given SlackBuild

*******************************************************************************/
void MainWindow::showInverseReqs(const BuildListItem & build)
{
  WINDOW *invreqwin;
  std::string selection;
  bool getting_input;
  unsigned int ninvreqs;
  InvReqBox invreqs;

  printStatus("Computing installed SlackBuilds depending on "
              + build.name() + " ...");
  invreqs.create(build, _slackbuilds);
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
      draw(true);
    }
  }

  clearStatus();
  delwin(invreqwin);
}

/*******************************************************************************

Shows package information about selected SlackBuild

*******************************************************************************/
void MainWindow::showPackageInfo(BuildListItem & build)
{
  std::string selection, msg;
  bool getting_selection;
  PackageInfoBox pkginfo;
  WINDOW *pkginfowin;

  // Make sure repo info is available

  if (! build.getBoolProp("installed")) { build.readPropsFromRepo(); }

  // Set up message

  msg =  "SlackBuild name: " + build.name() + "\n";
  if (build.getBoolProp("installed"))
  {
    msg += "Installed: yes\n";
    msg += "Installed version: " + build.getProp("installed_version")
        +  " (build " + build.getProp("installed_buildnum") + ")\n";
  }
  else { msg += "Installed: no\n"; }
  msg += "Available version: " + build.getProp("available_version")
       + " (build " + build.getProp("available_buildnum") + ")\n";
  if (build.getBoolProp("installed"))
  {
    msg += "Package name: " + build.getProp("package_name") + "\n";
    if (build.getBoolProp("blacklisted")) { msg += "Blacklisted: yes"; }
    else { msg += "Blacklisted: no"; }
  } 

  // Place message box

  pkginfowin = newwin(1, 1, 0, 0);
  pkginfo.setWindow(pkginfowin);
  pkginfo.setMessage(msg);
  placePopup(&pkginfo, pkginfowin);
  draw(true);

  // Get user input

  getting_selection = true;
  while (getting_selection)
  {
    selection = pkginfo.exec();
    getting_selection = false;
    if (selection == signals::resize)
    {
      getting_selection = true;
      placePopup(&pkginfo, pkginfowin);
      draw(true);
    }
  }

  // Get rid of window

  delwin(pkginfowin);
  draw(true);
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
  browser.limitTopDir(true);
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
        check = view_file(browser.directory() + fname);
        reset_prog_mode();
        draw(true);
      }
      else { displayError("Can only view files and symlinks."); }
    }
    else if (selection == signals::quit) { getting_input = false; }
    else if (selection == signals::resize) 
    { 
      placePopup(&browser, browserwin);
      draw(true);
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
  else { displayError("An error occurred while trying to sync."); }

  return check;
}

/*******************************************************************************

Applies action to tagged SlackBuilds

*******************************************************************************/
void MainWindow::applyTags(const std::string & action)
{
  WINDOW *tagwin;
  unsigned int ndisplay, i, j, ncategories;
  int ninstalled, nupgraded, nreinstalled, nremoved;
  bool getting_input, cancel_all, apply_changes, any_modified, needs_rebuild;
  std::string selection;
  BuildListItem *build;

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
      draw(true);
    }
  }

  clearStatus();
  delwin(tagwin);
  draw(true);

  // Apply changes

  needs_rebuild = false;
  if (apply_changes)
  {
    ninstalled = 0;
    nupgraded = 0;
    nreinstalled = 0;
    nremoved = 0;
    for ( i = 0; i < ndisplay; i++ ) 
    {
      build = static_cast<BuildListItem *>(_taglist.itemByIdx(i));
      if (build->getBoolProp("marked"))
      { 
        any_modified = modifyPackage(*build, action, ninstalled, nupgraded,
                                     nreinstalled, nremoved, cancel_all, true);
        if (! needs_rebuild) { needs_rebuild = any_modified; }

        // Because tags could have changed, determine if categories should be
        // tagged

        ncategories = _clistbox.numItems();
        for ( j = 0; j < ncategories; j++ )
        {
          if (_blistboxes[j].allTagged())
            _clistbox.itemByIdx(j)->setBoolProp("tagged", true);
          else { _clistbox.itemByIdx(j)->setBoolProp("tagged", false); }
        }
        draw(true);
      }
      if (cancel_all) { break; }
    }
    if (needs_rebuild)
      displayMessage("Summary of applied changes:\n\n"
           + std::string("Installed: ") + int_to_string(ninstalled) + "\n"
           + std::string("Upgraded: ") + int_to_string(nupgraded) + "\n"
           + std::string("Reinstalled: ") + int_to_string(nreinstalled) + "\n"
           + std::string("Removed: ") + int_to_string(nremoved), false);

    // Rebuild lists if SlackBuilds were installed/upgraded/reinstalled/removed

    if (needs_rebuild) { rebuild(); }
  } 
}

/*******************************************************************************

Drops to command line to view output

*******************************************************************************/
void MainWindow::viewCommandLine() const
{
  std::string response;

  def_prog_mode();
  endwin();
  std::cout << "Press Enter to return to main window ...";
  std::getline(std::cin, response);
  reset_prog_mode();
}

/*******************************************************************************

Displays an error message. Returns response from message box.

*******************************************************************************/
std::string MainWindow::displayError(const std::string & msg, bool centered,
                             const std::string & name, const std::string & info)
{
  std::string selection;
  bool getting_selection;
  MessageBox errbox(false, centered);
  WINDOW *errwin;

  // Place message box

  errwin = newwin(1, 1, 0, 0);
  errbox.setWindow(errwin);
  errbox.setName(name);
  errbox.setMessage(msg);
  errbox.setInfo(info);
  placePopup(&errbox, errwin);
  draw(true);

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
      draw(true);
    }
  }

  // Get rid of window

  delwin(errwin);
  draw(true);

  return selection;
}

/*******************************************************************************

Displays an info message. Returns response from message box.

*******************************************************************************/
std::string MainWindow::displayMessage(const std::string & msg, bool centered,
                             const std::string & name, const std::string & info)
{
  std::string selection;
  bool getting_selection;
  MessageBox msgbox(true, centered);
  WINDOW *msgwin;

  // Place message box

  msgwin = newwin(1, 1, 0, 0);
  msgbox.setWindow(msgwin);
  msgbox.setName(name);
  msgbox.setMessage(msg);
  msgbox.setInfo(info);
  msgbox.setColor(colors.getPair("fg_popup", "bg_popup"));
  placePopup(&msgbox, msgwin);
  draw(true);

  // Get user input

  getting_selection = true;
  while (getting_selection)
  {
    selection = msgbox.exec();
    getting_selection = false;
    if (selection == signals::resize)
    {
      getting_selection = true;
      placePopup(&msgbox, msgwin);
      draw(true);
    }
  }

  // Get rid of window

  delwin(msgwin);
  draw(true);

  return selection;
}

/*******************************************************************************

Prints package version information as status for installed SlackBuild.

*******************************************************************************/
void MainWindow::printPackageVersion(const BuildListItem & build)
{
  std::string statusmsg;

  if (build.getBoolProp("installed"))
  {
    if (build.getBoolProp("blacklisted"))
      statusmsg = "Installed: " + build.getProp("installed_version") +
        " (build " + build.getProp("installed_buildnum") + ") " +
        "-> Available: " + build.getProp("available_version") +
        " (build " + build.getProp("available_buildnum") + ") " +
        "(blacklisted)";
    else
      statusmsg = "Installed: " + build.getProp("installed_version") +
        " (build " + build.getProp("installed_buildnum") + ") " +
        "-> Available: " + build.getProp("available_version") +
        " (build " + build.getProp("available_buildnum") + ") ";
    printStatus(statusmsg);
  }
  else { clearStatus(); }
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
MainWindow::MainWindow(const std::string & version)
{
  _win1 = NULL;
  _win2 = NULL;
  _blistboxes.resize(0);
  _slackbuilds.resize(0);
  _categories.resize(0);
  _title = "sboui " + version;
  _filter = "all SlackBuilds";
  _info = "s: Sync | f: Filter | /: Search | o: Options | ?: Keys";
  _status = "";
  _category_idx = 0;
  _activated_listbox = 0;
  setWindow(stdscr);
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

  draw(true);

  // Read SlackBuilds repository

  printStatus("Reading SlackBuilds repository ...");
  retval = readLists();

  // Set filter

  if (retval != 0) 
  { 
    clearStatus();
    displayError("Error reading SlackBuilds repository. Please make sure that "
               + std::string("you have set repo_dir correctly in sboui.conf."));
  }
  else 
  { 
    if (_filter == "installed SlackBuilds") { filterInstalled(); }
    else if (_filter == "upgradable SlackBuilds") { filterUpgradable(); }
    else if (_filter == "tagged SlackBuilds") { filterTagged(); }
    else if (_filter == "blacklisted SlackBuilds") { filterBlacklisted(); }
    else if (_filter == "non-dependencies") { filterNonDeps(); }
    else if (_filter == "SlackBuilds with build options set")
      filterBuildOptions();
    else { filterAll(); }
  }
  draw(true);

  return retval;
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
    else if ( (selected == "Blacklisted") || (selection == "B") )
    {
      if (_filter != "blacklisted SlackBuilds") { filterBlacklisted(); } 
    }
    else if ( (selected == "Non-dependencies") || (selection == "N") )
    {
      if (_filter != "non-dependencies") { filterNonDeps(); } 
    }
    // Build options could have changed, so allow this one to be re-selected
    else if ( (selected == "Build options set")
           || (selection == "l") )
      filterBuildOptions();
    else if (selection == signals::resize)
    {
      getting_selection = true;
      placePopup(&_fbox, filterwin);
      draw(true);
      _fbox.draw(true);
    }
  }

  // Get rid of window and redraw

  delwin(filterwin);
  draw(true);
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
      draw(true);
      _searchbox.draw(true);
    }
    else if (selection == signals::keyEnter)
    { 
      getting_input = false;
      if (_searchbox.searchString().size() > 0)
      {
        filterSearch(_searchbox.searchString(), _searchbox.caseSensitive(),
                     _searchbox.wholeWord(), _searchbox.searchREADMEs());
      }
    }
    else if (selection == signals::quit) { getting_input = false; }
  }

  // Get rid of window and redraw

  delwin(searchwin);
  draw(true);
}

/*******************************************************************************

Dialog for actions pertaining to selected SlackBuild

*******************************************************************************/
void MainWindow::showBuildActions(BuildListItem & build, bool limited_actions)
{
  WINDOW *actionwin;
  std::string selection, selected, action;
  int ninstalled, nupgraded, nreinstalled, nremoved;
  bool getting_selection, cancel_all, check_rebuild, needs_rebuild;
  BuildActionBox actionbox;

  // Set up windows and dialog

  actionwin = newwin(1, 1, 0, 0);
  actionbox.setWindow(actionwin);
  actionbox.setName(build.name() + " actions");
  actionbox.create(build, limited_actions);
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
      draw(true);
    }
    else if ( (selected == "Browse files") || (selection == "B") )
    {
      hideWindow(actionwin);
      draw(true);
      browseFiles(build);
      placePopup(&actionbox, actionwin);
      draw(true);
    }
    else if ( (selected == "Set build options") || (selection == "S") )
    { 
      hideWindow(actionwin);
      draw(true);
      setBuildOptions(build);
      placePopup(&actionbox, actionwin);
      draw(true);
    }                                              
    else if ( (selected == "Show package info") || (selection == "h") )
    {
      hideWindow(actionwin);
      draw(true);
      showPackageInfo(build);
      placePopup(&actionbox, actionwin);
      draw(true);
    }
    else if (selection == signals::resize)
    {
      placePopup(&actionbox, actionwin);
      draw(true);
      actionbox.draw(true);
    }
    else if (selection == signals::quit) { getting_selection = false; }
    if (! limited_actions)
    {
      if ( (selected == "Install") || (selection == "I") ||
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
        draw(true);
        ninstalled = 0;
        nupgraded = 0;
        nreinstalled = 0;
        nremoved = 0;
        check_rebuild = modifyPackage(build, action, ninstalled, nupgraded,
                                      nreinstalled, nremoved, cancel_all);
        if (! needs_rebuild) { needs_rebuild = check_rebuild; }

        // If any changes were made, actions might need to change too

        if (check_rebuild) { actionbox.create(build); }
        placePopup(&actionbox, actionwin);
        draw(true);
      }                                              
      else if ( (selected == "Compute build order") || (selection == "C") )
      { 
        hideWindow(actionwin);
        draw(true);
        showBuildOrder(build);
        placePopup(&actionbox, actionwin);
        draw(true);
      }                                              
      else if ( (selected == "List inverse deps") || (selection == "L") )
      { 
        hideWindow(actionwin);
        draw(true);
        showInverseReqs(build);
        placePopup(&actionbox, actionwin);
        draw(true);
      }                                              
    }
  }

  // Get rid of window (redraw happens in MainWindow::show())

  delwin(actionwin);

  // Rebuild lists if SlackBuilds were installed/upgraded/reinstalled/removed

  if (needs_rebuild) { rebuild(); }
}

/*******************************************************************************

Performs a "quick search," jumping in the active list as the user types

*******************************************************************************/
void MainWindow::quickSearch()
{
  int rows, cols, check;
  QuickSearch qsearch;
  bool searching;
  std::string selection, entry;
  BuildListItem *build;

  getmaxyx(stdscr, rows, cols);

  qsearch.setWindow(stdscr);
  qsearch.setPosition(rows-2, 14); 
  qsearch.setWidth(cols-14);
  printStatus("Quick search: ", true);

  searching = true;
  while (searching)
  {
    selection = qsearch.exec(0);
    if (selection == signals::resize)
    {  
      getmaxyx(stdscr, rows, cols);
      qsearch.setPosition(rows-2, 14); 
      qsearch.setWidth(cols-14);
      draw(true);
      printStatus("Quick search: ", true);
      qsearch.draw(0, true, false);
    }
    else if ( (selection == signals::keyEnter) || (selection == signals::quit) )
      searching = false;
    else if (selection != "ignore")
    {
      entry = qsearch.text();
      if (_activated_listbox == 0)
      {
        check = _clistbox.highlightSearch(entry);
        if (check == 0)
        {
          _category_idx = _clistbox.highlight(); 
          _blistboxes[_category_idx].draw(true);
        }
      }
      else { _blistboxes[_category_idx].highlightSearch(entry); }
    }
  }

  if (_activated_listbox == 1)
  {
    build = static_cast<BuildListItem *>(
                                  _blistboxes[_category_idx].highlightedItem());
    printPackageVersion(*build);
  }
  else { clearStatus(); }
}

/*******************************************************************************

Not used, but needed for MainWindow to be derived from CursesWidget

*******************************************************************************/
void MainWindow::minimumSize(int & height, int & width) const {}
void MainWindow::preferredSize(int & height, int & width) const {}

/*******************************************************************************

Redraws window

*******************************************************************************/
void MainWindow::draw(bool force)
{
  clear();

  // Draw stuff

  redrawHeaderFooter(); 
  redrawWindows(force);
  refreshStatus();
}

/*******************************************************************************

Displays the main window

*******************************************************************************/
std::string MainWindow::exec()
{
  std::string selection;
  bool getting_input, all_tagged;
  int check_quit;
  unsigned int i, ncategories;
  BuildListItem *build;

  draw();

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
        // Note on static cast: allows the original BuildListItem pointer to
        // be referenced.

        build = static_cast<BuildListItem *>(
                                  _blistboxes[_category_idx].highlightedItem());
        printPackageVersion(*build);
      }

      // Tag signal: tag/untag all items in category

      else if ( (selection == "t") || (selection == "T") )
      {
        _blistboxes[_category_idx].tagAll();
        _category_idx = _clistbox.highlight();
        _blistboxes[_category_idx].draw(true);
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

        build = static_cast<BuildListItem *>(
                                  _blistboxes[_category_idx].highlightedItem());
        printPackageVersion(*build);
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

      // Tag signal

      else if ( (selection == "t") || (selection == "T") )
      {
        all_tagged = _blistboxes[_category_idx].allTagged();
        if (_clistbox.highlightedItem()->getBoolProp("tagged"))
        {
          if (! all_tagged) 
          { 
            _clistbox.highlightedItem()->setBoolProp("tagged", false);
            _clistbox.draw(true);
          }
        }
        else 
        {
          if (all_tagged) 
          { 
            _clistbox.highlightedItem()->setBoolProp("tagged", true);
            _clistbox.draw(true);
          }
        }
        // Display status message for installed SlackBuild

        build = static_cast<BuildListItem *>(
                                  _blistboxes[_category_idx].highlightedItem());
        printPackageVersion(*build);
      }
     
      // Enter signal: show action dialog

      else if (selection == signals::keyEnter)
      {
        build = static_cast<BuildListItem *>(
                                  _blistboxes[_category_idx].highlightedItem());
        showBuildActions(*build);

        // Determine if categories should be tagged and redraw

        ncategories = _clistbox.numItems();
        for ( i = 0; i < ncategories; i++ )
        {
          if (_blistboxes[i].allTagged())
            _clistbox.itemByIdx(i)->setBoolProp("tagged", true);
          else { _clistbox.itemByIdx(i)->setBoolProp("tagged", false); }
        }
        draw(true);
      }
    }

    // Key signals with the same action w/ either type of list box

    if (selection == "q") { getting_input = false; }
    else if (selection == signals::resize) { draw(true); }
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
    else if (selection == "c") { viewCommandLine(); }
    else if (selection == "i") { applyTags("Install"); }
    else if (selection == "u") { applyTags("Upgrade"); }
    else if (selection == "r") { applyTags("Remove"); }
    else if (selection == "e") { applyTags("Reinstall"); }
    else if ( (selection.size() == 1) && (selection[0] == 0x13) )  // Ctrl-s
      quickSearch(); 
  }

  return signals::quit;
}
