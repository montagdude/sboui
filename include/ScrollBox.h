#pragma once

#include <string>
#include <curses.h>
#include "AbstractListBox.h"

/*******************************************************************************

Same idea as a ListBox, but items are not selectable/highlightable

*******************************************************************************/
class ScrollBox: public AbstractListBox {

  protected:

    /* Scrolling up/down */

    int scrollFirst();
    int scrollLast();
    int scrollUp();
    int scrollDown();
    int scrollPreviousPage();
    int scrollNextPage();

    /* Drawing */
    
    virtual void redrawFrame() const;
    virtual void redrawSingleItem(unsigned int idx);
    void redrawAllItems();

  public:

    /* Constructors */

    ScrollBox();
    ScrollBox(WINDOW *win, const std::string & name);

    /* Get attributes */

    virtual void minimumSize(int & height, int & width) const;
    virtual void preferredSize(int & height, int & width) const;

    /* Draws frame, items, etc. as needed */
 
    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec();
};