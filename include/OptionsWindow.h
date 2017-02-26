#pragma once

#include <string>
#include <curses.h>
#include "TextInput.h"
#include "ToggleInput.h"
#include "Label.h"
#include "InputBox.h"

/*******************************************************************************

Options window

*******************************************************************************/
class OptionsWindow: public InputBox {

  private:

    TextInput _iopts_inp, _ivars_inp, _uopts_inp, _uvars_inp, _editor_inp;
    TextInput _repo_inp, _sync_inp, _inst_inp, _upgr_inp;
    ToggleInput _resolve_toggle, _confirm_toggle;
    Label _basic, _advanced;

    /* Drawing */

    void redrawFrame() const;

  public:

    /* Constructor and destructor */

    OptionsWindow();
    ~OptionsWindow();

    /* Read/apply settings */

    void readSettings();
    void applySettings() const; 

    /* Window sizing and placement */

    void placeWindow();
};
