#include <curses.h>
#include <cmath>     // floor
#include "Color.h"
#include "settings.h"
#include "CursesWidget.h"
#include "MouseEvent.h"

/*******************************************************************************

Prints to end of line, padding with spaces and avoiding borders. Renders pipes
as ACS_VLINEs.

printable: number of spaces that can be printed on, including the current column
    and any to the right. Defaults to window width - x - 1 (space for border).

*******************************************************************************/
void CursesWidget::printToEol(const std::string & msg, int printable) const
{
  int i, y, x, rows, cols, nprint, nspaces, msglen;

  getmaxyx(_win, rows, cols);
  getyx(_win, y, x);

  /* Math: Number of columns: cols
           Cursor position: x
           Number of spaces that can be printed to right = printable-msglen
           Default printable: cols-x-1
           => Because the last column is taken up by the border */

  msglen = msg.size();
  if (printable == -1) { printable = cols-x-1; }

  nprint = std::min(msglen, printable);
  for ( i = 0; i < nprint; i++ )
  {
    if (msg[i] == '|') { waddch(_win, ACS_VLINE); }
    else { waddch(_win, msg[i]); }
  }
  nspaces = std::max(printable-msglen, 0);
  printSpaces(nspaces);
}

/*******************************************************************************

Prints a given number of spaces

*******************************************************************************/
void CursesWidget::printSpaces(int nspaces) const
{
  int i;

  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
}

/*******************************************************************************

Clears window line-by-line, as wclear can cause flickering

*******************************************************************************/
void CursesWidget::clearWindow() const
{
  int rows, cols, i;

  getmaxyx(_win, rows, cols);

  for ( i = 0; i < rows; i++ )
  {
    wmove(_win, i, 0);
    wclrtoeol(_win);
  }
}

/*******************************************************************************

Highlights next button. Return value of 0 means that the highlighted button has
not changed; 1 means that it has.

*******************************************************************************/
int CursesWidget::highlightNextButton()
{
  if (_buttons.size() < 2)
    return 0;
  else if (_highlighted_button < int(_buttons.size())-1)
  {
    _highlighted_button += 1;
    return 1;
  }
  else
    return 0;
}

/*******************************************************************************

Highlights previous button. Return value of 0 means that the highlighted button
has not changed; 1 means that it has.

*******************************************************************************/
int CursesWidget::highlightPreviousButton()
{
  if (_buttons.size() < 2)
    return 0;
  else if (_highlighted_button > 0)
  {
    _highlighted_button -= 1;
    return 1;
  }
  else
    return 0;
}

/*******************************************************************************

Redraws buttons at bottom of list box

*******************************************************************************/
void CursesWidget::redrawButtons()
{
  int rows, cols, nbuttons, namelen, i, left, color_pair;
  double mid;

  getmaxyx(_win, rows, cols);

  nbuttons = _buttons.size();
  if (nbuttons > 0)
  {
    namelen = 0;
    for ( i = 0; i < nbuttons; i++ )
    {
      namelen += _buttons[i].size();
    }
    mid = double(cols-2)/2.;
    left = std::floor(mid - double(namelen)/2.0) + 1;
    _button_left[0] = left;
    _button_right[0] = _button_left[0] + _buttons[0].size()-1;
    for ( i = 1; i < nbuttons; i++ )
    {
      _button_left[i] = _button_right[i-1] + 1;
      _button_right[i] = _button_left[i] + _buttons[i].size()-1;
    }
    color_pair = colors.getPair(_button_fg, _button_bg);
    wmove(_win, rows-2, left);
    for ( i = 0; i < nbuttons; i++ )
    {
      if (i == _highlighted_button)
      {
        if (colors.turnOn(_win, color_pair) != 0)
          wattron(_win, A_REVERSE);
        wprintw(_win, _buttons[i].c_str());
        if (colors.turnOff(_win) != 0)
          wattroff(_win, A_REVERSE);
      }
      else
        wprintw(_win, _buttons[i].c_str());
    }
  }
}

/*******************************************************************************

Constructor

*******************************************************************************/
CursesWidget::CursesWidget()
{
  _win = NULL;
  _buttons.resize(0);
  _button_left.resize(0);
  _button_right.resize(0);
  _button_signals.resize(0);
  _button_bg = "bg_highlight_active";
  _button_fg = "fg_highlight_active";
  _fg_color = "fg_normal";
  _bg_color = "bg_normal";
  _highlighted_button = 0;
  _redraw_type = "all";
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void CursesWidget::setWindow(WINDOW *win) { _win = win; }

void CursesWidget::addButton(const std::string & button,
                             const std::string & signal)
{
  _buttons.push_back(button);
  _button_signals.push_back(signal);
  _button_left.resize(_buttons.size());
  _button_right.resize(_buttons.size());
  _highlighted_button = 0;
  if (_reserved_rows == _header_rows+1)
    _reserved_rows += 2;
}

void CursesWidget::clearButtons()
{
  _buttons.resize(0);
  _button_signals.resize(0);
  _button_left.resize(0);
  _button_right.resize(0);
  _highlighted_button = 0;
}

void CursesWidget::setButtons(const std::vector<std::string> & buttons,
                              const std::vector<std::string> & button_signals)
{
  clearButtons();
  _buttons = buttons;
  _button_signals = button_signals;
  _button_left.resize(_buttons.size());
  _button_right.resize(_buttons.size());
  _highlighted_button = 0;
}

void CursesWidget::setButtonColor(const std::string & button_fg,
                                  const std::string & button_bg)
{
  _button_fg = button_fg;
  _button_bg = button_bg;
}

void CursesWidget::setColor(const std::string & fg_color,
                            const std::string & bg_color)
{
  _fg_color = fg_color;
  _bg_color = bg_color;
}

/*******************************************************************************

Get properties

*******************************************************************************/
void CursesWidget::popupSize(int & height, int & width,
                             CursesWidget *popup) const
{
  int minheight, minwidth, prefheight, prefwidth, maxheight, maxwidth;
  int rows, cols;

  getmaxyx(stdscr, rows, cols);

  popup->minimumSize(minheight, minwidth);
  popup->preferredSize(prefheight, prefwidth);
  maxheight = rows-4;
  maxwidth = cols-4;

  if (prefheight < maxheight) { height = prefheight; }
  else 
  { 
    if (maxheight-1 > minheight) { height = maxheight-1; }
    else { height = minheight; }
  }
  if (prefwidth < maxwidth) { width = prefwidth; }
  else 
  { 
    if (maxwidth-1 > minwidth) { width = minwidth-1; }
    else { width = minwidth; }
  }
} 

void CursesWidget::placePopup(CursesWidget *popup, WINDOW *win) const
{
  int rows, cols, height, width, top, left;

  getmaxyx(stdscr, rows, cols);
  popupSize(height, width, popup);
  left = std::floor(double(cols)/2. - double(width)/2.);
  top = std::floor(double(rows)/2. - double(height)/2.);
  mvwin(win, top, left);
  wresize(win, height, width);
}

int CursesWidget::highlightedButton() const { return _highlighted_button; }

const std::string & CursesWidget::fgColor() const { return _fg_color; }
const std::string & CursesWidget::bgColor() const { return _bg_color; }

/*******************************************************************************

Hides a window by putting it at the center of the screen and giving it 0 size

*******************************************************************************/
void CursesWidget::hideWindow(WINDOW *win) const
{
  int rows, cols, left, top;

  getmaxyx(stdscr, rows, cols);
  left = std::floor(double(cols)/2.);
  top = std::floor(double(rows)/2.);
  mvwin(win, top, left);
  wresize(win, 0, 0);
}
