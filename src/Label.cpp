#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "Label.h"
#include "MouseEvent.h"

/*******************************************************************************

Constructors and destructor

*******************************************************************************/
Label::Label()
{
  _redraw_type = "label";
  _item_type = "Label";
  _color_idx = -1;
  _selectable = false;
  _hline = false;
}

Label::Label(bool selectable)
{
  _redraw_type = "label";
  _item_type = "Label";
  _color_idx = -1;
  _selectable = selectable;
  _hline = false;
}

Label::Label(bool selectable, bool hline)
{
  _redraw_type = "label";
  _item_type = "Label";
  _color_idx = -1;
  _selectable = selectable;
  _hline = hline;
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void Label::setColor(int color_idx) { _color_idx = color_idx; }
void Label::setSelectable(bool selectable) { _selectable = selectable; }
void Label::setHLine(bool hline) { _hline = hline; }

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string Label::handleMouseEvent(MouseEvent * mevent, int y_offset)
{
  int begy, begx, ycurs, xcurs;

  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;
  xcurs = mevent->x() - begx;

  if ( (mevent->button() == 1) || (mevent->button() == 3) )
  {
    // Check for clicking in the Label

    if ( (xcurs >= _posx) && (xcurs < _posx+_width) &&
         (ycurs == _posx-y_offset) )
    {
      if (_selectable)
      {
        if (mevent->doubleClick())
          return signals::keyEnter;
        else
          return signals::nullEvent;  // Because the event was handled here
      }
    }
  }

  return signals::mouseEvent;         // Defer to InputBox to handle event
}

/*******************************************************************************

Draws label

*******************************************************************************/
void Label::draw(int y_offset, bool force, bool highlight)
{
  int i;

  wmove(_win, _posy-y_offset, _posx);

  if (_redraw_type != "label") { return; }

  if (highlight && _selectable)
  {
    if (colors.turnOn(_win, "fg_highlight_active", "bg_highlight_active") != 0)
      wattron(_win, A_REVERSE);
  }
  else { colors.turnOn(_win, _color_idx); }

  if (_hline)
    for ( i = 0; i < _width; i++ ) { waddch(_win, ACS_HLINE); }
  else
    printToEol(_name);

  if (colors.turnOff(_win) != 0)
  {
    if (highlight && _selectable) { wattroff(_win, A_REVERSE); }
  }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke

*******************************************************************************/
std::string Label::exec(int y_offset, MouseEvent * mevent)
{
  int ch;
  std::string retval;
  MEVENT event;

  const int MY_ESC = 27;
  const int MY_TAB = 9;
  const int MY_SHIFT_TAB = 353;

  // Redraw

  draw(y_offset, false, true);

  // Get user input

  switch (ch = getch()) {

    // Enter key: return Enter signal

    case '\n':
    case '\r':
    case KEY_ENTER:
      retval = signals::keyEnter;
      break;

    // Space: activate item

    case ' ':
      retval = signals::keySpace;
      break;

    // Navigation keys

    case KEY_HOME:
      retval = signals::highlightFirst;
      break;
    case KEY_END:
      retval = signals::highlightLast;
      break;
    case KEY_PPAGE:
      retval = signals::highlightPrevPage;
      break;
    case KEY_NPAGE:
      retval = signals::highlightNextPage;
      break;
    case KEY_UP:
    case MY_SHIFT_TAB:
      retval = signals::highlightPrev;
      break;
    case KEY_DOWN:
    case MY_TAB:
      retval = signals::highlightNext;
      break;

    // Resize signal

    case KEY_RESIZE:
      retval = signals::resize;
      break;

    // Quit key

    case MY_ESC:
      retval = signals::quit;
      break;

    // Mouse

    case KEY_MOUSE:
      if ( (getmouse(&event) == OK) && mevent )
      {
        mevent->recordClick(event);
        retval = handleMouseEvent(mevent, y_offset);
      }
      break;

    default:
      retval = char(ch);
      break;
  }

  return retval;
}
