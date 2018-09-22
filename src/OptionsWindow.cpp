#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // min, max
#include "Color.h"
#include "signals.h"
#include "settings.h"
#include "TextInput.h"
#include "ToggleInput.h"
#include "Label.h"
#include "DefaultOptionsBox.h"
#include "MessageBox.h"
#include "OptionsWindow.h"
#include "MouseEvent.h"

using namespace settings;

/*******************************************************************************

Draws border and title

*******************************************************************************/
void OptionsWindow::redrawFrame() const
{
  int rows, cols, namelen, i, left, right;
  double mid;

  getmaxyx(_win, rows, cols);

  // Title

  namelen = _msg.size();
  mid = double(cols)/2.0;
  left = std::floor(mid - double(namelen)/2.0);
  right = left + namelen;
  wmove(_win, 0, left);
  wattron(_win, A_BOLD);
  wprintw(_win, _msg.c_str());
  wattroff(_win, A_BOLD);

  // Corners

  wmove(_win, 0, 0);
  waddch(_win, ACS_ULCORNER);
  wmove(_win, rows-1, 0);
  waddch(_win, ACS_LLCORNER);
  wmove(_win, rows-1, cols-1);
  waddch(_win, ACS_LRCORNER);
  wmove(_win, 0, cols-1);
  waddch(_win, ACS_URCORNER);

  // Top border

  wmove(_win, 0, 1);
  for ( i = 1; int(i) < left-1; i++ ) { waddch(_win, ACS_HLINE); }
  wmove(_win, 0, right+1);
  for ( i = right+1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
  
  // Left border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, 0, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
OptionsWindow::OptionsWindow()
{
  int count, line;
  unsigned int i, nthemes;

  _reserved_rows = 2;
  _header_rows = 1;
  _firstprint = _header_rows;
  _msg = "Options";
  _color_idx = colors.getPair("fg_normal", "bg_normal");

  count = 0;
  line = 1; 

  // UI settings

  _ui_settings.setColor(colors.getPair("header", "bg_normal"));
  addItem(&_ui_settings);
  _items[count]->setName("User interface settings");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  addItem(&_resolve_toggle);
  _items[count]->setName("Resolve dependencies");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 1;

  addItem(&_rebuild_toggle);
  _items[count]->setName("Rebuild inverse dependencies");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 1;

  addItem(&_confirm_toggle);
  _items[count]->setName("Ask for confirmation before applying changes");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 1;

  addItem(&_buildopts_toggle);
  _items[count]->setName("Save build options");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("List layout");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 0;

  _layout_box.setParent(this);
  _layout_box.addChoice("horizontal");
  _layout_box.addChoice("vertical");
  addItem(& _layout_box);
  _items[count]->setPosition(line,26);
  count++;
  line += 2;

  // Color settings

  _color_settings.setColor(colors.getPair("header", "bg_normal"));
  addItem(&_color_settings);  
  _items[count]->setName("Color settings");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  addItem(&_color_toggle);
  _items[count]->setName("Enable color");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Color theme");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(24);
  count++;
  line += 0;

  _color_box.setParent(this);
  nthemes = color_themes.size();
  for ( i = 0; i < nthemes; i++ )
  {
    _color_box.addChoice(color_themes[i].name());
  }
  addItem(& _color_box);
  _items[count]->setPosition(line,26);
  count++;
  line += 2;

  // Package manager settings

  _pm_settings.setColor(colors.getPair("header", "bg_normal"));
  addItem(&_pm_settings);  
  _items[count]->setName("Package manager settings");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  addItem(new Label());
  _items[count]->setName("Package manager");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 0;

  _pmgr_box.setParent(this);
  _pmgr_box.addChoice("sbopkg");
  _pmgr_box.addChoice("sbotools");
  _pmgr_box.addChoice("built-in");
  _pmgr_box.addChoice("custom");
  addItem(& _pmgr_box);
  _items[count]->setPosition(line,26);
  count++;
  line += 1;
  
  addItem(new Label());
  _items[count]->setName("Repository directory");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(20);
  count++;
  line += 0;

  addItem(&_repo_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Repository tag");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(20);
  count++;
  line += 0;

  addItem(&_tag_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Sync command");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(20);
  count++;
  line += 0;

  addItem(&_sync_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Install command");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(20);
  count++;
  line += 0;

  addItem(&_inst_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Upgrade command");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(20);
  count++;
  line += 0;

  addItem(&_upgr_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Reinstall command");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(20);
  count++;
  line += 0;

  addItem(&_reinst_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Additional install CLOs");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(24);
  count++;
  line += 0;

  addItem(&_iclos_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Install environment vars");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(24);
  count++;
  line += 0;

  addItem(&_ivars_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Additional upgrade CLOs");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(24);
  count++;
  line += 0;

  addItem(&_uclos_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 1;

  addItem(new Label());
  _items[count]->setName("Upgrade environment vars");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(24);
  count++;
  line += 0;

  addItem(&_uvars_inp);
  _items[count]->setPosition(line,26);
  _items[count]->setWidth(20);
  count++;
  line += 2;

  // Miscellaneous settings

  _misc_settings.setColor(colors.getPair("header", "bg_normal"));
  addItem(&_misc_settings);  
  _items[count]->setName("Miscellaneous settings");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  addItem(&_warninval_toggle);
  _items[count]->setName("Display a warning for invalid package names");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
}

OptionsWindow::~OptionsWindow()
{
  unsigned int i, nitems;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->itemType() == "Label")
    {
      if ( (_items[i]->name() != "User interface settings") && 
           (_items[i]->name() != "Color settings") &&
           (_items[i]->name() != "Package manager settings") &&
           (_items[i]->name() != "Miscellaneous settings") )
        delete _items[i];
    }
  }
}

/*******************************************************************************

Read and apply settings

*******************************************************************************/
void OptionsWindow::readSettings()
{
  _resolve_toggle.setEnabled(resolve_deps);
  _rebuild_toggle.setEnabled(rebuild_inv_deps);
  _confirm_toggle.setEnabled(confirm_changes);
  _buildopts_toggle.setEnabled(save_buildopts);
  _layout_box.setChoice(layout);

  _color_toggle.setEnabled(enable_color);
  _color_box.setChoice(color_theme);

  _pmgr_box.setChoice(package_manager);
  _repo_inp.setText(repo_dir);
  _tag_inp.setText(repo_tag);
  _sync_inp.setText(sync_cmd);
  _inst_inp.setText(install_cmd);
  _upgr_inp.setText(upgrade_cmd);
  _reinst_inp.setText(reinstall_cmd);
  _iclos_inp.setText(install_clos);
  _ivars_inp.setText(install_vars);
  _uclos_inp.setText(upgrade_clos);
  _uvars_inp.setText(upgrade_vars);

  _warninval_toggle.setEnabled(warn_invalid_pkgnames);
}

void OptionsWindow::applySettings(int & check_color, int & check_write) const
{
  check_color = 0;
  check_write = 0;

  resolve_deps = _resolve_toggle.enabled();
  rebuild_inv_deps = _rebuild_toggle.enabled();
  confirm_changes = _confirm_toggle.enabled();
  save_buildopts = _buildopts_toggle.enabled();
  layout = _layout_box.choice();

  color_theme = _color_box.choice();
  if (_color_toggle.enabled()) { check_color = activate_color(color_theme); }
  else { deactivate_color(); }

  package_manager = _pmgr_box.choice();
  repo_dir = _repo_inp.text();
  repo_tag = _tag_inp.text();
  sync_cmd = _sync_inp.text();
  install_cmd = _inst_inp.text();
  upgrade_cmd = _upgr_inp.text(); 
  reinstall_cmd = _reinst_inp.text();
  install_clos = _iclos_inp.text();
  install_vars = _ivars_inp.text();
  upgrade_clos = _uclos_inp.text();
  upgrade_vars = _uvars_inp.text();

  warn_invalid_pkgnames = _warninval_toggle.enabled();

  // Write configuration to ~/.sboui.conf 

  check_write = write_config();
}

/*******************************************************************************

Sizes and places window. Also sets width of text input boxes.

*******************************************************************************/
void OptionsWindow::placeWindow()
{
  int rows, cols;
  unsigned int i, nitems;

  getmaxyx(stdscr, rows, cols);
  mvwin(_win, 2, 0);
  wresize(_win, rows-4, cols);

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->itemType() == "TextInput")
      _items[i]->setWidth(cols-1-_items[i]->posx());
  }
}

/*******************************************************************************

User interaction

*******************************************************************************/
std::string OptionsWindow::exec(MouseEvent * mevent)
{
  bool getting_input;
  int y_offset, check_redraw;
  std::string selection, retval, prev_pkg_mgr;
  
  prev_pkg_mgr = _pmgr_box.choice();

  getting_input = true;
  while (getting_input)
  {
    // Draw input box elements
  
    draw();
    _redraw_type = "changed";
    y_offset = _firstprint - _header_rows;

    // Get user input from highlighted item

    prev_pkg_mgr = _pmgr_box.choice();
    selection = _items[_highlight]->exec(y_offset);
    if (selection == signals::resize)
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if ( (selection == signals::quit) ||
              (selection == signals::keyEnter) ||
              (selection == signals::keySpace) )
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }
    else if (selection == signals::highlightFirst)
    { 
      if (highlightFirst() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightLast) 
    { 
      if (highlightLast() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightPrevPage)
    {
      if (highlightPreviousPage() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightNextPage)
    {
      if (highlightNextPage() == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightPrev)
    { 
      if (_highlight == _first_selectable)
        check_redraw = highlightFirst();
      else { check_redraw = highlightPrevious(); }
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else if (selection == signals::highlightNext)
    {
      if (_highlight == _last_selectable)
        check_redraw = highlightLast();
      else { check_redraw = highlightNext(); }
      if (check_redraw == 1) { _redraw_type = "all"; }
      else { _redraw_type = "changed"; }
    }
    else
    {
      retval = selection;
      _redraw_type = "all";
      getting_input = false;
    }

    // If package manager has changed, ask to set other defaults automatically

    if (_pmgr_box.choice() != prev_pkg_mgr)
      askSetDefaults(_pmgr_box.choice());
  }

  return retval;
}

/*******************************************************************************

Displays a dialog asking whether to set default repo_dir, sync_cmd, install_cmd,
and upgrade_cmd when package manager is changed. In case it was changed to
custom, just display an info message.

*******************************************************************************/
void OptionsWindow::askSetDefaults(const std::string & new_pkg_mgr)
{
  WINDOW *defwin;
  std::string selection, msg;
  DefaultOptionsBox defbox; 
  MessageBox msgbox;
  bool getting_input;

  defwin = newwin(1, 1, 0, 0);

  // Prompt to enter other inputs when setting custom package manager

  if (new_pkg_mgr == "custom")
  {
    msgbox.setWindow(defwin);
    msgbox.setName("Custom package manager");
    msg = "You have selected a custom package manager. Be sure to set "
        + std::string("repo_dir, sync_cmd, install_cmd, and upgrade_cmd ")
        + std::string("appropriately.");
    msgbox.setMessage(msg); 
    msgbox.setInfo("Enter: Dismiss");
    msgbox.setColor(colors.getPair("fg_popup", "bg_popup"));
    placePopup(&msgbox, defwin);
    draw(true);

    // Get user input

    getting_input = true;
    while (getting_input)
    {
      selection = msgbox.exec();
      getting_input = false;
      if (selection == signals::resize)
      {
        getting_input = true;
        placePopup(&msgbox, defwin);
        draw(true);
      }
    }
  }

  // Otherwise, prompt to automatically set defaults
  
  else
  {
    defbox.setWindow(defwin);
    defbox.setPackageManager(new_pkg_mgr);
    placePopup(&defbox, defwin); 
    draw(true);

    getting_input = true;
    while (getting_input)
    {
      selection = defbox.exec();
      if (selection == signals::keyEnter)
      {
        getting_input = false;
        if (defbox.setRepoDir())
          _repo_inp.setText(defbox.repoDir());
        if (defbox.setSyncCmd())
          _sync_inp.setText(defbox.syncCmd());
        if (defbox.setInstallCmd())
          _inst_inp.setText(defbox.installCmd());
        if (defbox.setUpgradeCmd())
          _upgr_inp.setText(defbox.upgradeCmd());
        if (defbox.setReinstallCmd())
          _reinst_inp.setText(defbox.reinstallCmd());
      }
      else if (selection == signals::quit) { getting_input = false; }
      else if (selection == signals::resize)
      {
        placePopup(&defbox, defwin);
        draw(true);
      }
    }
  }

  delwin(defwin);
  draw(true);
}
