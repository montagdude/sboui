#pragma once

#include <string>
#include <curses.h>

/*******************************************************************************

Box for user input

*******************************************************************************/
class InputBox {

  protected:

    WINDOW *_win;
    std::string _msg, _info, _redraw_type, _entry;
    unsigned int _firsttext, _cursidx;

    /* Prints to end of line, padding with spaces and avoiding borders */

    void printToEol(const std::string & msg) const;
    void printSpaces(unsigned int nspaces) const;

    /* Determines first character to print in input box */

    unsigned int determineFirstText();

    /* Drawing */
    
    virtual void redrawFrame() const;
    void redrawInput();

  public:

    /* Constructors */

    InputBox();
    InputBox(WINDOW *win, const std::string & msg);

    /* Set attributes */

    virtual void setWindow(WINDOW *win);
    void setMessage(const std::string & msg);
    void setInfo(const std::string & info);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Draws frame, entry, etc. as needed */

    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec();
};
