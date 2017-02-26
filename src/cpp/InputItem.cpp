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

Prints a given number of spaces

*******************************************************************************/
void InputItem::printSpaces(unsigned int nspaces) const
{
  unsigned int i;

  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
}

/*******************************************************************************

Constructor and destructor. For use of virtual destructor, see:
http://stackoverflow.com/questions/461203/when-to-use-virtual-destructors#461224

*******************************************************************************/
InputItem::InputItem()
{
  _win = NULL;
  _posx = 0;
  _posy = 0;
  _width = 0;
  _name = "";
  _redraw_type = "none";
  _selectable = true;
  _auto_position = true;
}

InputItem::~InputItem() {}

/*******************************************************************************

Set attributes

*******************************************************************************/
void InputItem::setWindow(WINDOW *win) { _win = win; }
void InputItem::setName(const std::string & name) { _name = name; }
void InputItem::setWidth(int width) { _width = width; }
void InputItem::setPosition(int y, int x)
{
  _posy = y;
  _posx = x;
}

void InputItem::setAutoPosition(bool auto_position)
{ 
  _auto_position = auto_position;
}

/*******************************************************************************

Get attributes

*******************************************************************************/
const std::string & InputItem::name() const { return _name; }
int InputItem::posx() const { return _posx; }
int InputItem::posy() const { return _posy; }
int InputItem::width() const { return _width; }
bool InputItem::selectable() const { return _selectable; }
bool InputItem::autoPosition() const { return _auto_position; }

/*******************************************************************************

User interaction. The base class version just returns an empty string, so that
non-selectable InputItems (e.g., labels) don't have to implement it. Selectable
classes should reimplement this.

*******************************************************************************/
std::string InputItem::exec() { return ""; }

/*******************************************************************************

Accessing properties of different types. Derived classes should reimplement 
these as needed.

*******************************************************************************/
std::string InputItem::getStringProp() const { return ""; }
bool InputItem::getBoolProp() const { return false; }
int InputItem::getIntProp() const { return 0; }
double InputItem::getDoubleProp() const { return 0.; }
