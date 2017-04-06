#include <string>
#include <vector>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // min
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "string_util.h"
#include "MessageBox.h"

/*******************************************************************************

Draws window border and title

*******************************************************************************/
void MessageBox::redrawFrame() const
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
  if (_header_colorize) { colors.turnOn(_win, color_settings.fg_title,
                                              color_settings.bg_title); }
  else { wattron(_win, A_BOLD); }
  printSpaces(left-1);
  printToEol(_name);
  if (_header_colorize) { colors.turnOff(_win); }
  else { wattroff(_win, A_BOLD); }

  // Info on bottom of window

  namelen = _info.size();
  left = std::floor(mid - double(namelen)/2.0) + 1;
  wmove(_win, rows-2, 1);
  wclrtoeol(_win);
  if (_header_colorize) { colors.turnOn(_win, color_settings.fg_title,
                                              color_settings.bg_title); }
  else { wattron(_win, A_BOLD); }
  printSpaces(left-1);
  printToEol(_info);
  if (_header_colorize) { colors.turnOff(_win); }
  else { wattroff(_win, A_BOLD); }

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

Prints message in box within defined margins

*******************************************************************************/
void MessageBox::redrawMessage() const
{
  unsigned int i, max_lines, len;
  int rows, cols, left;
  double mid;
  std::vector<std::string> wrapped_message;

  getmaxyx(_win, rows, cols);
  wrapped_message = wrap_words(_message, cols-2-2*_margin_h);

  max_lines = wrapped_message.size();
  for ( i = 0; i < max_lines; i++ )
  {
    len = wrapped_message[i].size();
    mid = double(cols-2)/2.0;
    left = std::floor(mid - double(len)/2.0) + 1;
    wmove(_win, 3+_margin_v+i, 1);
    if (_centered) { printSpaces(left-1); }
    wprintw(_win, wrapped_message[i].c_str());
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
MessageBox::MessageBox(bool header_colorize, bool centered)
{
  _win = NULL;
  _name = "";
  _message = "";
  _info = "Enter: Ok | Esc: Cancel";
  _margin_v = 0;
  _margin_h = 0;
  _color_idx = -1;
  _header_colorize = header_colorize;
  _centered = centered;
}

MessageBox::MessageBox(WINDOW *win, const std::string & name,
                       bool header_colorize, bool centered)
{
  _win = win;
  _name = name;
  _message = "";
  _info = "Enter: Ok | Esc: Cancel";
  _margin_v = 0;
  _margin_h = 0;
  _color_idx = -1;
  _header_colorize = header_colorize;
  _centered = centered;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void MessageBox::setName(const std::string & name) { _name = name; }
void MessageBox::setMessage(const std::string & msg) { _message = msg; }
void MessageBox::setInfo(const std::string & info) { _info = info; }
void MessageBox::setColor(int color_idx) { _color_idx = color_idx; }

/*******************************************************************************

Get attributes

*******************************************************************************/
void MessageBox::minimumSize(int & height, int & width) const
{
  int reserved_rows, reserved_cols, msg_width;

  reserved_rows = 6 + 2*_margin_v;
  reserved_cols = 2;

  // Minimum usable width -- pick some reasonable number (message width 15)

  width = std::max(_name.size(), _info.size()) + reserved_cols;
  width = std::max(width, 15);
  msg_width = width-2-2*_margin_h;

  // Minimum usable height based on width and message

  height = reserved_rows + wrap_words(_message, msg_width).size();
}

void MessageBox::preferredSize(int & height, int & width) const
{
  int reserved_rows, reserved_cols, msg_width;
  std::vector<std::string> wrapped_msg;
  unsigned int i, nlines;

  reserved_rows = 6 + 2*_margin_v;
  reserved_cols = 2;

  // Preferred width -- pick some reasonable number (message width 50)

  width = std::max(_name.size(), _info.size()) + reserved_cols;
  width = std::max(width, int(30+2+2*_margin_h));
  msg_width = width-2-2*_margin_h;

  // Required height based on width and message

  wrapped_msg = wrap_words(_message, msg_width);
  nlines = wrapped_msg.size(); 
  height = reserved_rows + nlines;

  // Check width and adjust height accordingly

  for ( i = 0; i < nlines; i++ )
  {
    if (int(wrapped_msg[i].size()) > msg_width)
      msg_width = wrapped_msg[i].size(); 
  }
  width = msg_width+2+2*_margin_h;
  wrapped_msg = wrap_words(_message, msg_width);
  nlines = wrapped_msg.size(); 
  height = reserved_rows + nlines;
}

/*******************************************************************************

Draws box (frame, message). Since everything must be redrawn whenever this is
called, there is no _redraw_type for it.

*******************************************************************************/
void MessageBox::draw(bool force)
{
  wclear(_win);
  if (_color_idx == -1)
    colors.setBackground(_win, color_settings.fg_warning,
                               color_settings.bg_warning);
  else { colors.setBackground(_win, _color_idx); }
  redrawFrame();
  redrawMessage();
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or other signal

*******************************************************************************/
std::string MessageBox::exec()
{
  int ch;
  std::string retval;
  bool getting_input;

  const int MY_ESC = 27;

  // Get user input

  getting_input = true;
  while (getting_input)
  {
   
    // Draw message box

    draw();

    // Get user input

    switch (ch = getch()) {
  
      // Enter key

      case '\n':
      case '\r':
      case KEY_ENTER:
        retval = signals::keyEnter;
        getting_input = false;
        break;

      // Resize signal

      case KEY_RESIZE:
        retval = signals::resize;
        getting_input = false;
        break;

      // Quit key

      case MY_ESC:
        retval = signals::quit;
        getting_input = false;
        break;

      default:
        break;
    }
  }
  
  return retval;
}

