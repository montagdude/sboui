#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "ToggleInput.h"

using namespace color;

/*******************************************************************************

Drawing

*******************************************************************************/
void ToggleInput::redrawEntry() const
{
  wmove(_win, _posy, _posx);
  if (_enabled) { wprintw(_win, "[X] "); }
  else { wprintw(_win, "[ ] "); }
}

void ToggleInput::redrawText() const
{
  unsigned int nspaces;

  wmove(_win, _posy, _posx+4);
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
void ToggleInput::draw(bool force, bool highlight)
{
  int color_pair;

  if (force) { _redraw_type = "all"; }

  // Set color

  if (highlight)
  {
    color_pair = colors.pair(fg_highlight_active, bg_highlight_active);
    if (color_pair != -1) { wattron(_win, COLOR_PAIR(color_pair)); }
    else { wattron(_win, A_REVERSE); }
  }

  // Draw elements

  if ( (_redraw_type == "all") || (_redraw_type == "entry") )
  {
    redrawEntry();
  } 
  if (_redraw_type == "all") { redrawText(); }
  wmove(_win, _posy, _posx+1);

  // Turn off color and refresh

  if (highlight)
  {
    if (color_pair != -1) { wattroff(_win, COLOR_PAIR(color_pair)); }
    else { wattroff(_win, A_REVERSE); }
  }
  wrefresh(_win);
}

/*******************************************************************************

User interaction allows user to toggle the item

*******************************************************************************/
std::string ToggleInput::exec()
{
  int ch;
  bool getting_input;
  std::string retval;

  const int MY_ESC = 27;

  curs_set(1);

  getting_input = true;
  while (getting_input)
  {
    // Redraw
  
    draw(false, true);
    _redraw_type = "entry";

    // Get user input

    switch (ch = getch()) {

      // Enter key: return name of this item

      case '\n':
      case '\r':
      case KEY_ENTER: 
        retval = _name;
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
      case KEY_UP:
        retval = signals::highlightPrev;
        _redraw_type = "all";
        getting_input = false;
        break;
      case KEY_DOWN:
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
    }
  }

  curs_set(0);
  return retval;
}

/*******************************************************************************

Accessing properties

*******************************************************************************/
bool ToggleInput::getBoolProp() const { return _enabled; }
