#include <vector>
#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "MouseEvent.h"
#include "requirements.h"
#include "BuildListItem.h"
#include "BuildOrderBox.h"

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void BuildOrderBox::redrawFrame()
{
  int rows, cols, namelen, i, nspaces, vlineloc;
  double mid, left, right;

  getmaxyx(_win, rows, cols);

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

  vlineloc = cols-2 - std::string("Installed").size();
  nspaces = vlineloc - std::string("Name").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  printToEol("Installed");
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

  // Button area

  if (_buttons.size() > 0)
    redrawButtons();
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void BuildOrderBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int nspaces, vlineloc, printlen, rows, cols, i, nast;

  getmaxyx(_win, rows, cols);

  // Print divider before applying color

  vlineloc = cols-2 - std::string("Installed").size() - 1;
  wmove(_win, idx-_firstprint+_header_rows, vlineloc+1);
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

  // Print an asterisk next to any SlackBuild with build options set

  nast = 0;
  if (_items[idx]->getProp("build_options") != "")
    nast = 1;

  // Print item, spaces, install status

  printlen = std::min(int(_items[idx]->name().size()), vlineloc);
  nspaces = vlineloc - _items[idx]->name().size() - nast;
  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());
  if (nast == 1)
    waddch(_win, '*');

  for ( i = 0; int(i) < nspaces; i++ ) { waddch(_win, ' '); }

  wmove(_win, idx-_firstprint+3, vlineloc+2);
  if (_items[idx]->getBoolProp("installed")) { printToEol("   [X]   "); }
  else { printToEol("   [ ]   "); }

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
BuildOrderBox::BuildOrderBox()
{ 
  std::vector<std::string> buttons(2), button_signals(2);

  _reserved_rows = 6;
  _header_rows = 3;
  buttons[0] = "  Back  ";
  buttons[1] = "  Actions  ";
  button_signals[0] = signals::quit;
  button_signals[1] = "a";
  setButtons(buttons, button_signals);
  setColor("fg_popup", "bg_popup");
}

BuildOrderBox::BuildOrderBox(WINDOW *win, const std::string & name)
{
  std::vector<std::string> buttons(2), button_signals(2);

  _reserved_rows = 6;
  _header_rows = 3;
  buttons[0] = "  Back  ";
  buttons[1] = "  Actions  ";
  button_signals[0] = signals::quit;
  button_signals[1] = "a";
  setButtons(buttons, button_signals);
  setColor("fg_popup", "bg_popup");
  _win = win;
  _name = name;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void BuildOrderBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols, installed_cols;
  unsigned int i, nitems, nbuttons;

  // Minimum usable height

  nitems = _items.size();
  height = _reserved_rows + 2;

  // Minimum usable width

  installed_cols = std::string("Installed").size() + 1; // Room for divider
  reserved_cols = 2;
  width = _name.size() + installed_cols;
  nbuttons = _buttons.size();
  if (nbuttons > 0)
  {
    namelen = 0;
    for ( i = 0; i < nbuttons; i++ )
    {
      namelen += _buttons[i].size();
    }
    if (namelen > width) { width = namelen; }
  }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + installed_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void BuildOrderBox::preferredSize(int & height, int & width) const
{
  int widthpadding;
  unsigned int nitems;

  minimumSize(height, width);

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  widthpadding = 6;
  width += widthpadding;
}

/*******************************************************************************

Creates list based on SlackBuild selected. Returns 0 if dependency resolution
succeeded or 1 if some could not be found in the repository, or 2 if a bad mode
was specified. Mode is "forward"
or "inverse" to display a regular build order or a list of inverse requirements.

*******************************************************************************/
int BuildOrderBox::create(BuildListItem & build,
                         std::vector<std::vector<BuildListItem> > & slackbuilds,
                         const std::string & mode)
{
  int check; 
  unsigned int nbuilds, i;
  std::vector<BuildListItem *> reqlist;
  std::vector<std::string> buttons, button_signals;

  if (mode == "forward")
  {
    setName(build.name() + " build order");
    check = compute_reqs_order(build, reqlist, slackbuilds);
    reqlist.push_back(&build);
  }
  else if (mode == "inverse")
  {
    setName(build.name() + " inverse deps");
    compute_inv_reqs(build, reqlist, slackbuilds);
  }
  else
    return 2;

  nbuilds = reqlist.size();
  for ( i = 0; i < nbuilds; i++ ) { addItem(reqlist[i]); }

  if (nbuilds > 0)
  {
    buttons.resize(2);
    button_signals.resize(2);
    buttons[0] = "  Back  ";
    buttons[1] = "  Actions  ";
    button_signals[0] = signals::quit;
    button_signals[1] = "a";
    setButtons(buttons, button_signals);
  }
  else
  {
    buttons.resize(1);
    button_signals.resize(1);
    buttons[0] = "  Back  ";
    button_signals[0] = signals::quit;
    setButtons(buttons, button_signals);
  }

  return check;
}

/*******************************************************************************

Handles mouse events

*******************************************************************************/
std::string BuildOrderBox::handleMouseEvent(MouseEvent * mevent)
{
  int rows, cols, begy, begx, ycurs;
  std::string retval;

  getmaxyx(_win, rows, cols);
  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;

  // Use the inherited method from ListBox, but modify some behaviors

  retval = ListBox::handleMouseEvent(mevent);

  // Double-click on item: show actions

  if ( (ycurs >= int(_header_rows)) && (ycurs < rows-2) &&
       (retval == signals::keyEnter) )
    retval = "a";

  return retval;
}
