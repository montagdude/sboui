#include <iostream>
#include <string>
#include <curses.h>
#include <libconfig.h++>
#include "Color.h"
#include "ColorTheme.h"

using namespace libconfig;

/*******************************************************************************

Constructors

*******************************************************************************/
ColorTheme::ColorTheme()
{
  _name = "";
  _fg_normal = "";
  _bg_normal = "";
  _fg_title = "";
  _bg_title = "";
  _fg_info = "";
  _bg_info = "";
  _fg_highlight_active = "";
  _bg_highlight_active = "";
  _fg_highlight_inactive = "";
  _bg_highlight_inactive = "";
  _header = "";
  _header_popup = "";
  _tagged = "";
  _fg_popup = "";
  _bg_popup = "";
  _fg_warning = "";
  _bg_warning = "";
  _hotkey = "";
  _fg_combobox = "";
  _bg_combobox = "";
}

/*******************************************************************************

Set or get name

*******************************************************************************/
void ColorTheme::setName(const std::string & name) { _name = name; }
const std::string & ColorTheme::name() const { return _name; }

/*******************************************************************************

Sets default color theme

*******************************************************************************/
void ColorTheme::setDefaultColors()
{
  setName("default (dark)");
  _fg_normal = "white";     
  _bg_normal = "black";     
  _fg_title = "brightwhite";
  _bg_title = "blue";
  _fg_info = "brightwhite";
  _bg_info = "blue";
  _fg_highlight_active = "brightwhite";
  _bg_highlight_active = "cyan";
  _fg_highlight_inactive = "black";
  _bg_highlight_inactive = "white";
  _header = "brightyellow";
  _header_popup = "brightblack";
  _tagged = "brightred";
  _fg_popup = "blue";
  _bg_popup = "white";
  _fg_warning = "white";
  _bg_warning = "red";
  _hotkey = "brightblack";
  _fg_combobox = "blue";
  _bg_combobox = "white";
}

/*******************************************************************************

Read color theme from file. Returns 1 if cannot read color theme file, 2 if
there is a parse error, 3 if an item is missing, or 0 for success.

*******************************************************************************/
int ColorTheme::read(const std::string & color_theme_file)
{
  Config color_cfg;
  unsigned int i, nsettings;
  int retval;
  std::vector<std::string> color_names;
  std::vector<std::string *> color_vars;
  const std::string missing_msg = " color not found.";

  // Read config file

  try { color_cfg.readFile(color_theme_file.c_str()); }
  catch(const FileIOException &fioex)
  {
    std::cerr << "Error: cannot read color theme file " 
              << color_theme_file << "." << std::endl;
    return 1;
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return 2;
  }

  // Read theme name

  if (! color_cfg.lookupValue("theme_name", _name))
  {
    std::cerr << "Error: no theme_name specified for " << color_theme_file
              << "." << std::endl;
    return 3;
  }

  // Store settings in vectors so we can read them in a loop

  color_vars.push_back(&_fg_normal);
  color_vars.push_back(&_bg_normal);
  color_vars.push_back(&_fg_title);
  color_vars.push_back(&_bg_title);
  color_vars.push_back(&_fg_info);
  color_vars.push_back(&_bg_info);
  color_vars.push_back(&_fg_highlight_active);
  color_vars.push_back(&_bg_highlight_active);
  color_vars.push_back(&_fg_highlight_inactive);
  color_vars.push_back(&_bg_highlight_inactive);
  color_vars.push_back(&_header);
  color_vars.push_back(&_header_popup);
  color_vars.push_back(&_tagged);
  color_vars.push_back(&_fg_popup);
  color_vars.push_back(&_bg_popup);
  color_vars.push_back(&_fg_warning);
  color_vars.push_back(&_bg_warning);
  color_vars.push_back(&_hotkey);
  color_vars.push_back(&_fg_combobox);
  color_vars.push_back(&_bg_combobox);

  color_names.push_back("fg_normal");
  color_names.push_back("bg_normal");
  color_names.push_back("fg_title");
  color_names.push_back("bg_title");
  color_names.push_back("fg_info");
  color_names.push_back("bg_info");
  color_names.push_back("fg_highlight_active");
  color_names.push_back("bg_highlight_active");
  color_names.push_back("fg_highlight_inactive");
  color_names.push_back("bg_highlight_inactive");
  color_names.push_back("header");
  color_names.push_back("header_popup");
  color_names.push_back("tagged");
  color_names.push_back("fg_popup");
  color_names.push_back("bg_popup");
  color_names.push_back("fg_warning");
  color_names.push_back("bg_warning");
  color_names.push_back("hotkey");
  color_names.push_back("fg_combobox");
  color_names.push_back("bg_combobox");

  // Try to read inputs, but stop if there is a problem

  nsettings = color_vars.size();
  retval = 0;
  for ( i = 0; i < nsettings; i++ )
  {
    if (! color_cfg.lookupValue(color_names[i], *color_vars[i]))
    {
      std::cerr << "Error: '" + color_names[i] + "'" + missing_msg << std::endl;
      retval = 3;
      break;
    }
  }

  return retval;
}

/*******************************************************************************

Sets UI colors from theme

*******************************************************************************/
void ColorTheme::applyTheme(Color & colors) const
{
  start_color();
  colors.clear();
  colors.addPair("fg_normal", _fg_normal, "bg_normal", _bg_normal);
  colors.addPair("fg_title", _fg_title, "bg_title", _bg_title);
  colors.addPair("fg_info", _fg_info, "bg_info", _bg_info);
  colors.addPair("fg_highlight_active", _fg_highlight_active,
                 "bg_highlight_active", _bg_highlight_active);
  colors.addPair("fg_highlight_inactive", _fg_highlight_inactive,
                 "bg_highlight_inactive", _bg_highlight_inactive);
  colors.addPair("header", _header, "bg_normal", _bg_normal); 
  colors.addPair("header_popup", _header_popup, "bg_popup", _bg_popup); 
  colors.addPair("fg_popup", _fg_popup, "bg_popup", _bg_popup); 
  colors.addPair("tagged", _tagged, "bg_normal", _bg_normal); 
  colors.addPair("tagged", _tagged, "bg_highlight_active",_bg_highlight_active);
  colors.addPair("tagged", _tagged,
                 "bg_highlight_inactive",_bg_highlight_inactive); 
  colors.addPair("tagged", _tagged, "bg_popup", _bg_popup); 
  colors.addPair("fg_warning", _fg_warning, "bg_warning", _bg_warning);
  colors.addPair("bg_warning", _bg_warning, "fg_warning", _fg_warning);
  colors.addPair("hotkey", _hotkey, "bg_normal", _bg_normal); 
  colors.addPair("hotkey", _hotkey, "bg_popup", _bg_popup); 
  colors.addPair("hotkey", _hotkey, "bg_highlight_active",_bg_highlight_active);
  colors.addPair("hotkey", _hotkey,
                 "bg_highlight_inactive",_bg_highlight_inactive); 
  colors.addPair("fg_combobox", _fg_combobox, "bg_combobox", _bg_combobox); 
  colors.setBackground(stdscr, "fg_normal", "bg_normal");
}
