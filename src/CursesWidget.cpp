#include <curses.h>
#include <cmath>     // floor
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

Constructor

*******************************************************************************/
CursesWidget::CursesWidget() { _win = NULL; }

/*******************************************************************************

Set attributes

*******************************************************************************/
void CursesWidget::setWindow(WINDOW *win) { _win = win; }

/*******************************************************************************

Sets size and position of popup boxes

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
