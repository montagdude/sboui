#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "HelpItem.h"
#include "ScrollBox.h"

/*******************************************************************************

Help window: lists keyboard shortcuts (abstract version)

*******************************************************************************/
class HelpWindow: public ScrollBox {

  protected:

    unsigned int _shortcutwidth;
    std::string _leftlabel, _rightlabel;

    /* List of items to display */

    std::vector<HelpItem> _helpitems;

    /* Drawing */

    void redrawFrame();
    void redrawSingleItem(unsigned int idx);

    /* Width needed for right column */

    void shortcutWidth();

  public:

    /* Constructors and destructor */

    HelpWindow();
    HelpWindow(WINDOW *win, const std::string & name);
    ~HelpWindow();

    /* Set labels (usually 'Action' and 'Shortcut') */

    void setLabels(const std::string & leftlabel,
                   const std::string & rightlabel);

    /* Window sizing and placement */

    void placeWindow() const;

    /* Constructs list to display */
 
    virtual void createList() = 0;
};
