#pragma once

#include <string>
#include <vector>
#include "TextInput.h"
#include "Label.h"
#include "BuildListItem.h"
#include "InputBox.h"

/*******************************************************************************

Input box for build options

*******************************************************************************/
class BuildOptionsBox: public InputBox {

  private:

    Label _addlbl, _removelbl;
    std::vector<TextInput> _entries;

    // Place items

    void setUp();

    // Add/remove entries

    void addEntry();
    void removeLast();    

  public:

    /* Constructor */

    BuildOptionsBox();

    /* Set attributes */

    void setBuild(const BuildListItem & build);

    /* Get attributes */

    unsigned int numEntries() const;
    std::string entry(unsigned int idx) const;
    std::string entries() const;

    /* Draws frame, items, etc. as needed */

    void draw(bool force=false);

    /* User interaction loop */

    std::string exec();
};
