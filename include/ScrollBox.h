#pragma once

#include <string>
#include <curses.h>
#include "AbstractListBox.h"
#include "MouseEvent.h"

/*******************************************************************************

Same idea as a ListBox, but items are not selectable/highlightable.

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

    virtual void redrawSingleItem(unsigned int idx);
    virtual void redrawScrollIndicator() const;

  public:

    /* Constructors */

    ScrollBox();
    ScrollBox(WINDOW *win, const std::string & name);

    /* Mouse interaction */

    std::string handleMouseEvent(MouseEvent * mevent);

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
