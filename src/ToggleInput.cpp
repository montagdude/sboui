#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "ToggleInput.h"

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

Draws input

*******************************************************************************/
void ToggleInput::draw(int y_offset, bool force, bool highlight)
{
  if (force) { _redraw_type = "all"; }

  if (highlight)
  {
    if (colors.turnOn(_win, color_settings.fg_highlight_active,
                            color_settings.bg_highlight_active) != 0)
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
std::string ToggleInput::exec(int y_offset)
{
  int ch;
  bool getting_input;
  std::string retval;

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
