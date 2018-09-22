#include <string>
#include <curses.h>
#include "settings.h"
#include "signals.h"
#include "TextInput.h"
#include "QuickSearch.h"
#include "MouseEvent.h"

/*******************************************************************************

User interaction: returns key stroke. Unlike regular TextInput, returns whenever
the entry is changed, because the calling function needs to search as the user
types.

*******************************************************************************/
std::string QuickSearch::exec(int y_offset, MouseEvent * mevent)
{
  int ch;
  bool getting_input;
  std::string retval;
  unsigned int check_redraw;

  const int MY_DELETE = 330;
  const int MY_ESC = 27;
  const int MY_TAB = 9;
  const int MY_SHIFT_TAB = 353;

  curs_set(1);

  getting_input = true;
  while (getting_input)
  {
    // Redraw (do not highlight, like a regular TextInput)
  
    draw(y_offset, false, false);
    _redraw_type = "entry";
    getting_input = false;

    // Get user input

    switch (ch = getch()) {

      // Enter key: return Enter signal

      case '\n':
      case '\r':
      case KEY_ENTER: 
        retval = signals::keyEnter;
        _redraw_type = "entry";
        break;

      // Backspace key pressed: delete previous character.
      // Note: some terminals define it as 8, others as 127

      case 8:
      case 127:
      case KEY_BACKSPACE:
        if (_cursidx > 0)
        {
          _entry.erase(_cursidx-1,1);
          _cursidx--;
          if (_cursidx < _firsttext)
          {
            if (int(_cursidx) > _width-1) { _firsttext = _cursidx-_width+1; }
            else { _firsttext = 0; }
          }
        }
        break;

      // Delete key pressed: delete current character

      case MY_DELETE:
        if (_cursidx < _entry.size()) { _entry.erase(_cursidx,1); }
        break;      

      // Navigation keys

      case KEY_LEFT:
        getting_input = true;
        if (_cursidx > 0) { _cursidx--; }
        if (_cursidx < _firsttext) { _firsttext = _cursidx; }
        else { _redraw_type = "none"; }
        break;
      case KEY_RIGHT:
        getting_input = true;
        if (_cursidx < _entry.size()) { _cursidx++; }
        check_redraw = determineFirstText();
        if (check_redraw == 0) { _redraw_type = "none"; }
        break;
      case KEY_HOME:
        getting_input = true;
        if (_cursidx == 0) { _redraw_type = "none"; }
        _cursidx = 0;
        _firsttext = 0;
        break;
      case KEY_END:
        getting_input = true;
        _cursidx = _entry.size();
        check_redraw = determineFirstText();
        if (check_redraw == 0) { _redraw_type = "none"; }
        break;
      case KEY_PPAGE:
        retval = "ignore";
        _redraw_type = "entry";
        break;
      case KEY_NPAGE:
        retval = "ignore";
        _redraw_type = "entry";
        break;
      case KEY_UP:
      case MY_SHIFT_TAB:
        retval = signals::highlightPrev;
        _redraw_type = "entry";
        break;
      case KEY_DOWN:
      case MY_TAB:
        retval = signals::highlightNext;
        _redraw_type = "entry";
        break;

      // Resize signal
    
      case KEY_RESIZE:
        retval = signals::resize;
        _redraw_type = "entry";
        break;

      // Quit key

      case MY_ESC:
        retval = signals::quit;
        _redraw_type = "entry";
        break;

      // Add character to entry

      default:
        _entry.insert(_cursidx, 1, ch);
        _cursidx++;
        determineFirstText();
        break;
    }
  }

  // Redraw when exiting, because entry may have changed
  
  draw(y_offset, false, false);

  curs_set(0);
  return retval;
}
