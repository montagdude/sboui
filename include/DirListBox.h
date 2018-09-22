#pragma once

#include <string>
#include <curses.h>
#include "DirListing.h"
#include "SelectionBox.h"
#include "MouseEvent.h"

/*******************************************************************************

Selection to display and view files in a directory

*******************************************************************************/
class DirListBox: public SelectionBox {
  
  private:

    std::string _topdir, _currentdir;
    bool _limit_topdir;

    void redrawSingleItem(unsigned int idx);
    int navigateUp();
    void clear(); 

  public:

    /* Constructors and destructor */

    DirListBox();
    DirListBox(WINDOW *win, const std::string & name);
    ~DirListBox();

    /* Setting properties */

    void limitTopDir(bool limit);
    int setDirectory(const std::string & directory, bool reset_topdir=false);

    /* Accessing properties */

    const std::string & directory() const;

    /* User interaction loop */

    std::string exec(MouseEvent * mevent=NULL);
};
