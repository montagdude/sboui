#include <iostream>
#include <string>
#include <vector>
#include <curses.h>
#include <cstdlib>   // getenv
#include <sstream>
#include <libconfig.h++>
#include "Color.h"
#include "settings.h"

namespace settings
{
  std::string repo_dir, repo_tag;
  std::string package_manager;
  std::string sync_cmd;
  std::string install_cmd;
  std::string upgrade_cmd;
  std::string install_clos;
  std::string install_vars;
  std::string upgrade_clos;
  std::string upgrade_vars;
  std::string editor;
  std::string color_theme_file;
  std::string layout;
  bool resolve_deps, confirm_changes, enable_color;
}

Color colors;
color_struct color_settings, dark, light, mc_like, user;

using namespace settings;
using namespace libconfig;

const std::string default_conf_file = "/etc/sboui/sboui.conf";

/*******************************************************************************

Constructor for color_struct

*******************************************************************************/
color_struct::color_struct ()
{
  fg_normal = "";     
  bg_normal = "";     
  fg_title = "";
  bg_title = "";
  fg_info = "";
  bg_info = "";
  fg_highlight_active = "";
  bg_highlight_active = "";
  fg_highlight_inactive = "";
  bg_highlight_inactive = "";
  header = "";
  header_popup = "";
  tagged = "";
  fg_popup = "";
  bg_popup = "";
  fg_warning = "";
  bg_warning = "";
  hotkey = "";
  fg_combobox = "";
  bg_combobox = "";
}

/*******************************************************************************

Defines built-in color themes

*******************************************************************************/
void define_color_themes()
{
  dark.fg_normal = "white";     
  dark.bg_normal = "black";     
  dark.fg_title = "brightwhite";
  dark.bg_title = "blue";
  dark.fg_info = "brightwhite";
  dark.bg_info = "blue";
  dark.fg_highlight_active = "brightwhite";
  dark.bg_highlight_active = "cyan";
  dark.fg_highlight_inactive = "black";
  dark.bg_highlight_inactive = "white";
  dark.header = "brightyellow";
  dark.header_popup = "brightblack";
  dark.tagged = "brightred";
  dark.fg_popup = "blue";
  dark.bg_popup = "white";
  dark.fg_warning = "white";
  dark.bg_warning = "red";
  dark.hotkey = "brightblack";
  dark.fg_combobox = "blue";
  dark.bg_combobox = "white";
  
  light.fg_normal = "black";
  light.bg_normal = "white";
  light.fg_title = "brightwhite";
  light.bg_title = "cyan";
  light.fg_info = "brightwhite";
  light.bg_info = "cyan";
  light.fg_highlight_active = "brightwhite";
  light.bg_highlight_active = "blue";
  light.fg_highlight_inactive = "white";
  light.bg_highlight_inactive = "black";
  light.header = "brightred";
  light.header_popup = "brightwhite";
  light.tagged = "brightred";
  light.fg_popup = "white";
  light.bg_popup = "black";
  light.fg_warning = "white";
  light.bg_warning = "red";
  light.hotkey = "brightcyan";
  light.fg_combobox = "white";
  light.bg_combobox = "black";
  
  mc_like.fg_normal = "white";
  mc_like.bg_normal = "blue";
  mc_like.fg_title = "brightwhite";
  mc_like.bg_title = "cyan";
  mc_like.fg_info = "brightwhite";
  mc_like.bg_info = "cyan";
  mc_like.fg_highlight_active = "black";
  mc_like.bg_highlight_active = "cyan";
  mc_like.fg_highlight_inactive = "white";
  mc_like.bg_highlight_inactive = "black";
  mc_like.header = "brightyellow";
  mc_like.header_popup = "blue";
  mc_like.tagged = "brightyellow";
  mc_like.fg_popup = "black";
  mc_like.bg_popup = "white";
  mc_like.fg_warning = "white";
  mc_like.bg_warning = "red";
  mc_like.hotkey = "blue";
  mc_like.fg_combobox = "black";
  mc_like.bg_combobox = "white";
}

