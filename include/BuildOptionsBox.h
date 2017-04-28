#pragma once

#include <string>
#include "TextInput.h"
#include "BuildListItem.h"
#include "InputBox.h"

/*******************************************************************************

Input box for build options

*******************************************************************************/
class BuildOptionsBox: public InputBox {

  private:

    TextInput _entryitem;

  public:

    /* Constructors */

    BuildOptionsBox();

    /* Set attributes */

    void setBuild(const BuildListItem & build);

    /* Get attributes */

    std::string entry() const;

    /* Draws frame, items, etc. as needed */

    void draw(bool force=false);
};
