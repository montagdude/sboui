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
  else
    return;
  //FIXME: Button 4 supposedly reports scroll down events, but scroll up
  //(button 5) is only available in ncurses 6. Add scroll wheel support once
  //Slackware 15 is released.

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
