#pragma once

#include <curses.h>
#include "MessageBox.h"

/*******************************************************************************

Header for PackageInfoBox class

*******************************************************************************/
class PackageInfoBox: public MessageBox {

  private:

    void redrawMessage() const;

  public:

    /* Constructors */

    PackageInfoBox();
    PackageInfoBox(WINDOW *win);

    /* Get attributes */

    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;
};
