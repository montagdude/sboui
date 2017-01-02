#include <string>
#include <curses.h>
#include "Color.h"
#include "color_settings.h"
#include "signals.h"
#include "InputItem.h"
#include "TextInput.h"

using namespace color;

/*******************************************************************************

Prints to end of line, padding with spaces

*******************************************************************************/
void TextInput::printToEol(const std::string & msg) const
{
  int i, y, x, rows, cols, nspaces, msglen;

  getmaxyx(_win, rows, cols);
  getyx(_win, y, x);

  /* Math: Cursor position: x
           Number of spaces that can be printed to right = _width-x */

  msglen = msg.size();
  if (msglen > _width-x) { wprintw(_win, msg.substr(0, _width-x).c_str()); }
  else
  {
    nspaces = std::max(_width-x-msglen, 0);
    wprintw(_win, msg.c_str());
    for ( i = 0; i < nspaces; i++ ) { wprintw(_win, " "); }
  }
}

/*******************************************************************************

Prints a given number of spaces

*******************************************************************************/
void TextInput::printSpaces(unsigned int nspaces) const
{
  unsigned int i;

  for ( i = 0; i < nspaces; i++ ) { waddch(_win, ' '); }
}

/*******************************************************************************

Determine first character to print in text entry. Returns 0 if it has not
changed; 1 if it has.

*******************************************************************************/
unsigned int TextInput::determineFirstText()
{
  unsigned int firsttextstore;

  firsttextstore = _firsttext;
  if (int(_entry.size()) < _width) { _firsttext = 0; }
  else 
  { 
    if (int(_cursidx) - int(_firsttext) > _width-1)
    {
      _firsttext = _cursidx - _width + 1;
    }
  }

  if (firsttextstore == _firsttext) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Redraws user input

*******************************************************************************/
void TextInput::redrawEntry() const
{
  int pair_input, numprint;

  numprint = std::min(_width-1, int(_entry.size() - _firsttext));
  
  wmove(_win, _posy, _posx);
  pair_input = colors.pair(fg_highlight_active, bg_highlight_active);
  if (pair_input != -1) { wattron(_win, COLOR_PAIR(pair_input)); }
  printToEol(_entry.substr(_firsttext, numprint));
  if (pair_input != -1) { wattroff(_win, COLOR_PAIR(pair_input)); }
}

/*******************************************************************************

Constructor

*******************************************************************************/
TextInput::TextInput()
{
  _entry = "";
  _firsttext = 0;
  _cursidx = 0;
}

/*******************************************************************************

Draws text input

*******************************************************************************/
void TextInput::draw(bool force)
{
  if (force) { _redraw_type = "entry"; }

  if (_redraw_type == "entry") { redrawEntry(); }
  wmove(_win, _posy, _cursidx - _firsttext + 1);
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke or entry

*******************************************************************************/
std::string TextInput::exec(bool highlight)
{
  int ch;
  bool getting_input;
  std::string retval;
  unsigned int check_redraw;

  const int MY_DELETE = 330;
  const int MY_ESC = 27;

  curs_set(1);
  determineFirstText();

  getting_input = true;
  while (getting_input)
  {
    // Redraw
  
    draw();
    _redraw_type = "entry";

    // Get user input

    switch (ch = getch()) {

      // Enter key: return entry

      case '\n':
      case '\r':
      case KEY_ENTER: 
        retval = _entry;
        _redraw_type = "entry";
        getting_input = false;
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
        if (_cursidx > 0) { _cursidx--; }
        if (_cursidx < _firsttext) { _firsttext = _cursidx; }
        else { _redraw_type = "none"; }
        break;
      case KEY_RIGHT:
        if (_cursidx < _entry.size()) { _cursidx++; }
        check_redraw = determineFirstText();
        if (check_redraw == 0) { _redraw_type = "none"; }
        break;
      case KEY_HOME:
        if (_cursidx == 0) { _redraw_type = "none"; }
        _cursidx = 0;
        _firsttext = 0;
        break;
      case KEY_END:
        _cursidx = _entry.size();
        check_redraw = determineFirstText();
        if (check_redraw == 0) { _redraw_type = "none"; }
        break;

      // Resize signal
    
      case KEY_RESIZE:
        retval = signals::resizeSignal;
        _redraw_type = "entry";
        getting_input = false;
        break;

      // Quit key

      case MY_ESC:
        retval = signals::quitSignal;
        _redraw_type = "entry";
        getting_input = false;
        break;

      // Add character to entry

      default:
        _entry.insert(_cursidx, 1, ch);
        _cursidx++;
        determineFirstText();
        break;
    }
  }

  curs_set(0);
  return retval;
}

/*******************************************************************************

Accessing properties

*******************************************************************************/
std::string TextInput::getStringProp() const { return _entry; }
