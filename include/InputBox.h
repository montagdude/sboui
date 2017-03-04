#pragma once

#include <string>
#include <vector>
#include <curses.h>
#include "CursesWidget.h"

class InputItem;

/*******************************************************************************

Box containing a number of InputItems

*******************************************************************************/
class InputBox: public CursesWidget {

  protected:

    std::string _msg, _info, _redraw_type;
    std::vector<InputItem *> _items;    
    unsigned int _highlight, _prevhighlight, _firstprint, _reserved_rows;

    /* Setting item to be highlighted */

    void highlightFirst();
    void highlightLast();
    void highlightPrevious();
    void highlightNext();

    /* Drawing */
    
    virtual void redrawFrame() const;

  public:

    /* Constructors */

    InputBox();
    InputBox(WINDOW *win, const std::string & msg);

    /* Add items */

    void addItem(InputItem *item);

    /* Set attributes */

    void setWindow(WINDOW *win);
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
