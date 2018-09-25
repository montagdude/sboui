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
#include "MouseEvent.h"

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void InstallBox::redrawFrame()
{
  int rows, cols, namelen, i, nspaces, action_cols, vlineloc;
  unsigned int j, nitems, actionlen;
  double mid, left, right;

  getmaxyx(_win, rows, cols);

  // Info on bottom of window

  namelen = _info.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(namelen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, "fg_info", "bg_info");
  printSpaces(left-1);
  printToEol(_info);
  colors.turnOff(_win);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  colors.turnOn(_win, "fg_title", "bg_title");
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

  // Right border for header and footer

  mvwaddch(_win, 1, cols-1, ACS_VLINE);
  mvwaddch(_win, rows-2, cols-1, ACS_VLINE);

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Draw header

  wmove(_win, 1, 1);
  colors.turnOn(_win, "header_popup", "bg_popup");
  wprintw(_win, "Name");

  nitems = _items.size();
  action_cols = 0;
  for ( j = 0; j < nitems; j++ )
  {
    actionlen = _items[j]->getProp("action").size() + 1;
    if (int(actionlen) > action_cols) { action_cols = actionlen; }
  }
  vlineloc = cols-2 - action_cols - 1;
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
  int nspaces, actionlen, action_cols, vlineloc, printlen, rows, cols, i;
  unsigned int j, nitems;

  getmaxyx(_win, rows, cols);

  // Print divider before applying color

  nitems = _items.size();
  action_cols = 0;
  for ( j = 0; j < nitems; j++ )
  {
    actionlen = _items[j]->getProp("action").size() + 1;
    if (actionlen > action_cols) { action_cols = actionlen; }
  }
  vlineloc = cols-2 - action_cols - 1;
  wmove(_win, idx-_firstprint+_header_rows, vlineloc);
  waddch(_win, ACS_VLINE);

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+3, 1);

  // Turn on highlight color

  if (int(idx) == _highlight)
  {
    if (_items[idx]->getBoolProp("tagged")) { fg = "tagged"; }
    else { fg = "fg_highlight_active"; }
    bg = "bg_highlight_active"; 

    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
      if (_items[idx]->getBoolProp("tagged")) { wattron(_win, A_BOLD); } 
    }
  } 
  else
  {
    if (_items[idx]->getBoolProp("tagged")) { fg = "tagged"; }
    else { fg = "fg_popup"; }
    bg = "bg_popup";
    if (colors.turnOn(_win, fg, bg) != 0)
    {
      if (_items[idx]->getBoolProp("tagged")) { wattron(_win, A_BOLD); } 
    }
  }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item with selection, spaces, divider, action

  printlen = std::min(int(_items[idx]->name().size()), vlineloc-5);
  nspaces = vlineloc - 5 - (_items[idx]->name().size());

  if (_items[idx]->getBoolProp("marked")) { wprintw(_win, "[X] "); }
  else { wprintw(_win, "[ ] "); }

  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());

  for ( i = 0; int(i) < nspaces; i++ ) { waddch(_win, ' '); }

  wmove(_win, idx-_firstprint+3, vlineloc+1);
  waddch(_win, ' ');
  printToEol(_items[idx]->getProp("action"));

  // Move to area between brackets to show cursor

  wmove(_win, _highlight-_firstprint+3, 2);

  // Turn off color

  if (colors.turnOff(_win) != 0)
  {
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
    if (_items[idx]->getBoolProp("tagged")) { wattroff(_win, A_BOLD); } 
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
InstallBox::InstallBox()
{ 
  _info = "Enter: Ok | Esc: Cancel | a: Actions";
  _builds.resize(0);
  _ndeps = 0;
  _ninvdeps = 0;
}

InstallBox::InstallBox(WINDOW *win, const std::string & name)
{
  _info = "Enter: Ok | Esc: Cancel | a: Actions"; 
  _builds.resize(0);
  _ndeps = 0;
  _ninvdeps = 0;
  _win = win;
  _name = name;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void InstallBox::minimumSize(int & height, int & width) const
{
  int namelen, actionlen, reserved_cols, action_cols;
  unsigned int i, nitems;

  // Minimum usable height

  nitems = _items.size();
  height = _reserved_rows + 2;

  // Minimum usable width

  reserved_cols = 2;
  width = _name.size();
  action_cols = 0;
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    actionlen = _items[i]->getProp("action").size() + 1;
    if (actionlen > action_cols) { action_cols = actionlen; }
    namelen = _items[i]->name().size() + 4 + action_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void InstallBox::preferredSize(int & height, int & width) const
{
  int namelen, actionlen, reserved_cols, widthpadding, action_cols;
  unsigned int i, nitems;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size();
  action_cols = 0;
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    actionlen = _items[i]->getProp("action").size() + 1;
    if (actionlen > action_cols) { action_cols = actionlen; }
    namelen = _items[i]->name().size() + 4 + action_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

/* Note that this will be 0 unless create() is called with resolve_deps */
int InstallBox::numDeps() const { return _ndeps; }

/* Note that this will be 0 unless create() is called with rebuild_inv_deps
   and the SlackBuild is being upgraded */
int InstallBox::numInvDeps() const { return _ninvdeps; }

bool InstallBox::installingAllDeps() const
{
  int i;

  for ( i = 0; i < _ndeps; i++ )
  {
    if ( ((_items[i]->getProp("action") == "Install") ||   
          (_items[i]->getProp("action") == "Upgrade")) &&
         (! _items[i]->getBoolProp("marked")) ) { return false; }
  }

  return true;
}

bool InstallBox::installingRequested() const
{
  if ( (_items[_ndeps]->getProp("action") != "Remove") &&
       (_items[_ndeps]->getBoolProp("marked")) ) { return true; }
  else { return false; }
}

/*******************************************************************************

Creates list based on SlackBuild selected. Returns 0 if dependency resolution
succeeded or 1 if some could not be found in the repository.

*******************************************************************************/
int InstallBox::create(BuildListItem & build,
                       std::vector<std::vector<BuildListItem> > & slackbuilds,
                       const std::string & action, bool resolve_deps,
                       bool batch, bool rebuild_inv_deps) 
{
  int check; 
  unsigned int i, nbuilds;
  bool mark;
  std::string action_applied;
  std::string installed_version, available_version;
  std::vector<BuildListItem *> reqlist;

  // Get list of reqs and/or add requested SlackBuild to list

  _ndeps = 0;
  reqlist.resize(0);
  if (resolve_deps)
  {
    check = compute_reqs_order(build, reqlist, slackbuilds);
    if (check != 0) { return check; }
  }
  _ndeps = reqlist.size();
  reqlist.push_back(&build);

  if (batch) { _info = "Enter: Ok | Esc: Skip | c: Cancel | a: Actions"; }

  // Copy reqlist to _builds list and determine action for each

  nbuilds = 0;
  for ( i = 0; int(i) <= _ndeps; i++ )
  {
    mark = false;
    if (action != "Remove")
    {
      _builds.push_back(reqlist[i]);
      if (! reqlist[i]->getBoolProp("installed"))
      {
        mark = true;
        action_applied = "Install";
      }
      else
      {
        if (reqlist[i]->getBoolProp("upgradable"))
        {
          mark = true;
          action_applied = "Upgrade";
        }
        else
        {
          // By default, do not reinstall dependencies

          if ( (action == "Reinstall") && (int(i) == _ndeps) ) { mark = true; }
          else { mark = false; }
          action_applied = "Reinstall";
        }
      }
      _builds[nbuilds]->setBoolProp("marked", mark);
      _builds[nbuilds]->setProp("action", action_applied);
      nbuilds++;
    }
    else
    {
      if (reqlist[i]->getBoolProp("installed"))
      {
        _builds.push_back(reqlist[i]);
        
        // By default, do not remove dependencies

        if (int(i) == _ndeps) { mark = true; }
        else { mark = false; }
        action_applied = "Remove";
        _builds[nbuilds]->setBoolProp("marked", mark);
        _builds[nbuilds]->setProp("action", action_applied);
        nbuilds++;
      }
    }
  }

  // When removing, we only consider installed dependencies, so reset _ndeps
  // appropriately

  if (action == "Remove")
    _ndeps = nbuilds - 1;

  // Rebuild inverse deps if requested and in upgrade mode

  if ((action == "Upgrade") && rebuild_inv_deps)
  {
    reqlist.resize(0);
    compute_inv_reqs(build, reqlist, slackbuilds);
    _ninvdeps = reqlist.size();
    
    for ( i = 0; int(i) < _ninvdeps; i++ )
    {
      _builds.push_back(reqlist[i]);
      _builds[nbuilds]->setBoolProp("marked", true);
      if (_builds[nbuilds]->getBoolProp("upgradable"))
        _builds[nbuilds]->setProp("action", "Upgrade");
      else
        _builds[nbuilds]->setProp("action", "Reinstall");
      nbuilds++;
    }
  }

  // Add to list (note have to do this separately because _builds changes
  // throughout the above loop)

  for ( i = 0; i < nbuilds; i++ ) { addItem(_builds[i]); }

  // Unmark any blacklisted package

  for ( i = 0; i < nbuilds; i++ )
  {
    if ( (_builds[i]->getBoolProp("installed")) &&
         (_builds[i]->getBoolProp("blacklisted")) )
    {
      _builds[i]->setBoolProp("marked", false);
      _builds[i]->setProp("action", "(blacklisted)");
    }
  }

  // Set window title

  if (! resolve_deps)
    setName(build.name() + " (deps ignored)");
  else
  {
    if (_ndeps == 1)
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
                " (" + int_to_string(_ndeps) + " installed deps)");
      }
      else
        setName(build.name() + " (" + int_to_string(_ndeps) + " deps)");
    }
  }

  return 0;
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string InstallBox::exec(MouseEvent * mevent)
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;

  curs_set(1);

  // Highlight first entry on first display

  if ( (_highlight == 0) && (_prevhighlight == 0) ) { highlightFirst(); }

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
      if (! _items[_highlight]->getBoolProp("blacklisted"))
      {
        _items[_highlight]->setBoolProp("marked", 
                                 (! _items[_highlight]->getBoolProp("marked")));
        check_redraw = highlightNext();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
      }
      else { _redraw_type = "none"; }
      break;

    // t and T: tag item

    case 't':
      retval = "t";
      tagSlackBuild(_highlight);
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    case 'T':
      retval = "T";
      tagSlackBuild(_highlight);
      check_redraw = highlightPrevious();
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
    if ( (_builds[i]->getBoolProp("installed")) &&
         (_builds[i]->getBoolProp("marked")) )
    {
      pkgname = _builds[i]->getProp("package_name"); 
      pkgnamelen = pkgname.size(); 
      if (pkgname.substr(pkgnamelen-taglen, pkgnamelen) != settings::repo_tag)
        foreign.push_back(_builds[i]);
    }
  }

  return foreign;
}

