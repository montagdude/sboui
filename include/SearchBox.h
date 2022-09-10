#pragma once

#include <string>
#include "TextInput.h"
#include "ToggleInput.h"
#include "InputBox.h"

/*******************************************************************************

Search dialog

*******************************************************************************/
class SearchBox: public InputBox {

  private:

    TextInput _entryitem;
    ToggleInput _caseitem, _wholeitem, _readmeitem, _currentlistitem;

    /* Drawing */

    void redrawFrame();

  public:

    /* Constructor and destructor */

    SearchBox();
    ~SearchBox();

    /* Set attributes */

    void clearSearch();

    /* Get attributes */

    std::string searchString() const;
    bool caseSensitive() const;
    bool wholeWord() const;
    bool searchREADMEs() const;
    bool currentList() const;
};
