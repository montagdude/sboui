#include <curses.h>
#include <chrono>
#include "MouseEvent.h"

const int MouseEvent::_doubleclick_timeout = 250;   // Milliseconds. Longer than
                                                    // this between presses is
                                                    // considered a single click

/*******************************************************************************

Constructor

*******************************************************************************/
MouseEvent::MouseEvent()
{
  _tclick = std::chrono::high_resolution_clock::now();
  _button = -1;
  _doubleclick = false;
}

/*******************************************************************************

Records mouse press information and determines whether double click occurred

*******************************************************************************/
void MouseEvent::recordClick(const MEVENT & event)
{
  std::chrono::high_resolution_clock::time_point tnew;
  int newbutton;
  int duration;

  tnew = std::chrono::high_resolution_clock::now();

  if (event.bstate & BUTTON1_PRESSED)
    newbutton = 1;
  else if (event.bstate & BUTTON2_PRESSED)
    newbutton = 2; 
  else if (event.bstate & BUTTON4_PRESSED)
    newbutton = 4;
#if NCURSES_MOUSE_VERSION > 1
  else if (event.bstate & BUTTON5_PRESSED)
#else
  // Note: this may not give desired scroll-down behavior with some values of
  // TERM. For example, xterm-1003 activates this bit for all mouse movements.
  else if (event.bstate & REPORT_MOUSE_POSITION)
#endif
    newbutton = 5;
  else
    return;

  // Determine if a double click occurred

  _doubleclick = false;
  if ( (newbutton == 1) && (_button == 1) )
  {
    duration = std::chrono::duration_cast<std::chrono::milliseconds>
               (tnew-_tclick).count();
    if (duration <= MouseEvent::_doubleclick_timeout)
      _doubleclick = true;
  }

  // Store the rest of the event information

  _tclick = tnew;
  _button = newbutton;
  _x = event.x;
  _y = event.y;
}

/*******************************************************************************

Query event information

*******************************************************************************/
int MouseEvent::button() const { return _button; }
int MouseEvent::x() const { return _x; }
int MouseEvent::y() const { return _y; }
bool MouseEvent::doubleClick() const { return _doubleclick; }
