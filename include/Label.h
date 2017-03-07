#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

Label input item

*******************************************************************************/
class Label: public InputItem {

  private:

    int _color_idx;

  public:

    /* Constructor */

    Label();

    /* Set attributes */

    void setColor(int color_idx);

    /* Drawing */

    void draw(int y_offset, bool force=false, bool highlight=false);
};
