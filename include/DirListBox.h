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

    unsigned int _level;
    std::string _currentdir;

    void redrawSingleItem(unsigned int idx);
    int navigateUp();
    void clear(); 

  public:

    /* Constructors and destructor */

    DirListBox();
    DirListBox(WINDOW *win, const std::string & name);
    ~DirListBox();

    /* Setting properties */

    int setDirectory(const std::string & directory, int levelchange=0);

    /* Accessing properties */

    const std::string & directory() const;

    /* User interaction loop */

    std::string exec();
};