/*******************************************************************************

Sets default colors

*******************************************************************************/
void set_default_colors() { color_settings = dark; }

/*******************************************************************************

Copies user colors into color_settings variables. Returns 1 if any user color
has not been set, 0 otherwise.

*******************************************************************************/
int set_from_user_colors()
{
  if (user.fg_normal != "") { color_settings.fg_normal = user.fg_normal; }
  else { return 1; }
  if (user.bg_normal != "") { color_settings.bg_normal = user.bg_normal; }
  else { return 1; }
  if (user.fg_title != "") { color_settings.fg_title = user.fg_title; }
  else { return 1; }
  if (user.bg_title != "") { color_settings.bg_title = user.bg_title; }
  else { return 1; }
  if (user.fg_info != "") { color_settings.fg_info = user.fg_info; }
  else { return 1; }
  if (user.bg_info != "") { color_settings.bg_info = user.bg_info; }
  else { return 1; }
  if (user.fg_highlight_active != "")
    color_settings.fg_highlight_active = user.fg_highlight_active;
  else { return 1; }
  if (user.bg_highlight_active != "")
    color_settings.bg_highlight_active = user.bg_highlight_active;
  else { return 1; }
  if (user.fg_highlight_inactive != "")
    color_settings.fg_highlight_inactive = user.fg_highlight_inactive;
  else { return 1; }
  if (user.bg_highlight_inactive != "")
    color_settings.bg_highlight_inactive = user.bg_highlight_inactive;
  else { return 1; }
  if (user.header != "") { color_settings.header = user.header; }
  else { return 1; }
  if (user.header_popup != "") 
    color_settings.header_popup = user.header_popup;
  else { return 1; }
  if (user.tagged != "") { color_settings.tagged = user.tagged; }
  else { return 1; }
  if (user.fg_popup != "") { color_settings.fg_popup = user.fg_popup; }
  else { return 1; }
  if (user.bg_popup != "") { color_settings.bg_popup = user.bg_popup; }
  else { return 1; }
  if (user.fg_warning != "") { color_settings.fg_warning = user.fg_warning; }
  else { return 1; }
  if (user.bg_warning != "") { color_settings.bg_warning = user.bg_warning; }
  else { return 1; }
  if (user.hotkey != "") { color_settings.hotkey = user.hotkey; }
  else { return 1; }
  if (user.fg_combobox != "") { color_settings.fg_combobox = user.fg_combobox; }
  else { return 1; }
  if (user.bg_combobox != "") { color_settings.bg_combobox = user.bg_combobox; }
  else { return 1; }

  return 0;
}

/*******************************************************************************

Applies color settings in curses

*******************************************************************************/
void apply_color_settings()
{
  start_color();
  colors.clear();
  colors.addPair(color_settings.fg_normal, color_settings.bg_normal);
  colors.addPair(color_settings.fg_title, color_settings.bg_title);
  colors.addPair(color_settings.fg_info, color_settings.bg_info);
  colors.addPair(color_settings.fg_highlight_active,
                 color_settings.bg_highlight_active);
  colors.addPair(color_settings.fg_highlight_inactive,
                 color_settings.bg_highlight_inactive);
  colors.addPair(color_settings.header, color_settings.bg_normal); 
  colors.addPair(color_settings.header_popup, color_settings.bg_popup); 
  colors.addPair(color_settings.fg_popup, color_settings.bg_popup); 
  colors.addPair(color_settings.tagged, color_settings.bg_normal); 
  colors.addPair(color_settings.tagged, color_settings.bg_highlight_active); 
  colors.addPair(color_settings.tagged, color_settings.bg_highlight_inactive); 
  colors.addPair(color_settings.tagged, color_settings.bg_popup); 
  colors.addPair(color_settings.fg_warning, color_settings.bg_warning); 
  colors.addPair(color_settings.hotkey, color_settings.bg_normal); 
  colors.addPair(color_settings.hotkey, color_settings.bg_popup); 
  colors.addPair(color_settings.hotkey, color_settings.bg_highlight_active); 
  colors.addPair(color_settings.hotkey, color_settings.bg_highlight_inactive); 
  colors.addPair(color_settings.fg_combobox, color_settings.bg_combobox); 
  colors.setBackground(stdscr, color_settings.fg_normal,
                               color_settings.bg_normal);
} 

