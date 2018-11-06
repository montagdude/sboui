#pragma once

#include <vector>
#include <string>
#include <curses.h>
#include "HelpItem.h"
#include "HelpWindow.h"

/*******************************************************************************

Mouse help window: lists mouse bindings

*******************************************************************************/
class MouseHelpWindow: public HelpWindow {

  public:

    /* Constructor */

    MouseHelpWindow();

    /* Constructs list to display */
 
    void createList();
};
