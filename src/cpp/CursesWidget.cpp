#include <curses.h>
#include "CursesWidget.h"

/*******************************************************************************

Prints to end of line, padding with spaces and avoiding borders

*******************************************************************************/
void CursesWidget::printToEol(const std::string & msg) const
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
