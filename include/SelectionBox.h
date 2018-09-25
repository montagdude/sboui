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

    void redrawFrame();
    virtual void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    SelectionBox();
    SelectionBox(WINDOW *win, const std::string & name);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;

    /* Drawing */

    virtual void draw(bool force=false);

    /* User interaction loop */

    virtual std::string exec(MouseEvent * mevent=NULL);
};
