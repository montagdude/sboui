#include <string>
#include <curses.h>
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
  std::string env;
  std::string editor;
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

/*******************************************************************************

Reads settings from configuration file

*******************************************************************************/
int read_config()
{
  repo_dir = "/var/cache/packages/SBo";
  //repo_dir = "/data/dprosser/software/sboui_files/SBo";
  package_manager = "sbomgr";
  sync_cmd = "sbomgr update";
  //FIXME: put the following in the manpage
  /* When reinstalling, sboui does not first remove the existing package. To
     avoid duplicate packages from being installed in this situation, install_cmd
     should implement upgradepkg --reinstall --install-new instead of installpkg.
     sbopkg and sbotools both use this approach. */
  install_cmd = "sbomgr install -n";
  upgrade_cmd = "sbomgr upgrade";
  install_vars = "";
  install_opts = "";
  upgrade_vars = "";
  upgrade_opts = "";
  
  // Config variables to always pass to sboutil
  env = "REPO_DIR=" + repo_dir + " TAG=SBo ";
  
  // Editor for viewing files 
  editor = "vim";

  return 0;
}

/*******************************************************************************

Sets colors

*******************************************************************************/
int set_colors()
{
  if (has_colors())
  {
    // Dark color scheme
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

    // Light color scheme
    //fg_normal = "black";
    //bg_normal = "white";
    //fg_title = "white";
    //bg_title = "cyan";
    //fg_info = "white";
    //bg_info = "cyan";
    //fg_highlight_active = "white";
    //bg_highlight_active = "blue";
    //fg_highlight_inactive = "white";
    //bg_highlight_inactive = "black";
    //header = "red";
    //header_popup = "white";
    //tagged = "red";
    //fg_popup = "white";
    //bg_popup = "black";
    //fg_warning = "white";
    //bg_warning = "red";

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

  return 0;
}
