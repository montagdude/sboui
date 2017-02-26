#pragma once

#include <string>
#include <curses.h>
#include "InputItem.h"

/*******************************************************************************

String input item

*******************************************************************************/
class TextInput: public InputItem {

  private:

    std::string _entry;
    unsigned int _firsttext, _cursidx;

    /* Determines first character to print in input box */

    unsigned int determineFirstText();

    /* Drawing */
    
    void redrawEntry() const;

  public:

    /* Constructors */

    TextInput();

    /* Set properties */

    void setText(const std::string & text);
    void clear();

    /* User interaction */

    void draw(bool force=false, bool highlight=false);
    std::string exec();

    /* Accessing properties. */

    std::string getStringProp() const;
};
