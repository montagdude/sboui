#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"
#include "MouseEvent.h"

/*******************************************************************************

Label input item

*******************************************************************************/
class Label: public InputItem {

  private:

    int _color_idx;
    bool _hline;

  public:

    /* Constructors and destructor */

    Label();
    Label(bool selectable);
    Label(bool selectable, bool hline);

    /* Set attributes */

    void setColor(int color_idx);
    void setSelectable(bool selectable);
    void setHLine(bool hline);

    /* User interaction */

    std::string handleMouseEvent(MouseEvent * mevent, int y_offset);
    void draw(int y_offset, bool force=false, bool highlight=false);
    std::string exec(int y_offset, MouseEvent * mevent=NULL);
};
