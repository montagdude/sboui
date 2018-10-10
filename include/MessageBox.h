#pragma once

#include <string>
#include <curses.h>
#include "CursesWidget.h"
#include "MouseEvent.h"

/*******************************************************************************

Header for message box class

*******************************************************************************/
class MessageBox: public CursesWidget {

  protected:

    std::string _name, _message;
    unsigned int _margin_v, _margin_h;
    bool _header_colorize, _centered;

    void redrawFrame();
    virtual void redrawMessage() const;

  public:

    /* Constructors */

    MessageBox(bool header_colorize=true, bool centered=true);
    MessageBox(WINDOW *win, const std::string & name,
               bool header_colorize=true, bool centered=true);

    /* Set attributes */

    void setName(const std::string & name);
    void setMessage(const std::string & msg);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Mouse interaction */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* Draws frame and message */

    virtual void draw(bool force=false);

    /* User interaction loop */

    std::string exec(MouseEvent * mevent=NULL);
};
