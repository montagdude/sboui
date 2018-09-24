#pragma once

#include <string>
#include <curses.h>
#include "MouseEvent.h"

/*******************************************************************************

Abstract class for CursesWidgets. Derived classes must implement sizing rules,
draw, and exec methods.

*******************************************************************************/
class CursesWidget {

  protected:

    void printToEol(const std::string & msg, int printable_cols=-1) const;
    void printSpaces(int nspaces) const;
    void clearWindow () const;

    WINDOW *_win;

  public:

    /* Constructor */

    CursesWidget();

    /* Set attributes */

    virtual void setWindow(WINDOW *win);

    /* Sets size and position of popup boxes */

    virtual void popupSize(int & height, int & width, CursesWidget *popup) const;
    virtual void placePopup(CursesWidget *popup, WINDOW *win) const;

    /* Hides a popup window by putting it at the center with 0 size */

    virtual void hideWindow(WINDOW *win) const;

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const = 0;
    virtual void preferredSize(int & height, int & width) const = 0;

    /* Mouse interaction */

    virtual std::string handleMouseEvent(MouseEvent * mevent) = 0;

    /* Draws frame and message */

    virtual void draw(bool force=false) = 0;

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL) = 0;
};
