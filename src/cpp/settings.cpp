#include <iostream>
#include <string>
#include <curses.h>
#include <libconfig.h++>
#include "Color.h"
#include "settings.h"

namespace backend
{
  std::string repo_dir;
  std::string package_manager;
  std::string sync_cmd;
  std::string install_cmd;
  std::string upgrade_cmd;
  std::string install_vars;
  std::string install_opts;
  std::string upgrade_vars;
  std::string upgrade_opts;
  std::string editor;
  std::string color_theme_file;
  std::string env;
}

namespace color 
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
}

using namespace backend;
using namespace color;
using namespace libconfig;

/*******************************************************************************

Sets default colors

*******************************************************************************/
void set_default_colors()
{
  fg_normal = "white";     
  bg_normal = "black";     
  fg_title = "white";
  bg_title = "blue";
  fg_info = "white";
  bg_info = "blue";
  fg_highlight_active = "black";
  bg_highlight_active = "cyan";
  fg_highlight_inactive = "black";
  bg_highlight_inactive = "white";
  header = "yellow";
  header_popup = "black";
  tagged = "yellow";
  fg_popup = "blue";
  bg_popup = "white";
  fg_warning = "white";
  bg_warning = "red";
}

/*******************************************************************************

Applies color settings in curses

*******************************************************************************/
void apply_color_settings()
{
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
  bkgd(COLOR_PAIR(colors.pair(fg_normal, bg_normal)));
} 

/*******************************************************************************

Reads color theme configuration file

*******************************************************************************/
int read_color_theme(const std::string & color_theme_file)
{
  return 0;
}

/*******************************************************************************

Reads settings from configuration file

*******************************************************************************/
int read_config()
{
  int check;
  Config cfg;

  // Read config file

  try { cfg.readFile("/etc/sboui/sboui.cfg"); }
  catch(const FileIOException &fioex)
  {
    std::cerr << "Error: cannot read sboui.cfg." << std::endl;
    return 1;
  }
  catch(const ParseException &pex)
  {
    std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
              << " - " << pex.getError() << std::endl;
    return 1;
  }

  // Read inputs and/or set defaults

  try { cfg.lookupValue("repo_dir", repo_dir); }
  catch(const SettingNotFoundException &nfex)
  {
    std::cerr << "No repo_dir setting in sboui.cfg." << std::endl;
    return 1;
  }

  try { cfg.lookupValue("package_manager", package_manager); }
  catch(const SettingNotFoundException &nfex)
  {
    std::cerr << "No package_manager setting in sboui.cfg." << std::endl;
    return 1;
  }

  try { cfg.lookupValue("install_vars", install_vars); }
  catch(const SettingNotFoundException &nfex) { install_vars = ""; }

  try { cfg.lookupValue("install_opts", install_opts); }
  catch(const SettingNotFoundException &nfex) { install_opts = ""; }

  try { cfg.lookupValue("upgrade_vars", upgrade_vars); }
  catch(const SettingNotFoundException &nfex) { upgrade_vars = ""; }

  try { cfg.lookupValue("upgrade_opts", upgrade_opts); }
  catch(const SettingNotFoundException &nfex) { upgrade_opts = ""; }

  try { cfg.lookupValue("editor", editor); }
  catch(const SettingNotFoundException &nfex) { editor = "vim"; }

//FIXME: the following are necessary for custom package managers
  try { cfg.lookupValue("sync_cmd", sync_cmd); }
  catch(const SettingNotFoundException &nfex) { sync_cmd = "sbomgr update"; }

  try { cfg.lookupValue("install_cmd", install_cmd); }
  catch(const SettingNotFoundException &nfex) { install_cmd = 
                                                "sbomgr install -n"; }

  try { cfg.lookupValue("upgrade_cmd", upgrade_cmd); }
  catch(const SettingNotFoundException &nfex) { upgrade_cmd = 
                                                "sbomgr upgrade"; }
  
  // Config variables to always pass to sboutil

  env = "REPO_DIR=" + repo_dir + " TAG=SBo ";

  // Color settings

  try { cfg.lookupValue("color_theme_file", color_theme_file); }
  catch(const SettingNotFoundException &nfex) { color_theme_file = ""; }

  check = 1;
  if (color_theme_file != "") { check = read_color_theme(color_theme_file); }
  if (check == 1) 
  {
    set_default_colors();
    check = 0;
  } 
  
  return check;
}
