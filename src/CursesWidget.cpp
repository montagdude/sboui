#include <curses.h>
#include "CursesWidget.h"

/*******************************************************************************

Prints to end of line, padding with spaces and avoiding borders.
printable: number of spaces that can be printed on, including the current column
  and any to the right. Defaults to window width - x - 1 (space for border).

*******************************************************************************/
void CursesWidget::printToEol(const std::string & msg, int printable) const
{
  int i, y, x, rows, cols, nspaces, msglen;

  getmaxyx(_win, rows, cols);
  getyx(_win, y, x);

  /* Math: Number of columns: cols
           Cursor position: x
           Number of spaces that can be printed to right = printable-msglen
           Default printable: cols-x-1
           => Because the last column is taken up by the border */

  msglen = msg.size();
  if (printable == -1) { printable = cols-x-1; }
  if (msglen > printable)
    wprintw(_win, msg.substr(0, printable).c_str());
  else
  {
    nspaces = std::max(printable-msglen, 0);
    wprintw(_win, msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { wprintw(_win, " "); }
  }
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

Constructor

*******************************************************************************/
CursesWidget::CursesWidget() { _win = NULL; }

/*******************************************************************************

Set attributes

*******************************************************************************/
void CursesWidget::setWindow(WINDOW *win) { _win = win; }
