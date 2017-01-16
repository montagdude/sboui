#include <vector>
#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "color_settings.h"
#include "signals.h"
#include "requirements.h"
#include "BuildListItem.h"
#include "ScrollBox.h"
#include "BuildOrderBox.h"

using namespace color;

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void BuildOrderBox::redrawFrame() const
{
  unsigned int rows, cols, namelen, i, nspaces, vlineloc;
  double mid, left, right;
  int pair_title, pair_info;

  getmaxyx(_win, rows, cols);

  // Info on bottom of window

  namelen = _info.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_info != -1) { wattron(_win, COLOR_PAIR(pair_info)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_info);
  if (pair_info != -1) { wattroff(_win, COLOR_PAIR(pair_info)); }
  wattroff(_win, A_BOLD);

  // Title

  namelen = _name.size();
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  wattron(_win, A_BOLD);
  wprintw(_win, _name.c_str());
  wattroff(_win, A_BOLD);

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
  if (_items.size() > _firstprint + rows-4)
  {
    mvwaddch(_win, rows-4, cols-1, ACS_DARROW);
  }

  // Draw header

  pair_title = colors.pair(fg_title, bg_title);

  wmove(_win, 1, 1);
  if (pair_title != -1) { wattron(_win, COLOR_PAIR(pair_title)); }
  wattron(_win, A_BOLD);
  wprintw(_win, "Name");

  vlineloc = cols-2 - std::string("Installed").size();
  nspaces = vlineloc - std::string("Name").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  printToEol("Installed");
  if (pair_title != -1) { wattroff(_win, COLOR_PAIR(pair_title)); }
  wattroff(_win, A_BOLD);

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
void BuildOrderBox::redrawSingleItem(unsigned int idx)
{
  int nspaces, vlineloc, printlen, rows, cols, i;

  getmaxyx(_win, rows, cols);

  // Go to item location and print item

  wmove(_win, idx-_firstprint+3, 1);
  vlineloc = cols-2 - std::string("Installed").size() - 1;
  printlen = std::min(int(_items[idx]->name().size()), vlineloc);
  wprintw(_win, _items[idx]->name().substr(0,printlen).c_str());

  // Print spaces, divider, install status

  nspaces = vlineloc - _items[idx]->name().size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
  waddch(_win, ACS_VLINE);
  if (_items[idx]->getBoolProp("installed")) { printToEol("   [X]   "); }
  else { printToEol("   [ ]   "); }
}

/*******************************************************************************

Constructors

*******************************************************************************/
BuildOrderBox::BuildOrderBox()
{ 
  _reserved_rows = 6;
  _info = "Esc: Back"; 
}

BuildOrderBox::BuildOrderBox(WINDOW *win, const std::string & name)
{
  _reserved_rows = 6;
  _info = "Esc: Back";
  _win = win;
  _name = name;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void BuildOrderBox::setInfo(const std::string & info) { _info = info; }

/*******************************************************************************

Get attributes

*******************************************************************************/
void BuildOrderBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols, installed_cols;
  unsigned int i, nitems;

  // Minimum usable height

  height = _reserved_rows + 2;

  // Minimum usable width

  installed_cols = std::string("Installed").size() + 1; // Room for divider
  width = _name.size();
  reserved_cols = 2;
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + installed_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void BuildOrderBox::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols, widthpadding, installed_cols;
  unsigned int i, nitems;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  installed_cols = std::string("Installed").size() + 1; // Room for divider
  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size();
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size() + installed_cols;
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

/*******************************************************************************

Creates list based on SlackBuild selected. Returns 0 if dependency resolution
succeeded or 1 if some could not be found in the repository.

*******************************************************************************/
int BuildOrderBox::create(const BuildListItem & build,
                          const std::vector<BuildListItem> & slackbuilds) 
{
  int check; 
  unsigned int nbuilds, i;

  setName(build.name() + " build order");
  check = compute_reqs_order(build, _reqlist, slackbuilds);
  _reqlist.push_back(build);

  nbuilds = _reqlist.size();
  for ( i = 0; i < nbuilds; i++ ) { addItem(&_reqlist[i]); }

  return check;
}

/*******************************************************************************

Draws scroll box (frame, items, etc.) as needed

*******************************************************************************/
void BuildOrderBox::draw(bool force)
{
  int pair_popup;

  if (force) { _redraw_type = "all"; }

  // Draw list elements

  if (_redraw_type == "all")
  { 
    wclear(_win); 
    pair_popup = colors.pair(fg_popup, bg_popup);
    if (pair_popup != -1) { wbkgd(_win, COLOR_PAIR(pair_popup)); }
  }
  if (_redraw_type != "none") { redrawFrame(); }
  if ( (_redraw_type == "all") || (_redraw_type == "items")) { 
                                                            redrawAllItems(); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string BuildOrderBox::exec()
{
  int ch, check_redraw;
  std::string retval;

  const int MY_ESC = 27;

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

    // Arrows/Home/End/PgUp/Dn: scrolling

    case KEY_UP:
      retval = signals::scroll;
      check_redraw = scrollUp();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_DOWN:
      retval = signals::scroll;
      check_redraw = scrollDown();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_PPAGE:
      retval = signals::scroll;
      check_redraw = scrollPreviousPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_NPAGE:
      retval = signals::scroll;
      check_redraw = scrollNextPage();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_HOME:
      retval = signals::scroll;
      check_redraw = scrollFirst();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;
    case KEY_END:
      retval = signals::scroll;
      check_redraw = scrollLast();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "none"; }
      break;

    // Resize signal

    case KEY_RESIZE:
      retval = signals::resize;
      _redraw_type = "all";
      break;

    // Quit key

    case MY_ESC:
      retval = signals::quit;
      _redraw_type = "all";
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  return retval;
}
