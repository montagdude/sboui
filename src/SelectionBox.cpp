#include <string>
#include <curses.h>
#include <cmath>    // floor
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "ListItem.h"
#include "SelectionBox.h"

/*******************************************************************************

Draws window border, title, and info

*******************************************************************************/
void SelectionBox::redrawFrame() const
{
  int rows, cols, namelen, i, left;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _name.size();
  mid = double(cols-2)/2.0;
  left = std::floor(mid - double(namelen)/2.0) + 1;
  wmove(_win, 1, 1);
  wclrtoeol(_win);
  colors.turnOn(_win, color_settings.fg_title, color_settings.bg_title);
  printSpaces(left-1);
  printToEol(_name);
  colors.turnOff(_win);

  // Info on bottom of window

  namelen = _info.size();
  left = std::floor(mid - double(namelen)/2.0) + 1;
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

  // Right border for header and footer

  mvwaddch(_win, 1, cols-1, ACS_VLINE);
  mvwaddch(_win, rows-2, cols-1, ACS_VLINE);

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Horizontal dividers for header and footer

  wmove(_win, 2, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, rows-3, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Connections

  mvwaddch(_win, 2, 0, ACS_LTEE);
  mvwaddch(_win, 2, cols-1, ACS_RTEE);
  mvwaddch(_win, rows-3, 0, ACS_LTEE);
  mvwaddch(_win, rows-3, cols-1, ACS_RTEE);
}

/*******************************************************************************
 
Redraws a single item. Note: doesn't check if the item is actually on the
screen or not.

*******************************************************************************/
void SelectionBox::redrawSingleItem(unsigned int idx)
{
  std::string fg, bg;
  int color_pair1, color_pair2, i, len, hidx;

  // Go to item location, optionally highlight, and print item

  wmove(_win, idx-_firstprint+_header_rows, 1);

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
    color_pair1 = colors.getPair(fg, bg);
    color_pair2 = colors.getPair(color_settings.hotkey, bg);
    if (colors.turnOn(_win, color_pair1) != 0)
    { 
      if (_activated) { wattron(_win, A_REVERSE); }
    }
  } 
  else 
  { 
    color_pair1 = colors.getPair(color_settings.fg_popup,
                                 color_settings.bg_popup);
    color_pair2 = colors.getPair(color_settings.hotkey,
                                 color_settings.bg_popup);
  }

  // Save highlight idx for redrawing later.
  // Note: prevents this method from being const.
  
  if (int(idx) == _highlight) { _prevhighlight = _highlight; }

  // Print item

  len = _items[idx]->name().size();
  hidx = _items[idx]->hotKey();
  for ( i = 0; i < len; i++ )
  {
    if ( i == hidx )
    { 
      colors.turnOff(_win);
      if (colors.turnOn(_win, color_pair2) != 0) { wattron(_win, A_BOLD); }
      wprintw(_win, _items[idx]->name().substr(i,1).c_str());
      if (colors.turnOff(_win) != 0) { wattroff(_win, A_BOLD); } 
      colors.turnOn(_win, color_pair1);
    }
    else { wprintw(_win, _items[idx]->name().substr(i,1).c_str()); }
  }
  printToEol(std::string(""));

  // Turn off highlight color

  if (colors.turnOff(_win) != 0)
  { 
    if ( (int(idx) == _highlight) && _activated ) { wattroff(_win, A_REVERSE); }
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
SelectionBox::SelectionBox()
{
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;
  _header_rows = 3;
}

SelectionBox::SelectionBox(WINDOW *win, const std::string & name)
{
  _win = win;
  _name = name;
  _info = "Enter: Ok | Esc: Cancel";
  _reserved_rows = 6;
  _header_rows = 3;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void SelectionBox::setInfo(const std::string & info) { _info = info; }

/*******************************************************************************

Get attributes

*******************************************************************************/
void SelectionBox::minimumSize(int & height, int & width) const
{
  int namelen, reserved_cols;
  unsigned int i, nitems;

  // Minimum usable height

  height = _reserved_rows + 2;

  // Minimum usable width

  reserved_cols = 2;
  width = _name.size();
  if (int(_info.size()) > width) { width = _info.size(); }
  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols;
}

void SelectionBox::preferredSize(int & height, int & width) const
{
  int namelen, reserved_cols, widthpadding;
  unsigned int i, nitems;

  // Preferred height: no scrolling

  nitems = _items.size();
  height = _reserved_rows + nitems;

  // Preferred width: minimum usable + some padding

  widthpadding = 6;
  reserved_cols = 2;
  width = _name.size();
  if (int(_info.size()) > width) { width = _info.size(); }
  for ( i = 0; i < nitems; i++ )
  {
    namelen = _items[i]->name().size();
    if (namelen > width) { width = namelen; }
  }
  width += reserved_cols + widthpadding;
}

/*******************************************************************************

Draws box (frame, items, etc.) as needed

*******************************************************************************/
void SelectionBox::draw(bool force)
{
  if (force) { _redraw_type = "all"; }

  // Draw list elements

  if (_redraw_type == "all")
  {
    wclear(_win);
    colors.setBackground(_win, color_settings.fg_popup,
                               color_settings.bg_popup);
  }
  if (_redraw_type != "none")
  { 
    redrawFrame(); 
    redrawScrollIndicator();
  }
  if ( (_redraw_type == "all") || (_redraw_type == "items") ) {
                                                            redrawAllItems(); }
  else if (_redraw_type == "changed") { redrawChangedItems(); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: show display until user hits Enter or Esc

*******************************************************************************/
std::string SelectionBox::exec()
{
  int ch, check_redraw, hotkey;
  char ch_char, hotcharN, hotcharL;
  std::string retval;
  bool getting_input, check_hotkeys;
  unsigned int i;

  const int MY_ESC = 27;

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  getting_input = true;
  while (getting_input)
  {

    check_hotkeys = false;

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
        getting_input = false;
        break;
  
      // Arrows/Home/End/PgUp/Dn: change highlighted value
  
      case KEY_UP:
        check_redraw = highlightPrevious();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_DOWN:
        check_redraw = highlightNext();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_PPAGE:
        check_redraw = highlightPreviousPage();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_NPAGE:
        check_redraw = highlightNextPage();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_HOME:
        check_redraw = highlightFirst();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
      case KEY_END:
        check_redraw = highlightLast();
        if (check_redraw == 1) { _redraw_type = "all"; }
        else { _redraw_type = "changed"; }
        break;
  
      // Resize signal
  
      case KEY_RESIZE:
        retval = signals::resize;
        _redraw_type = "all";
        getting_input = false;
        break;
  
      // Quit key
  
      case MY_ESC:
        retval = signals::quit;
        _redraw_type = "all";
        getting_input = false;
        break;

      default:
        _redraw_type = "none";
        check_hotkeys = true;
        break;
    }
      
    // Handle hotkeys (allow upper and lower case)

    if (check_hotkeys)
    {
      ch_char = char(ch);
      for ( i = 0; i < numItems(); i++ )
      {
        hotkey = _items[i]->hotKey();
        if (hotkey != -1)
        {
          hotcharN = _items[i]->name()[hotkey];
          hotcharL = std::tolower(_items[i]->name()[hotkey]);
          if ( (ch_char == hotcharN) || (ch_char == hotcharL) )
          {
            retval = hotcharN;
            _redraw_type = "all";
            getting_input = false;
            break;
          }
        }
      }
    }

  }
  return retval;
}
