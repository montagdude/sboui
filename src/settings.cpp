#include <iostream>
#include <string>
#include <vector>
#include <curses.h>
#include <cstdlib>   // getenv
#include <sstream>
#include <libconfig.h++>
#include "Color.h"
#include "ColorTheme.h"
#include "DirListing.h"
#include "settings.h"

#ifndef DATADIR
  #define DATADIR "/usr/share/sboui"
#endif

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
  std::string color_theme;
  std::string layout;
  bool resolve_deps, confirm_changes, enable_color, rebuild_inv_deps;
  bool save_buildopts, warn_invalid_pkgnames;
}

Color colors;
std::vector<ColorTheme> color_themes;

using namespace settings;
using namespace libconfig;

const std::string default_conf_file = "/etc/sboui/sboui.conf";

/*******************************************************************************

Reads color themes from system and local directories

*******************************************************************************/
void get_color_themes()
{
  std::string env_home, system_themes_dir, user_themes_dir, theme_full_path;
  unsigned int i, ndirs, stat, j, nfiles, k, nthemes;
  DirListing themes_dir;
  direntry theme_file;

  env_home = std::getenv("HOME");
  system_themes_dir = DATADIR "/themes";
  user_themes_dir = env_home + "/.local/share/sboui/themes";
  ndirs = 2;
  for ( i = 0; i < ndirs; i++ )
  {
    if (i == 0) { stat = themes_dir.setFromPath(system_themes_dir); }
    else { stat = themes_dir.setFromPath(user_themes_dir); }
    if (stat == 1) { continue; }

    nfiles = themes_dir.size();
    for ( j = 0; j < nfiles; j++ )
    {
      theme_file = themes_dir(j);
      if (theme_file.type == "reg") 
      {
        ColorTheme theme;
        theme_full_path = theme_file.path + "/" + theme_file.name;
        stat = theme.read(theme_full_path);
        if (stat == 0)
        {
          nthemes = color_themes.size();
          for ( k = 0; k < nthemes; k++ )
          {
            if (theme.name() == color_themes[k].name())
            {
              std::cout << "Warning: theme " << theme_full_path << " masks "
                        << "previously defined '" << theme.name() << "' theme."
                        << std::endl;
              color_themes.erase(color_themes.begin()+k);
            }
          }
          color_themes.push_back(theme);
        }
      }
    }
  }
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
  ColorTheme default_theme;

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

  if (! cfg.lookupValue("resolve_deps", resolve_deps))
    resolve_deps = true;

  if (! cfg.lookupValue("rebuild_inv_deps", rebuild_inv_deps))
    rebuild_inv_deps = false;

  if (! cfg.lookupValue("confirm_changes", confirm_changes)) 
    confirm_changes = true;

  if (! cfg.lookupValue("save_buildopts", save_buildopts)) 
    save_buildopts = true;

  if (! cfg.lookupValue("warn_invalid_pkgnames", warn_invalid_pkgnames))
    warn_invalid_pkgnames = true;

  if (! cfg.lookupValue("layout", layout)) { layout = "horizontal"; }
  else if ( (layout != "horizontal") && (layout != "vertical") )
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

  if (! cfg.lookupValue("enable_color", enable_color)) { enable_color = true; }
  if (! cfg.lookupValue("color_theme", color_theme))
    color_theme = "default (dark)";

  // Set up color themes

  color_themes.clear();
  default_theme.setDefaultColors();
  color_themes.push_back(default_theme);
  apply_color_theme("default (dark)");
  get_color_themes(); 
  if (enable_color)
  {
    check = activate_color(color_theme);
    if (check != 0) 
    {
      std::cout << "Press Enter to continue ...";
      std::getline(std::cin, response);
    }
  }
  else { deactivate_color(); }

  return 0;
}

/*******************************************************************************

Writes configuration file to $HOME/.sboui.conf

*******************************************************************************/
int write_config(const std::string & conf_file)
{
  Config cfg;
  std::string home, my_conf_file;
  char *env_home;
  FILE *fp;

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
    else { return 1; }
  }

  // Add settings to cfg

  Setting &root = cfg.getRoot();
  root.add("resolve_deps", Setting::TypeBoolean) = resolve_deps;
  root.add("rebuild_inv_deps", Setting::TypeBoolean) = rebuild_inv_deps;
  root.add("confirm_changes", Setting::TypeBoolean) = confirm_changes;
  root.add("save_buildopts", Setting::TypeBoolean) = save_buildopts;
  root.add("layout", Setting::TypeString) = layout;
  root.add("editor", Setting::TypeString) = editor;
  root.add("install_clos", Setting::TypeString) = install_clos;
  root.add("install_vars", Setting::TypeString) = install_vars;
  root.add("upgrade_clos", Setting::TypeString) = upgrade_clos;
  root.add("upgrade_vars", Setting::TypeString) = upgrade_vars;
  root.add("package_manager", Setting::TypeString) = package_manager;
  root.add("repo_dir", Setting::TypeString) = repo_dir;
  root.add("repo_tag", Setting::TypeString) = repo_tag;
  root.add("sync_cmd", Setting::TypeString) = sync_cmd;
  root.add("install_cmd", Setting::TypeString) = install_cmd;
  root.add("upgrade_cmd", Setting::TypeString) = upgrade_cmd;
  root.add("enable_color", Setting::TypeBoolean) = enable_color;
  root.add("color_theme", Setting::TypeString) = color_theme;
  root.add("warn_invalid_pkgnames", Setting::TypeBoolean) =
                                                          warn_invalid_pkgnames;

  // Overwrite config file

  fp = fopen(my_conf_file.c_str(), "w");
  if (fp == NULL) { return 2; }
  fprintf(fp, "# sboui configuration file\n");
  fprintf(fp, "#\n");
  fprintf(fp, "# Any manual edits to this file will be overwritten when\n");
  fprintf(fp, "# changes are made in the Options window of sboui.\n");
  fprintf(fp, "#\n");
  try { cfg.write(fp); }
  catch(const FileIOException & fioex) { return 2; }
  fclose(fp);

  return 0;
}

/*******************************************************************************

Applies color theme. Returns 0 on success; 1 if not found.

*******************************************************************************/
int apply_color_theme(const std::string & theme)
{
  unsigned int i, nthemes;

  nthemes = color_themes.size();
  for ( i = 0; i < nthemes; i++ )
  {
    if (color_themes[i].name() == theme)
    {
      color_themes[i].applyTheme(colors);
      return 0;
    }
  }
  return 1;
} 

/*******************************************************************************

Enables color. Returns 1 if color theme file requested but could not be read,
2 if terminal does not support color.

*******************************************************************************/
int activate_color(const std::string & theme)
{
  int check;

  if (has_colors())
  {
    check = apply_color_theme(theme);
    if (check != 0)
    {
      std::cerr << "Unrecognized color theme '" + color_theme + "'."
                << std::endl;
      apply_color_theme("default (dark)");
    }
    enable_color = true;
    return check;
  }
  else
  {
    enable_color = false;
    std::cerr << "Color is not supported in this terminal." << std::endl;
    return 2;
  }
}

/*******************************************************************************

Disables color

*******************************************************************************/
void deactivate_color()
{
  if (has_colors()) { apply_color_theme("default (dark)"); }
  colors.clear();
  enable_color = false;
}   
