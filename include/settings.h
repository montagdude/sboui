#pragma once

#include <string>
#include "Color.h"

namespace settings
{
  extern std::string repo_dir, repo_tag;
  extern std::string package_manager;
  extern std::string sync_cmd, install_cmd, upgrade_cmd;
  extern std::string install_clos, install_vars;
  extern std::string upgrade_clos, upgrade_vars;
  extern std::string editor; 
  extern std::string color_theme_file;
  extern std::string layout;
  extern bool resolve_deps, confirm_changes, enable_color;
}

extern Color colors;
struct color_struct {
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
  color_struct(); // Constructor
};
extern color_struct color_settings;

void apply_color_settings();
int read_config(const std::string & conf_file="");
int activate_color();
void deactivate_color();
