#pragma once

#include <string>
#include <curses.h>
#include "DirListing.h"
#include "SelectionBox.h"

/*******************************************************************************

Selection to display and view files in a directory

*******************************************************************************/
class DirListBox: public SelectionBox {
  
  private:

    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors and destructor */

    DirListBox();
    DirListBox(WINDOW *win, const std::string & name);
    ~DirListBox();

    /* Setting properties */

    int setDirectory(const std::string & directory);

    /* User interaction loop */

    std::string exec();
};
