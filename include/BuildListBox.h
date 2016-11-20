#pragma once

#include <string>
#include "ListBox.h"

/*******************************************************************************

ListBox for categories

*******************************************************************************/
class BuildListBox: public ListBox {
  
  private:

    void redrawFrame() const;
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    BuildListBox();
    BuildListBox(WINDOW *win, const std::string & name);

    /* Setting properties */

    unsigned int tagAll();

    /* Accessing properties */

    bool allTagged() const;

    /* User interaction loop */

    std::string exec();
};
