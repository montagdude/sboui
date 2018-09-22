#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"
#include "MouseEvent.h"

/*******************************************************************************

Toggle input item

*******************************************************************************/
class ToggleInput: public InputItem {

  private:

    bool _enabled;

    /* Drawing */

    void redrawEntry(int y_offset) const;
    void redrawText(int y_offset) const;

  public:

    /* Constructors */

    ToggleInput();

    /* Setting properties */

    void setEnabled(bool enabled);
    void toggle();

    /* User interaction */

    void draw(int y_offset, bool force=false, bool highlight=false);
    std::string exec(int y_offset, MouseEvent * mevent=NULL);

    /* Accessing properties. */

    bool getBoolProp() const;
    bool enabled() const;
};
