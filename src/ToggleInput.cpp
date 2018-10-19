#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "ToggleInput.h"
#include "MouseEvent.h"

/*******************************************************************************

Drawing

*******************************************************************************/
void ToggleInput::redrawEntry(int y_offset) const
{
  wmove(_win, _posy-y_offset, _posx);
  if (_enabled) { wprintw(_win, "[X] "); }
  else { wprintw(_win, "[ ] "); }
}

void ToggleInput::redrawText(int y_offset) const
{
  int nspaces;

  wmove(_win, _posy-y_offset, _posx+4);
  if (_width > int(_name.size())+4) { nspaces = _width - (_name.size()+4); }
  else { nspaces = 0; } 
  wprintw(_win, _name.c_str());
  printSpaces(nspaces);
}

/*******************************************************************************

Constructor

*******************************************************************************/
ToggleInput::ToggleInput()
{
  _redraw_type = "all";
  _item_type = "ToggleInput";
  _selectable = true;
  _name = "ToggleInput";
  _enabled = false;
}

/*******************************************************************************

Setting properties

*******************************************************************************/
void ToggleInput::setEnabled(bool enabled) { _enabled = enabled; }
void ToggleInput::toggle() { _enabled = !_enabled; }

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string ToggleInput::handleMouseEvent(MouseEvent * mevent, int y_offset)
{
  int begy, begx, ycurs, xcurs;

  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;
  xcurs = mevent->x() - begx;

  if ( (mevent->button() == 1) || (mevent->button() == 3) )
  {
    // Check for clicking in the ToggleInput

    if ( (xcurs >= _posx) && (xcurs < _posx+_width) &&
         (ycurs == _posy-y_offset) )
    {
      // Check for clicking in or on the check box

      if ( (xcurs-_posx >= 0) && (xcurs-_posx <= 2) )
        toggle();
      return signals::nullEvent;  // Because the event was handled here
    }
  }

  return signals::mouseEvent;     // Defer to InputBox to handle event
}

/*******************************************************************************

Draws input

*******************************************************************************/
void ToggleInput::draw(int y_offset, bool force, bool highlight)
{
  if (force) { _redraw_type = "all"; }

  if (highlight)
  {
    if (colors.turnOn(_win, "fg_highlight_active", "bg_highlight_active") != 0)
      wattron(_win, A_REVERSE);
  }

  if ( (_redraw_type == "all") || (_redraw_type == "entry") )
    redrawEntry(y_offset);
  if (_redraw_type == "all") { redrawText(y_offset); }
  wmove(_win, _posy-y_offset, _posx+1);

  if (highlight)
    if (colors.turnOff(_win) != 0) { wattroff(_win, A_REVERSE); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction allows user to toggle the item

*******************************************************************************/
std::string ToggleInput::exec(int y_offset, MouseEvent * mevent)
{
  int ch;
  bool getting_input;
  std::string retval;
  MEVENT event;

  const int MY_ESC = 27;
  const int MY_TAB = 9;
  const int MY_SHIFT_TAB = 353;

  curs_set(1);

  getting_input = true;
  while (getting_input)
  {
    // Redraw
  
    draw(y_offset, false, true);
    _redraw_type = "entry";

    // Get user input

    switch (ch = getch()) {

      // Enter key: return enter signal

      case '\n':
      case '\r':
      case KEY_ENTER: 
        retval = signals::keyEnter;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Space: toggle item

      case ' ':
        toggle();
        _redraw_type = "entry";
        break;

      // Navigation keys

      case KEY_HOME:
        retval = signals::highlightFirst;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_END:
        retval = signals::highlightLast;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_PPAGE:
        retval = signals::highlightPrevPage;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_NPAGE:
        retval = signals::highlightNextPage;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_UP:
      case MY_SHIFT_TAB:
        retval = signals::highlightPrev;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_DOWN:
      case MY_TAB:
        retval = signals::highlightNext;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_RIGHT:
        retval = signals::keyRight;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_LEFT:
        retval = signals::keyLeft;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Resize signal
    
      case KEY_RESIZE:
        retval = signals::resize;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Quit key

      case MY_ESC:
        retval = signals::quit;
        _redraw_type = "all";
        getting_input = false;
        break;

      // Mouse

      case KEY_MOUSE:
        if ( (getmouse(&event) == OK) && mevent )
        {
          mevent->recordClick(event);
          retval = handleMouseEvent(mevent, y_offset);
          if (retval == signals::mouseEvent)
          {
            _redraw_type = "all";
            getting_input = false;
          }
          else
            _redraw_type = "entry";
        }
        break;

      default:
        retval = char(ch);
        _redraw_type = "all";
        getting_input = false;
        break;
    }
  }

  curs_set(0);
  return retval;
}

/*******************************************************************************

Accessing properties

*******************************************************************************/
bool ToggleInput::getBoolProp() const { return _enabled; }
bool ToggleInput::enabled() const { return _enabled; }
