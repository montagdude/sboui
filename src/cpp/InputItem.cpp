#include <string>
#include <curses.h>
#include "InputItem.h"

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
