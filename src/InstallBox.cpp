#include <iostream>
#include <vector>
#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "settings.h"
#include "string_util.h"
#include "signals.h"
#include "requirements.h"
#include "backend.h"
#include "BuildListItem.h"
#include "InstallBox.h"

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void InstallBox::redrawFrame() const
{
  int rows, cols, namelen, i, nspaces, vlineloc;
  double mid, left, right;

  getmaxyx(_win, rows, cols);

  // Info on bottom of window

  namelen = _info.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(namelen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, color_settings.fg_info, color_settings.bg_info);
  printSpaces(left-1);
  printToEol(_info);
  colors.turnOff(_win);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  colors.turnOn(_win, color_settings.fg_title, color_settings.bg_title);
  wprintw(_win, _name.c_str());
  colors.turnOff(_win);

  // Corners

  wmove(_win, 0, 0);
  waddch(_win, ACS_ULCORNER);
  wmove(_win, rows-1, 0);
  waddch(_win, ACS_LLCORNER);
  wmove(_win, rows-1, cols-1);
  waddch(_win, ACS_LRCORNER);
  wmove(_win, 0, cols-1);
  waddch(_win, ACS_URCORNER);

  // Top border

  wmove(_win, 0, 1);
  for ( i = 1; int(i) < left-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, 0, right+1);
  for ( i = right+1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Symbols on right border to indicate scrolling

  if (_firstprint != 0) { mvwaddch(_win, 3, cols-1, ACS_UARROW); }
  if (int(_items.size()) > _firstprint + rows-4)
  {
    mvwaddch(_win, rows-4, cols-1, ACS_DARROW);
  }

  // Draw header

  wmove(_win, 1, 1);
  colors.turnOn(_win, color_settings.header_popup, color_settings.bg_popup);
  wprintw(_win, "Name");

  vlineloc = cols-2 - std::string(" Reinstall ").size();
  nspaces = vlineloc - std::string("Name").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  printToEol(" Action");
  colors.turnOff(_win);

  // Draw horizontal and then vertical lines

  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, rows-3, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  for ( i = 3; i < rows-4; i++ ) { mvwaddch(_win, i, vlineloc, ACS_VLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, rows-3, 0, ACS_LTEE);
  mvwaddch(_win, rows-3, cols-1, ACS_RTEE);
  mvwaddch(_win, 2, vlineloc, ACS_TTEE);
  mvwaddch(_win, rows-3, vlineloc, ACS_BTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void InstallBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int nspaces, vlineloc, printlen, rows, cols, i;

  getmaxyx(_win, rows, cols);

  // Print divider before applying color

  vlineloc = cols-2 - std::string(" Reinstall ").size() - 1;
  wmove(_win, idx-_firstprint+3, vlineloc+1);
  waddch(_win, ACS_VLINE);

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+3, 1);

  // Turn on highlight color

  if (int(idx) == _highlight)
  {
    if (_activated) 
    { 
      fg = color_settings.fg_highlight_active;
      bg = color_settings.bg_highlight_active; 
    }
    else
    {
      fg = color_settings.fg_highlight_inactive;
      bg = color_settings.bg_highlight_inactive; 
    }
    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else { colors.turnOn(_win, color_settings.fg_popup,
                             color_settings.bg_popup); }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item with selection, spaces, divider, action

  printlen = std::min(int(_items[idx]->name().size()), vlineloc-4);
  nspaces = vlineloc - 4 - (_items[idx]->name().size());

  if (_items[idx]->getBoolProp("tagged")) { wprintw(_win, "[X] "); }
  else { wprintw(_win, "[ ] "); }

  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());

  for ( i = 0; int(i) < nspaces; i++ ) { waddch(_win, ' '); }

  wmove(_win, idx-_firstprint+3, vlineloc+2);
  waddch(_win, ' ');
  printToEol(_items[idx]->getProp("action"));

  // Move to area between brackets to show cursor

  wmove(_win, _highlight-_firstprint+3, 2);

  // Turn off color

  if (colors.turnOff(_win) != 0)
  {
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
InstallBox::InstallBox()
{ 
  _reserved_rows = 6;
  _info = "Enter: Ok | Esc: Cancel"; 
  _builds.resize(0);
}

InstallBox::InstallBox(WINDOW *win, const std::string & name)
{
  _reserved_rows = 6;
  _info = "Enter: Ok | Esc: Cancel"; 
  _builds.resize(0);
  _win = win;
  _name = name;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void InstallBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols, action_cols;
  unsigned int i, nitems;

  // Minimum usable height

  nitems = _items.size();
  height = _reserved_rows + 2;

  // Minimum usable width

  action_cols = std::string(" Reinstall ").size() + 1; // Room for divider
  reserved_cols = 2;
  width = _name.size() + action_cols;
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + 4 + action_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void InstallBox::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols, widthpadding, action_cols;
  unsigned int i, nitems;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  action_cols = std::string(" Reinstall ").size() + 1; // Room for divider
  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size() + action_cols;
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + 4 + action_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

bool InstallBox::installingAllDeps() const
{
  int nreqs, i;

  nreqs = _items.size() - 1;   // Only consider dependencies, not the requested
                               // SlackBuild

  for ( i = 0; i < nreqs; i++ )
  {
    if ( ((_items[i]->getProp("action") == "Install") ||   
          (_items[i]->getProp("action") == "Upgrade")) &&
         (! _items[i]->getBoolProp("tagged")) ) { return false; }
  }

  return true;
}

/*******************************************************************************

Creates list based on SlackBuild selected. Returns 0 if dependency resolution
succeeded or 1 if some could not be found in the repository.

*******************************************************************************/
int InstallBox::create(BuildListItem & build,
                       std::vector<std::vector<BuildListItem> > & slackbuilds,
                       const std::string & action, bool resolve_deps,
                       bool batch) 
{
  int check; 
  unsigned int nreqs, i, nbuilds;
  bool tag;
  std::string action_applied;
  std::vector<std::string> installedpkgs;
  std::string installed_version, available_version;
  std::vector<BuildListItem *> reqlist;

  // Get list of reqs and/or add requested SlackBuild to list

  reqlist.resize(0);
  if (resolve_deps)
  {
    check = compute_reqs_order(build, reqlist, slackbuilds);
    if (check != 0) { return check; }
  }
  nreqs = reqlist.size();
  reqlist.push_back(&build);

  // Re-check install status and show different options in batch mode

  if (batch)
  {
    _info = "Enter: Ok | Esc: Skip | c: Cancel"; 
    installedpkgs = list_installed_packages();
    for ( i = 0; i <= nreqs; i++ ) 
    { 
      reqlist[i]->readInstalledProps(installedpkgs);
      reqlist[i]->readPropsFromRepo();
    }
  }

  // Copy reqlist to _builds list and determine action for each

  nbuilds = 0;
  for ( i = 0; i <= nreqs; i++ )
  {
    if (action != "Remove")
    {
      _builds.push_back(*reqlist[i]);
      if (! reqlist[i]->getBoolProp("installed"))
      {
        tag = true;
        action_applied = "Install";
      }
      else
      {
        if (reqlist[i]->upgradable())
        {
          tag = true;
          action_applied = "Upgrade";
        }
        else
        {
          // By default, do not reinstall dependencies

          if ( (action == "Reinstall") && (i == nreqs) ) { tag = true; }
          else { tag = false; }
          action_applied = "Reinstall";
        }
      }
      _builds[nbuilds].setBoolProp("tagged", tag);
      _builds[nbuilds].addProp("action", action_applied);
      nbuilds++;
    }
    else
    {
      if (reqlist[i]->getBoolProp("installed"))
      {
        _builds.push_back(*reqlist[i]);
        
        // By default, do not remove dependencies

        if (i == nreqs) { tag = true; }
        else { tag = false; }
        action_applied = "Remove";
        _builds[nbuilds].setBoolProp("tagged", tag);
        _builds[nbuilds].addProp("action", action_applied);
        nbuilds++;
      }
    }
  }

  // Add to list (note have to do this separately because _builds changes
  // throughout the above loop)

  for ( i = 0; i < nbuilds; i++ ) { addItem(&_builds[i]); }

  // Set window title

  if (! resolve_deps)
    setName(build.name() + " (deps ignored)");
  else
  {
    if (nbuilds == 2)
    {
      if (action == "Remove")
        setName(build.name() + " (1 installed dep)");
      else
        setName(build.name() + " (1 dep)");
    }
    else
    {
      if (action == "Remove")
      {
        setName(build.name() + 
                " (" + int_to_string(nbuilds-1) + " installed deps)");
      }
      else
        setName(build.name() + " (" + int_to_string(nbuilds-1) + " deps)");
    }
  }

  return 0;
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string InstallBox::exec()
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;

  curs_set(1);

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  // Draw list elements

  draw();

  // Get user input

  switch (ch = getch()) {

    // Enter key: accept selection

    case '\n':
    case '\r':
    case KEY_ENTER:
      retval = signals::keyEnter;
      _redraw_type = "all";
      break;

    // Arrows/Home/End/PgUp/Dn: change highlighted value

    case KEY_UP:
      retval = signals::highlight;
      check_redraw = highlightPrevious();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_DOWN:
      retval = signals::highlight;
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_PPAGE:
      retval = signals::highlight;
      check_redraw = highlightPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_NPAGE:
      retval = signals::highlight;
      check_redraw = highlightNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_HOME:
      retval = signals::highlight;
      check_redraw = highlightFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;
    case KEY_END:
      retval = signals::highlight;
      check_redraw = highlightLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    // Resize signal: redraw (may not work with some curses implementations)

    case KEY_RESIZE:
      retval = signals::resize;
      _redraw_type = "all";
      break;

    // Quit key

    case MY_ESC:
      retval = signals::quit;
      _redraw_type = "all";
      break;

    // Space: toggle item

    case ' ':
      retval = " ";
      _items[_highlight]->setBoolProp("tagged", 
                                 (! _items[_highlight]->getBoolProp("tagged")));
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  curs_set(0);

  return retval;
}

/*******************************************************************************

Checks for packages to upgrade/remove/reinstall that were installed from a
different repo (i.e., tag doesn't match)

*******************************************************************************/
std::vector<const BuildListItem *> InstallBox::checkForeign() const
{
  unsigned int nbuilds, i, taglen, pkgnamelen;
  std::string pkgname;
  std::vector<const BuildListItem *> foreign;

  foreign.resize(0);
  nbuilds = _builds.size();
  taglen = settings::repo_tag.size();
  for ( i = 0; i < nbuilds; i++ )
  {
    if ( (_builds[i].getBoolProp("installed")) &&
         (_builds[i].getBoolProp("tagged")) )
    {
      pkgname = _builds[i].getProp("package_name"); 
      pkgnamelen = pkgname.size(); 
      if (pkgname.substr(pkgnamelen-taglen, pkgnamelen) != settings::repo_tag)
        foreign.push_back(&_builds[i]);
    }
  }

  return foreign;
}

/*******************************************************************************

Install, upgrade, reinstall, or remove SlackBuild and dependencies. Returns 0 on
success. Also counts number of SlackBuilds that were changed.

*******************************************************************************/
int InstallBox::applyChanges(int & ninstalled, int & nupgraded,
                             int & nreinstalled, int & nremoved) const
{
  unsigned int nbuilds, i;
  int retval;
  std::string action, response, msg;

  // Install/upgrade/reinstall/remove selected SlackBuilds

  nbuilds = _builds.size();
  retval = 0;
  for ( i = 0; i < nbuilds; i++ )
  {
    if (_builds[i].getBoolProp("tagged"))
    {
      action = _builds[i].getProp("action");
      if (action == "Install") { retval = install_slackbuild(_builds[i]); }
      else if (action == "Upgrade") { retval = upgrade_slackbuild(_builds[i]); }
      else if (action == "Remove") { retval = remove_slackbuild(_builds[i]); }
      else if (action == "Reinstall")
      { 
        retval = remove_slackbuild(_builds[i]);
        if (retval == 0) { retval = install_slackbuild(_builds[i]); }
      }
      else
      {
        std::cout << "Error: unrecognized action " + action + "." << std::endl;
        retval = 1;
      }

      // Handle errors

      if (retval == 127)
      {
        std::cout << "Error: package manager not found. ";
        break;
      }
      else if (retval != 0)
      {
        if (i != nbuilds-1)
        {
          std::cout << "An error occurred. Continue anyway [y/N]? ";
          std::getline(std::cin, response);
          if ( (response != "y") && (response != "Y") ) { return retval; }
        }
        else { std::cout << " An error occurred. "; }
      }
      else
      {
        if (_builds[i].getProp("action") == "Install")
          ninstalled++;
        else if (_builds[i].getProp("action") == "Upgrade")
          nupgraded++;
        else if (_builds[i].getProp("action") == "Reinstall")
          nreinstalled++; 
        else
          nremoved++; 
      }
    }
  }

  if (retval != 0)
  {
    std::cout << "Press Enter to return to main window ...";
    std::getline(std::cin, response);
  }

  return retval;
} 
