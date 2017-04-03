#pragma once

#include <string>
#include <curses.h>

/*******************************************************************************

Abstract class for CursesWidgets. Derived classes must implement sizing rules,
draw, and exec methods.

*******************************************************************************/
class CursesWidget {

  protected:

    void printToEol(const std::string & msg, int printable_cols=-1) const;
    void printSpaces(int nspaces) const;

    WINDOW *_win;

  public:

    /* Constructor */

    CursesWidget();

    /* Set attributes */

    virtual void setWindow(WINDOW *win);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const = 0;
    virtual void preferredSize(int & height, int & width) const = 0;

    /* Draws frame and message */
 
    virtual void draw(bool force=false) = 0;

    /* User interaction loop */

    virtual std::string exec() = 0;
};
