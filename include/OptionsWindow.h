#pragma once

#include <string>
#include <curses.h>
#include "TextInput.h"
#include "ToggleInput.h"
#include "Label.h"
#include "ComboBox.h"
#include "InputBox.h"
#include "MouseEvent.h"

/*******************************************************************************

Options window

*******************************************************************************/
class OptionsWindow: public InputBox {

  private:

    TextInput _iclos_inp, _ivars_inp, _uclos_inp, _uvars_inp; 
    TextInput _repo_inp, _tag_inp, _sync_inp, _inst_inp, _upgr_inp, _reinst_inp;
    ToggleInput _resolve_toggle, _rebuild_toggle, _confirm_toggle,
                _buildopts_toggle, _color_toggle, _warninval_toggle,
                _cumfilt_toggle;
    Label _ui_settings, _color_settings, _pm_settings, _misc_settings;
    ComboBox _layout_box, _color_box, _pmgr_box;

    /* Drawing */

    void redrawFrame();

  public:

    /* Constructor and destructor */

    OptionsWindow();
    ~OptionsWindow();

    /* Read/apply settings */

    void readSettings();
    void applySettings(int & check_color, int & check_write,
                       const std::string & conf_file="") const;

    /* Window sizing and placement */

    void placeWindow();

    /* User interaction loop */

    std::string exec(MouseEvent * mevent=NULL);

    /* Automatic defaults dialog when package manager is changed */

    void askSetDefaults(const std::string & new_pkg_mgr,
                        MouseEvent * mevent=NULL); 
};