/*******************************************************************************

Reads color theme configuration file

*******************************************************************************/
int read_color_theme(const std::string & color_theme_file)
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
    std::cerr << "Error cannot read color theme file." << std::endl;
    return 1;
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return 1;
  }

  // Store settings in vectors so we can read them in a loop

  color_vars.push_back(&user.fg_normal);
  color_vars.push_back(&user.bg_normal);
  color_vars.push_back(&user.fg_title);
  color_vars.push_back(&user.bg_title);
  color_vars.push_back(&user.fg_info);
  color_vars.push_back(&user.bg_info);
  color_vars.push_back(&user.fg_highlight_active);
  color_vars.push_back(&user.bg_highlight_active);
  color_vars.push_back(&user.fg_highlight_inactive);
  color_vars.push_back(&user.bg_highlight_inactive);
  color_vars.push_back(&user.header);
  color_vars.push_back(&user.header_popup);
  color_vars.push_back(&user.tagged);
  color_vars.push_back(&user.fg_popup);
  color_vars.push_back(&user.bg_popup);
  color_vars.push_back(&user.fg_warning);
  color_vars.push_back(&user.bg_warning);
  color_vars.push_back(&user.hotkey);
  color_vars.push_back(&user.fg_combobox);
  color_vars.push_back(&user.bg_combobox);

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
      retval = 1;
      break;
    }
  }

  return retval;
}

