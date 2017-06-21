#pragma once

#include <string>
#include "Color.h"

/*******************************************************************************

Color theme class

*******************************************************************************/
class ColorTheme {

  private:

    std::string _name;
    std::string _fg_normal, _bg_normal;
    std::string _fg_title, _bg_title;
    std::string _fg_info, _bg_info;
    std::string _fg_highlight_active, _bg_highlight_active;
    std::string _fg_highlight_inactive, _bg_highlight_inactive;
    std::string _header, _header_popup;
    std::string _tagged;
    std::string _fg_popup, _bg_popup;
    std::string _fg_warning, _bg_warning;
    std::string _hotkey;
    std::string _fg_combobox, _bg_combobox;

  public:

    /* Constructor */

    ColorTheme();

    /* Set or get name */

    void setName(const std::string & name);
    const std::string & name() const;

    /* Set default colors */

    void setDefaultColors();

    /* Read color theme from file */

    int read(const std::string & color_theme_file);

    /* Set UI colors from theme */

    void applyTheme(Color & colors) const;
};
