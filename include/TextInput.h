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

    /* Prints to end of line, padding with spaces and avoiding borders */

    void printToEol(const std::string & msg) const;
    void printSpaces(unsigned int nspaces) const;

    /* Determines first character to print in input box */

    unsigned int determineFirstText();

    /* Drawing */
    
    void redrawEntry() const;

  public:

    /* Constructors */

    TextInput();

    /* User interaction */

    void draw(bool force=false);
    std::string exec(bool highlight=false);

    /* Accessing properties. */

    std::string getStringProp() const;
};
