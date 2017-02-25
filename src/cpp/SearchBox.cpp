#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // min, max
#include "Color.h"
#include "settings.h"
#include "TextInput.h"
#include "ToggleInput.h"
#include "SearchBox.h"

using namespace color;

/*******************************************************************************

Draws window border, message, and info

*******************************************************************************/
void SearchBox::redrawFrame() const
{
  unsigned int rows, cols, msglen, i;
  int left, pair_title, pair_info;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  msglen = _msg.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, 1, 1);
  wclrtoeol(_win);
  pair_title = colors.pair(fg_title, bg_title);
  if (pair_title != -1) { wattron(_win, COLOR_PAIR(pair_title)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_msg);
  if (pair_title != -1) { wattroff(_win, COLOR_PAIR(pair_title)); }
  wattroff(_win, A_BOLD);

  // Info on bottom of window

  msglen = _info.size();
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  pair_info = colors.pair(fg_info, bg_info);
  if (pair_info != -1) { wattron(_win, COLOR_PAIR(pair_info)); }
  wattron(_win, A_BOLD);
  printSpaces(left-1);
  printToEol(_info);
  if (pair_info != -1) { wattroff(_win, COLOR_PAIR(pair_info)); }
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
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Horizontal dividers for header, footer, and text input

  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, 5, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, rows-3, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Connections

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, 5, 0, ACS_LTEE);
  mvwaddch(_win, 5, cols-1, ACS_RTEE);
  mvwaddch(_win, rows-3, 0, ACS_LTEE);
  mvwaddch(_win, rows-3, cols-1, ACS_RTEE);
}

/*******************************************************************************

Constructors

*******************************************************************************/
SearchBox::SearchBox()
{
  _reserved_rows = 7;
  _msg = "Search repository";

  _label.setWidth(30);
  _label.setName("Search term:");
  _entryitem.setWidth(30);
  _caseitem.setName("Case sensitive");
  _caseitem.disable();
  _caseitem.setWidth(30);
  _wholeitem.setName("Whole word");
  _wholeitem.disable();
  _wholeitem.setWidth(30);

  addItem(&_label);
  addItem(&_entryitem);
  addItem(&_caseitem);
  addItem(&_wholeitem);
  _label.setPosition(3,1);
  _entryitem.setPosition(4,1);
  _caseitem.setPosition(6,1);
  _wholeitem.setPosition(7,1);
}

SearchBox::SearchBox(WINDOW *win, const std::string & msg)
{
  _win = win;
  _msg = msg;
  _reserved_rows = 7;

  _label.setWidth(30);
  _label.setName("Search term:");
  _entryitem.setWidth(30);
  _caseitem.setName("Case sensitive");
  _caseitem.disable();
  _caseitem.setWidth(30);
  _wholeitem.setName("Whole word");
  _wholeitem.disable();
  _wholeitem.setWidth(30);

  addItem(&_label);
  addItem(&_entryitem);
  addItem(&_caseitem);
  addItem(&_wholeitem);
  _label.setPosition(3,1);
  _entryitem.setPosition(4,1);
  _caseitem.setPosition(6,1);
  _wholeitem.setPosition(7,1);
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void SearchBox::clearSearch() { _entryitem.clear(); }

/*******************************************************************************

Get attributes

*******************************************************************************/
std::string SearchBox::searchString() const
{ 
  return _entryitem.getStringProp(); 
}

bool SearchBox::caseSensitive() const { return _caseitem.getBoolProp(); }
bool SearchBox::wholeWord() const { return _wholeitem.getBoolProp(); }
