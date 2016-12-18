#include <string>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max, min
#include "Color.h"
#include "color_settings.h"
#include "ListBox.h" // quitSignal
#include "InputBox.h"

using namespace color;

/*******************************************************************************

Prints to end of line, padding with spaces and avoiding borders

*******************************************************************************/
void InputBox::printToEol(const std::string & msg) const
{
  int i, y, x, rows, cols, nspaces, msglen;

  getmaxyx(_win, rows, cols);
  getyx(_win, y, x);

  /* Math: Number of columns: cols
           Cursor position: x
           Number of spaces that can be printed to right = cols-1-x
           => Because the last column is taken up by the border */

  msglen = msg.size();
  if (msglen > cols-1-x) { wprintw(_win, msg.substr(0, cols-1-x).c_str()); }
  else
  {
    nspaces = std::max(cols-1-x-msglen, 0);
    wprintw(_win, msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { wprintw(_win, " "); }
  }
}

/*******************************************************************************

Prints a given number of spaces

*******************************************************************************/
void InputBox::printSpaces(unsigned int nspaces) const
{
  unsigned int i;

  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
}

/*******************************************************************************

Determine first character to print in text entry. Returns 0 if it has not
changed; 1 if it has.

*******************************************************************************/
unsigned int InputBox::determineFirstText()
{
  int rows, cols;
  unsigned int firsttextstore;

  getmaxyx(_win, rows, cols);
  firsttextstore = _firsttext;
  if (int(_entry.size()) < cols-3) { _firsttext = 0; }
  else 
  { 
    if (int(_cursidx) - int(_firsttext) > cols-3)
    {
      _firsttext = _cursidx - cols + 3;
    }
  }

  if (firsttextstore == _firsttext) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Draws window border, message, and info

*******************************************************************************/
void InputBox::redrawFrame() const
{
  unsigned int rows, cols, msglen, i;
  int left, pair_title, pair_info;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  msglen = _msg.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(msglen)/2.0);
  wmove(_win, 1, 0);
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
  left = std::floor(mid - double(msglen)/2.0);
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

Redraws user input

*******************************************************************************/
void InputBox::redrawInput()
{
  int rows, cols, pair_input, numprint;

  getmaxyx(_win, rows, cols);
  numprint = std::min(cols-1, int(_entry.size() - _firsttext));
  
  wmove(_win, 3, 1);
  pair_input = colors.pair(fg_highlight_active, bg_highlight_active);
  if (pair_input != -1) { wattron(_win, COLOR_PAIR(pair_input)); }
  printToEol(_entry.substr(_firsttext, numprint));
  if (pair_input != -1) { wattroff(_win, COLOR_PAIR(pair_input)); }
}

/*******************************************************************************

Constructors

*******************************************************************************/
InputBox::InputBox()
{
  _win = NULL;
  _msg = "";
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _entry = "";
  _firsttext = 0;
  _cursidx = 0;
}

InputBox::InputBox(WINDOW *win, const std::string & msg)
{
  _win = win;
  _msg = msg;
  _info = "Enter: Ok | Esc: Cancel";
  _redraw_type = "all";
  _entry = "";
  _firsttext = 0;
  _cursidx = 0;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void InputBox::setWindow(WINDOW *win) { _win = win; }
void InputBox::setMessage(const std::string & msg) { _msg = msg; }
void InputBox::setInfo(const std::string & info) { _info = info; }

/*******************************************************************************

Get attributes

*******************************************************************************/
void InputBox::minimumSize(int & height, int & width) const
{
  int reserved_cols;

  // No scrolling is required for this - just give the number of rows needed

  height = 7;

  // Minimum usable width (at least 2 characters visible in entry)

  width = std::max(_msg.size(), _info.size());
  width = std::max(width, 2);
  reserved_cols = 2;       // For frame border
  width += reserved_cols;
}

void InputBox::preferredSize(int & height, int & width) const
{
  int reserved_cols;

  // No scrolling is required for this - just give the number of rows needed
 
  height = 7;

  // Preferred width (at least 27 characters visible in entry)

  width = std::max(_msg.size(), _info.size());
  width = std::max(width, 27);
  reserved_cols = 2;
  width += reserved_cols;
}

/*******************************************************************************

Draws input box (frame, entry, etc.) as needed

*******************************************************************************/
void InputBox::draw(bool force)
{
  int pair_popup;

  if (force) { _redraw_type = "all"; }

  // Draw input box elements

  if (_redraw_type == "all")
  {
    wclear(_win);
    pair_popup = colors.pair(fg_popup, bg_popup);
    if (pair_popup != -1) { wbkgd(_win, COLOR_PAIR(pair_popup)); }
  }
  if (_redraw_type != "none") { redrawFrame(); }
  if ( (_redraw_type == "all") || (_redraw_type == "input") ) { redrawInput(); }
  wmove(_win, 3, _cursidx - _firsttext + 1);
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or selected item name

*******************************************************************************/
std::string InputBox::exec()
{
  int ch, rows, cols;
  bool getting_input;
  std::string retval;
  unsigned int check_redraw;

  const int MY_DELETE = 330;
  const int MY_ESC = 27;

  curs_set(1);
  getmaxyx(_win, rows, cols);
  determineFirstText();

  getting_input = true;
  while (getting_input)
  {

    // Draw input box elements
  
    draw();
    _redraw_type = "input";

    // Get user input

    switch (ch = getch()) {

      // Enter key: return entry

      case '\n':
      case '\r':
      case KEY_ENTER: 
        retval = _entry;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Backspace key pressed: delete previous character.
      // Note: some terminals define it as 8, others as 127

      case 8:
      case 127:
      case KEY_BACKSPACE:
        if (_cursidx > 0)
        {
          _entry.erase(_cursidx-1,1);
          _cursidx--;
          if (_cursidx < _firsttext)
          {
            if (int(_cursidx) > cols-3) { _firsttext = _cursidx - cols + 3; }
            else { _firsttext = 0; }
          }
        }
        break;

      // Delete key pressed: delete current character

      case MY_DELETE:
        if (_cursidx < _entry.size()) { _entry.erase(_cursidx,1); }
        break;      

      // Navigation keys

      case KEY_LEFT:
        if (_cursidx > 0) { _cursidx--; }
        if (_cursidx < _firsttext) { _firsttext = _cursidx; }
        else { _redraw_type = "none"; }
        break;
      case KEY_RIGHT:
        if (_cursidx < _entry.size()) { _cursidx++; }
        check_redraw = determineFirstText();
        if (check_redraw == 0) { _redraw_type = "none"; }
        break;
      case KEY_HOME:
        if (_cursidx == 0) { _redraw_type = "none"; }
        _cursidx = 0;
        _firsttext = 0;
        break;
      case KEY_END:
        _cursidx = _entry.size();
        check_redraw = determineFirstText();
        if (check_redraw == 0) { _redraw_type = "none"; }
        break;

      // Resize signal
    
      case KEY_RESIZE:
        retval = ListBox::resizeSignal;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Quit key

      case MY_ESC:
        retval = ListBox::quitSignal;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Add character to entry

      default:
        _entry.insert(_cursidx, 1, ch);
        _cursidx++;
        determineFirstText();
        break;
    }
  }

  curs_set(0);
  return retval;
}
