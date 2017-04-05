#pragma once

#include <string>
#include <curses.h>
#include "CursesWidget.h"

/*******************************************************************************

Header for message box class

*******************************************************************************/
class MessageBox: public CursesWidget {

  protected:

    std::string _name, _message, _info;
    unsigned int _margin_v, _margin_h;
    int _color_idx;
    bool _header_colorize;

    void redrawFrame() const;
    void redrawMessage() const;

  public:

    /* Constructors */

    MessageBox(bool header_colorize=true);
    MessageBox(WINDOW *win, const std::string & name,
               bool header_colorize=true);

    /* Set attributes */

    void setName(const std::string & name);
    void setMessage(const std::string & msg);
    void setInfo(const std::string & info); 
    void setColor(int color_idx);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* Draws frame and message */
 
    virtual void draw(bool force=false);

    /* User interaction loop */

    std::string exec();
};
