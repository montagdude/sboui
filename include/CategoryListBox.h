#pragma once

#include <string>
#include <curses.h>
#include "ListBox.h"

/*******************************************************************************

ListBox for categories

*******************************************************************************/
class CategoryListBox: public ListBox {
  
  private:

    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    CategoryListBox();
    CategoryListBox(WINDOW *win, const std::string & name);

    /* User interaction loop */

    std::string exec();
};
