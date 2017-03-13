#include <iostream>
#include <string>
#include <vector>
#include <curses.h>
#include <libconfig.h++>
#include "Color.h"
#include "settings.h"

#ifndef SYSCONFDIR
  #define SYSCONFDIR "/etc/sboui"
#endif

namespace settings
{
  std::string repo_dir;
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
  std::string env;
  bool resolve_deps, confirm_changes, enable_color;
}

namespace color_settings
{
  Color colors;
  std::string fg_normal, bg_normal;
  std::string fg_title, bg_title;
  std::string fg_info, bg_info;
  std::string fg_highlight_active, bg_highlight_active;
  std::string fg_highlight_inactive, bg_highlight_inactive;
  std::string header, header_popup;
  std::string tagged;
  std::string fg_popup, bg_popup;
  std::string fg_warning, bg_warning;
  std::string hotkey;
  std::string fg_combobox, bg_combobox;
}

using namespace settings;
using namespace color_settings;
using namespace libconfig;

/*******************************************************************************

Sets default colors

*******************************************************************************/
void set_default_colors()
{
  fg_normal = "white";     
  bg_normal = "black";     
  fg_title = "brightwhite";
  bg_title = "blue";
  fg_info = "brightwhite";
  bg_info = "blue";
  fg_highlight_active = "brightwhite";
  bg_highlight_active = "cyan";
  fg_highlight_inactive = "black";
  bg_highlight_inactive = "white";
  header = "brightyellow";
  header_popup = "brightblack";
  tagged = "brightred";
  fg_popup = "blue";
  bg_popup = "white";
  fg_warning = "white";
  bg_warning = "red";
  hotkey = "brightblack";
  fg_combobox = "blue";
  bg_combobox = "white";
}

/*******************************************************************************

Applies color settings in curses

*******************************************************************************/
void apply_color_settings()
{
  start_color();
  colors.clear();
  colors.addPair(fg_normal, bg_normal);
  colors.addPair(fg_title, bg_title);
  colors.addPair(fg_info, bg_info);
  colors.addPair(fg_highlight_active, bg_highlight_active);
  colors.addPair(fg_highlight_inactive, bg_highlight_inactive);
  colors.addPair(header, bg_normal); 
  colors.addPair(header_popup, bg_popup); 
  colors.addPair(tagged, bg_normal); 
  colors.addPair(tagged, bg_highlight_active); 
  colors.addPair(tagged, bg_highlight_inactive); 
  colors.addPair(fg_popup, bg_popup); 
  colors.addPair(fg_warning, bg_warning); 
  colors.addPair(hotkey, bg_normal); 
  colors.addPair(hotkey, bg_popup); 
  colors.addPair(hotkey, bg_highlight_active); 
  colors.addPair(hotkey, bg_highlight_inactive); 
  colors.addPair(fg_combobox, bg_combobox); 
  colors.setBackground(stdscr, fg_normal, bg_normal);
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

  color_vars.push_back(&fg_normal);
  color_vars.push_back(&bg_normal);
  color_vars.push_back(&fg_title);
  color_vars.push_back(&bg_title);
  color_vars.push_back(&fg_info);
  color_vars.push_back(&bg_info);
  color_vars.push_back(&fg_highlight_active);
  color_vars.push_back(&bg_highlight_active);
  color_vars.push_back(&fg_highlight_inactive);
  color_vars.push_back(&bg_highlight_inactive);
  color_vars.push_back(&header);
  color_vars.push_back(&header_popup);
  color_vars.push_back(&tagged);
  color_vars.push_back(&fg_popup);
  color_vars.push_back(&bg_popup);
  color_vars.push_back(&fg_warning);
  color_vars.push_back(&bg_warning);
  color_vars.push_back(&hotkey);
  color_vars.push_back(&fg_combobox);
  color_vars.push_back(&bg_combobox);

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
  std::string my_conf_file, response;

  // Read config file

  if (conf_file != "") { my_conf_file = conf_file; }
  else { my_conf_file = SYSCONFDIR "/sboui.cfg"; }

  try { cfg.readFile(my_conf_file.c_str()); }
  catch(const FileIOException &fioex)
  {
    std::cerr << "Error: cannot read configuration file " << my_conf_file
              << "." << std::endl;
    return 1;
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

  if (! cfg.lookupValue("editor", editor)) { editor = "vim"; }

  if (! cfg.lookupValue("install_clos", install_clos)) { install_clos = ""; }

  if (! cfg.lookupValue("install_vars", install_vars)) { install_vars = ""; }

  if (! cfg.lookupValue("upgrade_clos", upgrade_clos)) { upgrade_clos = ""; }

  if (! cfg.lookupValue("upgrade_vars", upgrade_vars)) { upgrade_vars = ""; }

  // Package manager settings

  if (! cfg.lookupValue("package_manager", package_manager))
  {
    std::cerr << "Error: No package_manager setting in sboui.cfg." << std::endl;
    return 1;
  }

  if (! cfg.lookupValue("repo_dir", repo_dir))
  {
    std::cerr << "Error: no repo_dir setting in sboui.cfg." << std::endl;
    return 1;
  }

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

  // Environment variables passed to sboutil

  env = "REPO_DIR=" + repo_dir + " TAG=SBo ";

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

  if (has_colors())
  {
    if (color_theme_file != "")
    {
      check = read_color_theme(color_theme_file);
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
