#pragma once

#include "HelpItem.h"
#include "HelpWindow.h"

/*******************************************************************************

Key help window: lists keyboard shortcuts

*******************************************************************************/
class KeyHelpWindow: public HelpWindow {

  public:

    /* Constructor */

    KeyHelpWindow();

    /* Constructs list to display */
 
    void createList();
};
