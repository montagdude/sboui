#include <string>
#include <curses.h>
#include "InputItem.h"
#include "Label.h"

/*******************************************************************************

Constructor

*******************************************************************************/
Label::Label()
{
  _redraw_type = "label";
  _selectable = false;
  _color_pair = -1;
  _bold = false;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void Label::setColor(int color_pair) { _color_pair = color_pair; }
void Label::setBold(bool bold) { _bold = bold; }

/*******************************************************************************

Draws label

*******************************************************************************/
void Label::draw(bool force, bool highlight)
{
  wmove(_win, _posy, _posx);

  if (_color_pair != -1) { wattron(_win, COLOR_PAIR(_color_pair)); }
  if (_bold) { wattron(_win, A_BOLD); }
  wprintw(_win, _name.c_str());
  if (_color_pair != -1) { wattroff(_win, COLOR_PAIR(_color_pair)); }
  if (_bold) { wattroff(_win, A_BOLD); }

  wrefresh(_win);
}
