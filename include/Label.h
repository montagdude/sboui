#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

Label input item

*******************************************************************************/
class Label: public InputItem {

  private:

    int _color_pair;
    bool _bold;

  public:

    /* Constructor */

    Label();

    /* Set attributes */

    void setColor(int color_pair);
    void setBold(bool bold);

    /* Drawing */

    void draw(bool force=false, bool highlight=false);
};
