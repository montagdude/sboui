#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

String input item

*******************************************************************************/
class TextInput: public InputItem {

  protected:

    std::string _entry;
    unsigned int _firsttext, _cursidx;

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

    /* User interaction */

    void draw(int y_offset, bool force=false, bool highlight=false);
    virtual std::string exec(int y_offset);

    /* Accessing properties. */

    std::string getStringProp() const;
};
