#include <string>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

Prints to end of line, padding with spaces

*******************************************************************************/
void InputItem::printToEol(const std::string & msg) const
{
  int i, y, x, rows, cols, rightspace, nspaces, msglen;

  getmaxyx(_win, rows, cols);
  getyx(_win, y, x);

  /* Math: Cursor position: x
           Number of spaces that can be printed to right = _width-(x-_posx) */

  msglen = msg.size();
  rightspace = _width - (x - _posx);
  if (msglen > rightspace) { wprintw(_win, msg.substr(0, rightspace).c_str()); }
  else
  {
    nspaces = std::max(rightspace-msglen, 0);
    wprintw(_win, msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { wprintw(_win, " "); }
  }
}

/*******************************************************************************

Constructor

*******************************************************************************/
InputItem::InputItem()
{
  _win = NULL;
  _posx = 0;
  _posy = 0;
  _width = 0;
  _name = "";
  _redraw_type = "none";
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void InputItem::setWindow(WINDOW *win) { _win = win; }
void InputItem::setName(const std::string & name) { _name = name; }
void InputItem::setWidth(int width) { _width = width; }
void InputItem::setPosition(int x, int y)
{
  _posx = x;
  _posy = y;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
const std::string & InputItem::name() const { return _name; }

/*******************************************************************************

Accessing properties of different types. Derived classes should reimplement 
these as needed.

*******************************************************************************/
std::string InputItem::getStringProp() const { return ""; }
bool InputItem::getBoolProp() const { return false; }
int InputItem::getIntProp() const { return 0; }
double InputItem::getDoubleProp() const { return 0.; }
