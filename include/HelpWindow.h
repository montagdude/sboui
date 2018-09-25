#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "HelpItem.h"
#include "ScrollBox.h"

/*******************************************************************************

Help window: lists keyboard shortcuts

*******************************************************************************/
class HelpWindow: public ScrollBox {

  private:

    unsigned int _shortcutwidth;

    /* List of items to display */

    std::vector<HelpItem> _helpitems;

    /* Drawing */

    void redrawFrame();
    void redrawSingleItem(unsigned int idx);

    /* Constructs list to display */
 
    void createList();

  public:

    /* Constructors and destructor */

    HelpWindow();
    HelpWindow(WINDOW *win, const std::string & name);
    ~HelpWindow();

    /* Window sizing and placement */

    void placeWindow() const;
};
