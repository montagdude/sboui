#include <string>
#include <curses.h>
#include "Color.h"
#include "settings.h"
#include "signals.h"
#include "InputItem.h"
#include "TextInput.h"
#include "MouseEvent.h"

/*******************************************************************************

Determine first character to print in text entry. Returns 0 if it has not
changed; 1 if it has.

*******************************************************************************/
unsigned int TextInput::determineFirstText()
{
  unsigned int firsttextstore;

  firsttextstore = _firsttext;
  if (int(_entry.size()+_labellen) < _width) { _firsttext = 0; }
  else 
  { 
    if (int(_cursidx) - int(_firsttext) > _width-1-int(_labellen))
    {
      _firsttext = _cursidx - _width + _labellen + 1;
    }
  }

  // Just in case _firsttext goes too far (i.e., if _width hasn't been set yet)

  if (_firsttext >= _entry.size()) { _firsttext = 0; }

  if (firsttextstore == _firsttext) { return 0; }
  else { return 1; }
}

/*******************************************************************************

Redraws user input

*******************************************************************************/
void TextInput::redrawEntry(int y_offset) const
{
  int numprint, scroll_offset;

  if (_cursidx == _entry.size()) { scroll_offset = 1; }
  else { scroll_offset = 0; }

  numprint = std::min(_width - scroll_offset - int(_labellen), 
                      int( _entry.size()) - int(_firsttext) + int(_labellen));
  
  wmove(_win, _posy-y_offset, _posx);
  printToEol(_label + _entry.substr(_firsttext, numprint));
}

/*******************************************************************************

Constructor

*******************************************************************************/
TextInput::TextInput()
{
  _redraw_type = "entry";
  _item_type = "TextInput";
  _selectable = true;
  _entry = "";
  _label = "";
  _labellen = 0;
  _firsttext = 0;
  _cursidx = 0;
}

/*******************************************************************************

Set properties

*******************************************************************************/
void TextInput::setText(const std::string & text)
{
  _entry = text;
  _cursidx = _entry.size();
  determineFirstText();
}

void TextInput::clear()
{ 
  _entry = "";
  _firsttext = 0;
  _cursidx = 0;
}

void TextInput::setLabel(const std::string & label)
{
  _label = label;
  _labellen = _label.size();
}

void TextInput::removeLabel()
{
  _label = "";
  _labellen = 0;
}

/*******************************************************************************

Handles mouse event

*******************************************************************************/
std::string TextInput::handleMouseEvent(MouseEvent * mevent, int y_offset)
{
  int begy, begx, ycurs, xcurs, xinpbeg, xinpend;

  getbegyx(_win, begy, begx);
  ycurs = mevent->y() - begy;
  xcurs = mevent->x() - begx;

  if ( (mevent->button() == 1) || (mevent->button() == 3) )
  {
    // Check for clicking in the TextInput

    if ( (xcurs >= _posx) && (xcurs < _posx+_width) &&
         (ycurs == _posy-y_offset) )
    {
      xinpbeg = _posx + _labellen;
      xinpend = xinpbeg + _entry.size() - _firsttext;
      if (xcurs < xinpbeg)
        _cursidx = _firsttext;
      else if (xcurs >= xinpend)
        _cursidx = _entry.size();
      else
        _cursidx = xcurs-1+_firsttext-_labellen;
      _redraw_type = "entry";
      return signals::nullEvent;  // Because the event was handled here
    }
  }

  return signals::mouseEvent;     // Defer to InputBox to handle event
}

/*******************************************************************************

Draws text input

*******************************************************************************/
void TextInput::draw(int y_offset, bool force, bool highlight)
{
  if (force) { _redraw_type = "entry"; }

  if (highlight)
  {
    if (colors.turnOn(_win, "fg_highlight_active", "bg_highlight_active") != 0)
      wattron(_win, A_REVERSE);
  }

  if (_redraw_type == "entry") { redrawEntry(y_offset); }
  wmove(_win, _posy-y_offset, _posx + _cursidx + _labellen - _firsttext);

  if (highlight)
    if (colors.turnOff(_win) != 0) { wattroff(_win, A_REVERSE); }
  wrefresh(_win);
}

/*******************************************************************************

User interaction: returns key stroke

*******************************************************************************/
std::string TextInput::exec(int y_offset, MouseEvent * mevent)
{
  int ch;
  bool getting_input;
  std::string retval;
  unsigned int check_redraw;
  MEVENT event;

  const int MY_DELETE = 330;
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

      // Enter key: return Enter signal

      case '\n':
      case '\r':
      case KEY_ENTER: 
        retval = signals::keyEnter;
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
            if (int(_cursidx) > _width-int(_labellen)-1)
              _firsttext = _cursidx-_width+1;
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
      case KEY_PPAGE:
        retval = signals::highlightPrevPage;
        _redraw_type = "entry";
        getting_input = false;
        break;
      case KEY_NPAGE:
        retval = signals::highlightNextPage;
        _redraw_type = "entry";
        getting_input = false;
        break;
      case KEY_UP:
      case MY_SHIFT_TAB:
        retval = signals::highlightPrev;
        _redraw_type = "entry";
        getting_input = false;
        break;
      case KEY_DOWN:
      case MY_TAB:
        retval = signals::highlightNext;
        _redraw_type = "entry";
        getting_input = false;
        break;

      // Resize signal
    
      case KEY_RESIZE:
        retval = signals::resize;
        _redraw_type = "entry";
        getting_input = false;
        break;

      // Quit key

      case MY_ESC:
        retval = signals::quit;
        _redraw_type = "entry";
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
            _redraw_type = "entry";
            getting_input = false;
          }
        }
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
std::string TextInput::text() const { return _entry; }
