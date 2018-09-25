#pragma once

#include <string>
#include <curses.h>
#include "ListBox.h"
#include "MouseEvent.h"

/*******************************************************************************

ListBox for categories

*******************************************************************************/
class CategoryListBox: public ListBox {
  
  private:

    void redrawFrame();
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    CategoryListBox();
    CategoryListBox(WINDOW *win, const std::string & name);

    /* User interaction loop */

    std::string exec(MouseEvent * mevent=NULL);
};