/*******************************************************************************

Reads settings from configuration file

*******************************************************************************/
int read_config(const std::string & conf_file)
{
  int check;
  Config cfg;
  std::string my_conf_file, home, response;
  char *env_home, *env_editor;

  // Determine config file to read

  if (conf_file != "") { my_conf_file = conf_file; }
  else
  {
    env_home = std::getenv("HOME");
    if (env_home != NULL)
    { 
      std::stringstream sshm;
      sshm << env_home;
      sshm >> home;
      my_conf_file = home + "/.sboui.conf";
    }
    else { my_conf_file = ""; }
  }

  // Read default config file if no user file

  try { cfg.readFile(my_conf_file.c_str()); }
  catch(const FileIOException &fioex)
  {
    my_conf_file = default_conf_file;
    try { cfg.readFile(my_conf_file.c_str()); }
    catch(const FileIOException &fioex1)
    {
      std::cerr << "Error: cannot read configuration file " << my_conf_file
                << "." << std::endl;
      return 1;
    }
    catch(const ParseException &pex1)
    {
      std::cerr << "Parse error at " << pex1.getFile() << ":" << pex1.getLine()
                << " - " << pex1.getError() << std::endl;
      return 1;
    }
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return 1;
  }

  // Read inputs and/or set defaults

  if (! cfg.lookupValue("resolve_deps", resolve_deps)) { resolve_deps = true; }

  if (! cfg.lookupValue("confirm_changes", confirm_changes)) 
    confirm_changes = true;

  if (! cfg.lookupValue("enable_color", enable_color)) { enable_color = true; }

  if (! cfg.lookupValue("layout", layout)) { layout = "horizontal"; }
  if ( (layout != "horizontal") && (layout != "vertical") )
  {
    layout = "horizontal";
    std::cout << "Unrecognized layout option. Using default." << std::endl;
    std::cout << "Press Enter to continue ...";
    std::getline(std::cin, response);
  }

  if (! cfg.lookupValue("editor", editor))
  {
    env_editor = std::getenv("EDITOR");
    if (env_editor != NULL)
    { 
      std::stringstream ssed;
      ssed << env_editor;
      ssed >> editor;
    }
    else { editor = "vi"; }
  }

  if (! cfg.lookupValue("install_clos", install_clos)) { install_clos = ""; }

  if (! cfg.lookupValue("install_vars", install_vars)) { install_vars = ""; }

  if (! cfg.lookupValue("upgrade_clos", upgrade_clos)) { upgrade_clos = ""; }

  if (! cfg.lookupValue("upgrade_vars", upgrade_vars)) { upgrade_vars = ""; }

  // Package manager settings

  if (! cfg.lookupValue("package_manager", package_manager))
  {
    std::cerr << "Error: No package_manager setting in "
              << my_conf_file << "." << std::endl;
    return 1;
  }

  if (! cfg.lookupValue("repo_dir", repo_dir))
  {
    std::cerr << "Error: no repo_dir setting in "
              << my_conf_file << "." << std::endl;
    return 1;
  }

  if (! cfg.lookupValue("repo_tag", repo_tag)) { repo_tag = "_SBo"; }

  if ( (package_manager != "sbopkg") && (package_manager != "sbotools") &&
       (package_manager != "custom") )
  {
    std::cerr << "Error: package_manager must be sbopkg, sbotools, or custom." 
              << std::endl;
    return 1;
  }

  if (! cfg.lookupValue("sync_cmd", sync_cmd))
  { 
    if (package_manager == "sbopkg") { sync_cmd = "sbopkg -r"; }
    else if (package_manager == "sbotools") { sync_cmd = "sbosnap update"; }
    else
    {
      std::cerr << "Error: must specify sync_cmd for custom package_manager."
                << std::endl;
      return 1;
    }
  }

  if (! cfg.lookupValue("install_cmd", install_cmd))
  {
    if (package_manager == "sbopkg") { install_cmd = "sbopkg -B -i"; }
    else if (package_manager == "sbotools") { install_cmd = "sboinstall -r"; } 
    else
    {
      std::cerr << "Error: must specify install_cmd for custom package_manager."
                << std::endl;
      return 1;
    }
  }

  if (! cfg.lookupValue("upgrade_cmd", upgrade_cmd))
  {
    if (package_manager == "sbopkg") { upgrade_cmd = "sbopkg -B -i"; }
    else if (package_manager == "sbotools") { upgrade_cmd = "sboupgrade -r"; } 
    else
    {
      std::cerr << "Error: must specify upgrade_cmd for custom package_manager."
                << std::endl;
      return 1;
    }
  }

  // Color settings. Try to read user settings or revert to defaults.

  if (cfg.lookupValue("color_theme_file", color_theme_file))
  { 
    check = read_color_theme(color_theme_file); 
    if (check != 0)
    {
      std::cout << "Using default colors. Press Enter to continue ...";
      std::getline(std::cin, response);
      set_default_colors();
    }
    else { set_from_user_colors(); }
  }
  else 
  { 
    color_theme_file = "";
    set_default_colors(); 
  }

  return 0;
}

/*******************************************************************************

Enables color. Returns 1 if terminal does not support color; 0 otherwise.

*******************************************************************************/
int activate_color()
{
  int check;

  // Define built-in themes if not done yet

  if (dark.fg_normal == "") { define_color_themes(); }

  if (has_colors())
  {
    if (color_theme_file != "")
    {
      check = set_from_user_colors();
      if (check != 0) { set_default_colors(); }
    }
    else { set_default_colors(); }
    apply_color_settings();
    enable_color = true;
    return 0;
  }
  else
  {
    enable_color = false;
    return 1;
  }
}

/*******************************************************************************

Disables color

*******************************************************************************/
void deactivate_color()
{
  if (has_colors()) 
  { 
    set_default_colors();
    apply_color_settings(); 
  }
  colors.clear();
  enable_color = false;
}   
