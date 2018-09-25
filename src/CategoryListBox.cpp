#include <vector>
#include <string>
#include <cmath>   // floor
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "BuildListItem.h"
#include "BuildListBox.h"
#include "CategoryListBox.h"


/*******************************************************************************

Draws window border, title, and header

*******************************************************************************/
void CategoryListBox::redrawFrame()
{
  int rows, cols, namelen, i, left, right;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols)/2.0;
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

  // Right border for header

  mvwaddch(_win, 1, cols-1, ACS_VLINE);

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Draw header

  wmove(_win, 1, 1);
  colors.turnOn(_win, "header", "bg_normal");
  printToEol("Name");
  colors.turnOff(_win);
  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Draw connections between horizontal and vertical lines

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void CategoryListBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+_header_rows, 1);

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

  // Print item

  printToEol(_items[idx]->name());

  // Turn off highlight color

  if (colors.turnOff(_win) != 0)
  {
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
    if (_items[idx]->getBoolProp("tagged")) { wattroff(_win, A_BOLD); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
CategoryListBox::CategoryListBox()
{
  _reserved_rows = 4;
  _header_rows = 3;
}

CategoryListBox::CategoryListBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _reserved_rows = 4;
  _header_rows = 3;
}

/*******************************************************************************

User interaction: returns key stroke or other signal. This is the same
as the one for BuildListBox, except that tagging doesn't highlight the next
entry.

*******************************************************************************/
std::string CategoryListBox::exec(MouseEvent * mevent)
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
      retval = signals::keyEnter;
      _redraw_type = "all";
      break;

    // Tab key: return keyTab

    case MY_TAB:
      retval = signals::keyTab;
      _redraw_type = "changed";
      break;

    // Left/right arrows

    case KEY_LEFT:
      retval = signals::keyLeft;
      _redraw_type = "changed";
      break;
    case KEY_RIGHT:
      retval = signals::keyRight;
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

    // t or T: tag item

    case 't':
      retval = "t";
      _items[_highlight]->setBoolProp("tagged", 
                                 (! _items[_highlight]->getBoolProp("tagged")));
      check_redraw = highlightNext();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
      break;

    case 'T':
      retval = "T";
      _items[_highlight]->setBoolProp("tagged", 
                                 (! _items[_highlight]->getBoolProp("tagged")));
      check_redraw = highlightPrevious();
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
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
