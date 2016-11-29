#pragma once

#include <vector>
#include <string>
#include "ListItem.h"
#include "SelectionBox.h"

/*******************************************************************************

SelectionBox for filters

*******************************************************************************/
class FilterBox: public SelectionBox {
  
  private:

    std::vector<ListItem> _choices;

  public:

    /* Constructors */

    FilterBox();
    FilterBox(WINDOW *win, const std::string & name);

    /* Draws frame, items, etc. as needed */

    void draw(bool force=false);
};
