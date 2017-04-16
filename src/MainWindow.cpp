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

/*******************************************************************************

Prints/clears status message

*******************************************************************************/
void MainWindow::printStatus(const std::string & msg)
{
  int rows, cols;

  getmaxyx(stdscr, rows, cols);
  move(rows-2, 0);
  clrtoeol();
  printToEol(msg, cols);
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
  colors.turnOn(stdscr, color_settings.fg_title, color_settings.bg_title);
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
  colors.turnOn(stdscr, color_settings.fg_info, color_settings.bg_info);
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
  _installedlist.resize(0);
  _nondeplist.resize(0);
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

  list_installed(_slackbuilds, _installedlist, pkg_errors, missing_info);

  // Warning for bad package names

  npkgerr = pkg_errors.size();
  if (npkgerr > 0)
  {
    errmsg = "The following installed packages have invalid names "
           + std::string("and were ignored:\n");
    for ( i = 0; i < npkgerr; i++ ) { errmsg += "\n" + pkg_errors[i]; }
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
  unsigned int k, ninstalled;
  int i, j, ntagged;
  std::vector<std::string> pkg_errors, missing_info;

  // Clear information that may have changed from slackbuilds list

  ninstalled = _installedlist.size();
  for ( k = 0; k < ninstalled; k++ )
  {
    if (_installedlist[k]->getBoolProp("installed"))
    {
      _installedlist[k]->setBoolProp("installed", false);
      _installedlist[k]->setProp("installed_version", "");
      _installedlist[k]->setProp("available_version", "");
      _installedlist[k]->setProp("requires", "");
      _installedlist[k]->setProp("package_name", "");
    }
  }

  // Clear tags

  ntagged = _taglist.numTagged();
  for ( k = 0; int(k) < ntagged; k++ )
  {
    find_slackbuild(_taglist.taggedByIdx(k).name(), _slackbuilds, i, j);
    _slackbuilds[i][j].setBoolProp("tagged", false);
  }
  _taglist.clearList();

  // Rebuild installed list 

  _nondeplist.resize(0);
  list_installed(_slackbuilds, _installedlist, pkg_errors, missing_info);

  // Re-filter (data, tags could have changed)

  if (_filter == "installed SlackBuilds") { filterInstalled(); }
  else if (_filter == "upgradable SlackBuilds") { filterUpgradable(); } 
  else if (_filter == "tagged SlackBuilds") { filterTagged(); } 
  else if (_filter == "blacklisted SlackBuilds") { filterBlacklisted(); }
  else if (_filter == "non-dependencies") { filterNonDeps(); } 
  else { filterAll(); }

  draw(true);
}

/*******************************************************************************

Displays all SlackBuilds

*******************************************************************************/
void MainWindow::filterAll()
{
  unsigned int i, nbuilds, ncategories;
  std::string choice;

  _filter = "all SlackBuilds";
  printStatus("Filtering by all SlackBuilds ...");

  _activated_listbox = 0;
  _category_idx = 0;
  ncategories = _slackbuilds.size();
  nbuilds = 0;
  for ( i = 0; i < ncategories; i++ ) { nbuilds += _slackbuilds[i].size(); }

  filter_all(_slackbuilds, _categories, _win2, _clistbox, _blistboxes);

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

  _filter = "installed SlackBuilds";
  printStatus("Filtering by installed SlackBuilds ...");

  if (_installedlist.size() == 0) 
    list_installed(_slackbuilds, _installedlist, pkg_errors, missing_info);

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

  setTagList();
}

/*******************************************************************************

Displays upgradable SlackBuilds

*******************************************************************************/
void MainWindow::filterUpgradable()
{
  unsigned int nupgradable;
  std::vector<std::string> pkg_errors, missing_info;

  _filter = "upgradable SlackBuilds";
  printStatus("Filtering by upgradable SlackBuilds ...");
  if (_installedlist.size() == 0) 
    list_installed(_slackbuilds, _installedlist, pkg_errors, missing_info); 

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

  setTagList();
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

  filter_by_prop(_slackbuilds, "tagged", _categories, _win2, _clistbox,
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

  _filter = "blacklisted SlackBuilds";
  printStatus("Filtering by blacklisted SlackBuilds ...");

  _category_idx = 0;
  _activated_listbox = 0;
  nblacklisted = 0;

  filter_by_prop(_slackbuilds, "blacklisted", _categories, _win2, _clistbox,
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

  if (_installedlist.size() == 0) 
    list_installed(_slackbuilds, _installedlist, pkg_errors, missing_info);
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

  setTagList();
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

  setTagList();
}

/*******************************************************************************

Shows options window

*******************************************************************************/
int MainWindow::showOptions()
{
  WINDOW *optionswin;
  std::string selection, errmsg;
  bool getting_input;
  int check;

  clear();
  setInfo("Enter: Apply settings | Esc: Back to main");
  redrawHeaderFooter();

  optionswin = newwin(1, 1, 0, 0);
  _options.setWindow(optionswin);
  _options.placeWindow();
  _options.readSettings();

  getting_input = true;
  check = 0;
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
      check = _options.applySettings();
    }
  }

  // Error messages for problems with color theme file

  if (check != 0)
  {
    switch (check) {
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
  else
    if (selection == signals::keyEnter)
      displayMessage("Settings were successfully applied.");

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
  unsigned int i, ninstaller, nforeign;
  std::vector<const BuildListItem *> foreign;
  InstallBox installer;

  if (settings::resolve_deps)
    printStatus("Computing dependencies for " + build.name() + " ...");
  check = installer.create(build, _slackbuilds, action, settings::resolve_deps,
                           batch);

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
      draw(true);
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

  return check;
}

/*******************************************************************************

Applies action to tagged SlackBuilds

*******************************************************************************/
void MainWindow::applyTags(const std::string & action)
{
  WINDOW *tagwin;
  unsigned int ndisplay, i;
  int ninstalled, nupgraded, nreinstalled, nremoved;
  bool getting_input, cancel_all, apply_changes, any_modified, needs_rebuild;
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
      item = _taglist.itemByIdx(i);
      if (item.getBoolProp("tagged"))
      { 
        any_modified = modifyPackage(item, action, ninstalled, nupgraded,
                                     nreinstalled, nremoved, cancel_all, true);
        if (! needs_rebuild) { needs_rebuild = any_modified; }
      }
      draw(true);
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
  msgbox.setColor(colors.getPair(color_settings.fg_popup,
                                 color_settings.bg_popup));
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

Constructor and destructor

*******************************************************************************/
MainWindow::MainWindow(const std::string & version)
{
  _win1 = NULL;
  _win2 = NULL;
  _blistboxes.resize(0);
  _slackbuilds.resize(0);
  _installedlist.resize(0);
  _nondeplist.resize(0);
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
                     _searchbox.wholeWord());
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
void MainWindow::showBuildActions(BuildListItem & build)
{
  WINDOW *actionwin;
  std::string selection, selected, action;
  int ninstalled, nupgraded, nreinstalled, nremoved;
  bool getting_selection, cancel_all, needs_rebuild;
  BuildActionBox actionbox;

  // Set up windows and dialog

  actionwin = newwin(1, 1, 0, 0);
  actionbox.setWindow(actionwin);
  actionbox.setName(build.name() + " actions");
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
      draw(true);
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
      draw(true);
      ninstalled = 0;
      nupgraded = 0;
      nreinstalled = 0;
      nremoved = 0;
      needs_rebuild = modifyPackage(build, action, ninstalled, nupgraded,
                                    nreinstalled, nremoved, cancel_all);
      if (needs_rebuild) { getting_selection = false; }
      else
      {
        placePopup(&actionbox, actionwin);
        draw(true);
      }
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
    else if ( (selected == "Browse files") || (selection == "B") )
    {
      hideWindow(actionwin);
      draw(true);
      browseFiles(build);
      placePopup(&actionbox, actionwin);
      draw(true);
    }
    else if (selection == signals::resize)
    {
      placePopup(&actionbox, actionwin);
      draw(true);
      actionbox.draw(true);
    }
    else { getting_selection = false; }
  }

  // Get rid of window (redraw happens in MainWindow::show())

  delwin(actionwin);

  // Rebuild lists if SlackBuilds were installed/upgraded/reinstalled/removed

  if (needs_rebuild) { rebuild(); }
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
  std::string selection, statusmsg;
  bool getting_input, all_tagged;
  int check_quit;
  unsigned int i, ncategories;
  BuildListItem build;
  ListItem *item;

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

        build = *_blistboxes[_category_idx].highlightedItem();
        if (build.getBoolProp("installed"))
        {
          if (build.getBoolProp("blacklisted"))
            statusmsg = "Installed: " + build.getProp("installed_version") +
              " -> Available: " + build.getProp("available_version") +
              " (blacklisted)";
          else
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
          if (build.getBoolProp("blacklisted"))
            statusmsg = "Installed: " + build.getProp("installed_version") +
              " -> Available: " + build.getProp("available_version") +
              " (blacklisted)";
          else
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
    else if (selection == "i") { applyTags("Install"); }
    else if (selection == "u") { applyTags("Upgrade"); }
    else if (selection == "r") { applyTags("Remove"); }
    else if (selection == "e") { applyTags("Reinstall"); }
  }

  return signals::quit;
}