/*******************************************************************************

Install, upgrade, reinstall, or remove SlackBuild and dependencies. Returns 0 on
success. Also counts number of SlackBuilds that were changed.

*******************************************************************************/
int InstallBox::applyChanges(int & ninstalled, int & nupgraded,
                             int & nreinstalled, int & nremoved)
{
  unsigned int nbuilds, i;
  int retval;
  std::string action, response, msg;

  // Install/upgrade/reinstall/remove selected SlackBuilds

  nbuilds = _builds.size();
  retval = 0;
  for ( i = 0; i < nbuilds; i++ )
  {
    if (_builds[i]->getBoolProp("marked"))
    {
      action = _builds[i]->getProp("action");
      if (action == "Install") { retval = install_slackbuild(*_builds[i]); }
      else if (action == "Upgrade") { retval = 
                                      upgrade_slackbuild(*_builds[i]); }
      else if (action == "Remove") { retval = remove_slackbuild(*_builds[i]); }
      else if (action == "Reinstall") { retval =
                                        reinstall_slackbuild(*_builds[i]); }
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
        if (_builds[i]->getProp("action") == "Install")
          ninstalled++;
        else if (_builds[i]->getProp("action") == "Upgrade")
          nupgraded++;
        else if (_builds[i]->getProp("action") == "Reinstall")
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
