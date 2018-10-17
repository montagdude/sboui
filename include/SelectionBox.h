#pragma once

#include <string>
#include <curses.h>
#include "ListBox.h"
#include "MouseEvent.h"

/*******************************************************************************

ListBox for making a selection

*******************************************************************************/
class SelectionBox: public ListBox {

  protected:

    bool _modal;      // If false, mouse click outside of window will kick out
                      // of user interaction loop

  public:

    /* Constructors */

    SelectionBox();
    SelectionBox(WINDOW *win, const std::string & name);

    /* Set / get attributes */

    void setModal(bool modal);
    bool modal() const;

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
