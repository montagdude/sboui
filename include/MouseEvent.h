#pragma once

#include <curses.h>
#include <chrono>

/*******************************************************************************

Mouse event class. Keeps track of button presses and determines when double
clicks occur.

*******************************************************************************/
class MouseEvent {

  protected:

    std::chrono::high_resolution_clock::time_point _tclick;
    int _button;
    int _x, _y;
    bool _doubleclick;
    const static int _doubleclick_timeout;

  public:

    /* Constructor */

    MouseEvent();

    /* Record click information */

    void recordClick(const MEVENT & event);

    /* Query click information */

    int button() const;
    int x() const;
    int y() const;
    bool doubleClick() const;
};
