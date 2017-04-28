#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // min, max
#include "Color.h"
#include "settings.h"
#include "TextInput.h"
#include "ToggleInput.h"
#include "Label.h"
#include "SearchBox.h"

/*******************************************************************************

Draws window border, message, and info

*******************************************************************************/
void SearchBox::redrawFrame() const
{
  int rows, cols, msglen, i, left;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  msglen = _msg.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, 1, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, color_settings.fg_title, color_settings.bg_title);
  printSpaces(left-1);
  printToEol(_msg);
  colors.turnOff(_win);

  // Info on bottom of window

  msglen = _info.size();
  left = std::floor(mid - double(msglen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, color_settings.fg_info, color_settings.bg_info);
  printSpaces(left-1);
  printToEol(_info);
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

Constructors and destructor

*******************************************************************************/
SearchBox::SearchBox()
{
  _firstprint = _header_rows;
  _msg = "Search repository";

  addItem(new Label());
  _items[0]->setWidth(30);
  _items[0]->setName("Search term:");
  _items[0]->setPosition(3,1);

  addItem(&_entryitem);
  _entryitem.setWidth(30);
  _entryitem.setPosition(4,1);

  addItem(&_caseitem);
  _caseitem.setName("Case sensitive");
  _caseitem.setEnabled(false);
  _caseitem.setWidth(30);
  _caseitem.setPosition(6,1);

  addItem(&_wholeitem);
  _wholeitem.setName("Whole words only");
  _wholeitem.setEnabled(false);
  _wholeitem.setWidth(30);
  _wholeitem.setPosition(7,1);

  addItem(&_readmeitem);
  _readmeitem.setName("Search READMEs");
  _readmeitem.setEnabled(false);
  _readmeitem.setWidth(30);
  _readmeitem.setPosition(8,1);
}

SearchBox::~SearchBox() { delete _items[0]; }

/*******************************************************************************

Set attributes

*******************************************************************************/
void SearchBox::clearSearch() { _entryitem.clear(); }

/*******************************************************************************

Get attributes

*******************************************************************************/
std::string SearchBox::searchString() const { return _entryitem.text(); }
bool SearchBox::caseSensitive() const { return _caseitem.enabled(); }
bool SearchBox::wholeWord() const { return _wholeitem.enabled(); }
bool SearchBox::searchREADMEs() const { return _readmeitem.enabled(); }

/*******************************************************************************

Redraws box and all input items

*******************************************************************************/
void SearchBox::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  if (_redraw_type == "all") 
  { 
    wclear(_win);
    colors.setBackground(_win, color_settings.fg_popup,
                               color_settings.bg_popup);
    redrawFrame();
    redrawAllItems(force);
  }
  else { redrawChangedItems(force); }
  wrefresh(_win);
}
