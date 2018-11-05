#pragma once

#include <string>
#include <vector>
#include "Color.h"
#include "ColorTheme.h"

#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION ""
#endif

namespace settings
{
  extern std::string repo_dir, repo_tag;
  extern std::string package_manager;
  extern std::string sync_cmd, install_cmd, upgrade_cmd, reinstall_cmd;
  extern std::string install_clos, install_vars;
  extern std::string upgrade_clos, upgrade_vars;
  extern std::string editor, viewer;
  extern std::string color_theme;
  extern std::string layout;
  extern bool resolve_deps, confirm_changes, enable_color, rebuild_inv_deps;
  extern bool save_buildopts, warn_invalid_pkgnames;
}

extern Color colors;
extern std::vector<ColorTheme> color_themes;

int read_config(const std::string & conf_file="");
int write_config(const std::string & conf_file="");
int apply_color_theme(const std::string & theme);
int activate_color(const std::string & theme);
void deactivate_color();
