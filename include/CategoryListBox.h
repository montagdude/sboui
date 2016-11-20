#pragma once

#include <string>
#include "BuildListBox.h"

/*******************************************************************************

ListBox for categories

*******************************************************************************/
class CategoryListBox: public BuildListBox {
  
  private:

    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    CategoryListBox();
    CategoryListBox(WINDOW *win, const std::string & name);
};
