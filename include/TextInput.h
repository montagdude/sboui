#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"
#include "MouseEvent.h"

/*******************************************************************************

String input item

*******************************************************************************/
class TextInput: public InputItem {

  protected:

    std::string _entry, _label;
    unsigned int _firsttext, _cursidx, _labellen;

    /* Determines first character to print in input box */

    unsigned int determineFirstText();

    /* Drawing */
    
    void redrawEntry(int y_offset) const;

  public:

    /* Constructors */

    TextInput();

    /* Set properties */

    void setText(const std::string & text);
    void clear();
    void setLabel(const std::string & label);
    void removeLabel();

    /* User interaction */

    std::string handleMouseEvent(MouseEvent * mevent, int y_offset);
    void draw(int y_offset, bool force=false, bool highlight=false);
    virtual std::string exec(int y_offset, MouseEvent * mevent=NULL);

    /* Accessing properties */

    std::string getStringProp() const;
    std::string text() const;
};
