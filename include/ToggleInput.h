#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

Toggle input item

*******************************************************************************/
class ToggleInput: public InputItem {

  private:

    bool _enabled;

    /* Drawing */

    void redrawEntry() const;
    void redrawText() const;

  public:

    /* Constructors */

    ToggleInput();

    /* Setting properties */

    void setEnabled(bool enabled);
    void toggle();

    /* User interaction */

    void draw(bool force=false, bool highlight=false);
    std::string exec();

    /* Accessing properties. */

    bool getBoolProp() const;
};
