#include <vector>
#include <string>
#include <cmath>     // floor
#include <algorithm> // min
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "TagList.h"
#include "BuildListItem.h"
#include "BuildListBox.h"

/*******************************************************************************

Draws window border, title, and header

*******************************************************************************/
void BuildListBox::redrawFrame()
{
  int rows, cols, namelen, i, nspaces, vlineloc, left, right;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  wattron(_win, A_BOLD);
  wprintw(_win, "%s", _name.c_str());
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

  // Right border for header

  mvwaddch(_win, 1, cols-1, ACS_VLINE);
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Draw header

  wmove(_win, 1, 1);
  colors.turnOn(_win, "header", "bg_normal");
  wprintw(_win, "Name");
  colors.turnOff(_win);

  vlineloc = cols-2 - std::string("Installed").size();
  nspaces = vlineloc - std::string("Name").size();
  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }

  colors.turnOn(_win, "header", "bg_normal");
  printToEol("Installed");
  wmove(_win, 2, 1);
  colors.turnOff(_win);

  // Draw horizontal and then vertical line

  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  for ( i = 3; i < rows-1; i++ ) { mvwaddch(_win, i, vlineloc, ACS_VLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, 2, vlineloc, ACS_TTEE);
  mvwaddch(_win, rows-1, vlineloc, ACS_BTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void BuildListBox::redrawSingleItem(unsigned int idx)
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
    if (_activated) 
    { 
      if (_items[idx]->getBoolProp("tagged")) { fg = "tagged"; }
      else { fg = "fg_highlight_active"; }
      bg = "bg_highlight_active"; 
    }
    else
    {
      if (_items[idx]->getBoolProp("tagged")) { fg = "tagged"; }
      else { fg = "fg_highlight_inactive"; }
      bg = "bg_highlight_inactive"; 
    }
    if (colors.turnOn(_win, fg, bg) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
      if (_items[idx]->getBoolProp("tagged")) { wattron(_win, A_BOLD); } 
    }
  } 
  else
  {
    if (_items[idx]->getBoolProp("tagged")) { fg = "tagged"; }
    else { fg = "fg_normal"; }
    bg = "bg_normal";
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
  wprintw(_win, "%s", _items[idx]->name().substr(0,printlen).c_str());
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
BuildListBox::BuildListBox()
{ 
  _reserved_rows = 4; 
  _header_rows = 3;
  _taglist = NULL;
}
BuildListBox::BuildListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _reserved_rows = 4;
  _header_rows = 3;
  _taglist = NULL;
}

/*******************************************************************************

Checks if all items are tagged

*******************************************************************************/
bool BuildListBox::allTagged() const
{
  unsigned int i, nitems;
  bool all_tagged;

  nitems = _items.size();
  all_tagged = true;
  for ( i = 0; i < nitems; i++ )
  {
    if (! _items[i]->getBoolProp("tagged")) 
    {
      all_tagged = false;
      break;
    }
  }

  return all_tagged;
}

/*******************************************************************************

Sets a pointer to the tag list

*******************************************************************************/
void BuildListBox::setTagList(TagList *taglist) { _taglist = taglist; }

/*******************************************************************************

Tags or untags a single item

*******************************************************************************/
void BuildListBox::tagSlackBuild(unsigned int idx)
{
  if (idx < numItems())
  {
    _items[idx]->setBoolProp("tagged", ! _items[idx]->getBoolProp("tagged"));
    if (_items[idx]->getBoolProp("tagged")) { _taglist->addItem(_items[idx]); }
    else { _taglist->removeItem(_items[idx]); }
  }
}
void BuildListBox::tagHighlightedSlackBuild() { tagSlackBuild(_highlight); }

/*******************************************************************************

Tags or untags (if everything is already tagged) items. Returns 0 if the
operation tags everything, and 1 if it untags everything.

*******************************************************************************/
unsigned int BuildListBox::tagAll()
{
  unsigned int i, nitems, retval;

  // First check if everything is tagged

  nitems = _items.size();
  if (! allTagged())
  {
    for ( i = 0; i < nitems; i++ ) 
    { 
      if (! _items[i]->getBoolProp("tagged")) 
      { 
        _items[i]->setBoolProp("tagged", true); 
        _taglist->addItem(_items[i]);
      }
    }
    retval = 0;
  }
  else
  {
    for ( i = 0; i < nitems; i++ ) 
    { 
      if (_items[i]->getBoolProp("tagged")) 
      {
        _items[i]->setBoolProp("tagged", false); 
        _taglist->removeItem(_items[i]);
      }
    }
    retval = 1;
  }

  // Set all items to be redrawn

  _redraw_type = "items";

  return retval;
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string BuildListBox::exec(MouseEvent * mevent)
{
  int ch, check_redraw;
  std::string retval;
  MEVENT event;

  const int MY_ESC = 27;
  const int MY_TAB = 9;

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
      _redraw_type = "all";
      if (int(_button_signals.size()) >= _highlighted_button+1)
        retval = _button_signals[_highlighted_button];
      else
        retval = signals::keyEnter;
      break;

    // Tab key: return keyTab

    case MY_TAB:
      retval = signals::keyTab;
      _redraw_type = "changed";
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

    // Right and left keys: change highlighted button (if present)

    case KEY_RIGHT:
      retval = signals::keyRight;
      check_redraw = highlightNextButton();
      _redraw_type = "changed";
      break;

    case KEY_LEFT:
      retval = signals::keyLeft;
      check_redraw = highlightPreviousButton();
      _redraw_type = "changed";
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

    // t and T: tag item

    case 't':
      retval = signals::tag;
      _redraw_type = "changed";
      break;

    case 'T':
      retval = signals::tag;
      _redraw_type = "changed";
      break;

    // F9 key: activates menubar

    case KEY_F(9):
      retval = signals::keyF9;
      _redraw_type = "changed";
      break;

    // Mouse

    case KEY_MOUSE:
      if ( (getmouse(&event) == OK) && mevent )
      {
        mevent->recordClick(event);
        _redraw_type = "changed";
        retval = signals::mouseEvent;
      }
      else
        return signals::nullEvent;
      break;

    default:
      retval = char(ch);
      _redraw_type = "none";
      break;
  }
  return retval;
}
