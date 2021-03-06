#pragma once

#include <string>
#include <curses.h>
#include "ComboBoxList.h"
#include "MouseEvent.h"

/*******************************************************************************

List displayed by entries in the Menubar. Differs from ComboBox list only in
that it has a border.

*******************************************************************************/
class MenubarList: public ComboBoxList {
  
  protected:

    unsigned int _outerpad, _innerpad;
    int _hotkey;

    void redrawFrame();
    void redrawSingleItem(unsigned int idx);

  public:

    /* Constructors */

    MenubarList();
    MenubarList(WINDOW *win);

    /* Setting properties */

    void setPad(unsigned int outerpad, unsigned int innerpad);
    void setHotKey(int hotkey);

    /* Accessing properties */

    int hotKey() const;
    void minimumSize(int & height, int & width) const;
    void preferredSize(int & height, int & width) const;
};
