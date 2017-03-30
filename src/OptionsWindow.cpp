#include <string>
#include <curses.h>
#include <cmath>      // floor
#include <algorithm>  // min, max
#include "Color.h"
#include "settings.h"
#include "TextInput.h"
#include "ToggleInput.h"
#include "Label.h"
#include "OptionsWindow.h"

using namespace settings;
using namespace color_settings;

/*******************************************************************************

Draws border and title

*******************************************************************************/
void OptionsWindow::redrawFrame() const
{
  int rows, cols, namelen, i, left, right;
  double mid;
  unsigned int nitems;

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

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }

  // Symbols on right border to indicate scrolling

  nitems = _items.size();
  if (_firstprint != _header_lines) { mvwaddch(_win, _header_lines, cols-1,
                                               ACS_UARROW); }
  if (_items[nitems-1]->posy() > _firstprint + rows-_reserved_rows - 1)
    mvwaddch(_win, rows-2, cols-1, ACS_DARROW);
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
OptionsWindow::OptionsWindow()
{
  int count, line;

  _reserved_rows = 2;
  _header_lines = 1;
  _firstprint = _header_lines;
  _msg = "Options";

  count = 0;
  line = 1; 

  // Label for basic settings

  _basic.setColor(colors.getPair(header, bg_normal));
  addItem(&_basic);
  _items[count]->setName("Basic settings");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  // Toggle inputs

  addItem(&_resolve_toggle);
  _items[count]->setName("Resolve dependencies");
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

  addItem(&_color_toggle);
  _items[count]->setName("Enable color");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  // Label + ComboBox for layout

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
  line += 1;

  // Labels + text input boxes for basic settings

  addItem(new Label());
  _items[count]->setName("Editor");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(24);
  count++;
  line += 0;

  addItem(&_editor_inp);
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

  // Advanced settings

  _advanced.setColor(colors.getPair(header, bg_normal));
  addItem(&_advanced);  
  _items[count]->setName("Advanced settings");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 2;

  // Label + ComboBox for layout

  addItem(new Label());
  _items[count]->setName("Package manager");
  _items[count]->setPosition(line,1);
  _items[count]->setWidth(_items[count]->name().size());
  count++;
  line += 0;

  _pmgr_box.setParent(this);
  _pmgr_box.addChoice("sbopkg");
  _pmgr_box.addChoice("sbotools");
  _pmgr_box.addChoice("custom");
  addItem(& _pmgr_box);
  _items[count]->setPosition(line,26);
  count++;
  line += 1;
  
  // Labels + text input boxes for advanced settings

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
}

OptionsWindow::~OptionsWindow()
{
  unsigned int i, nitems;

  nitems = _items.size();
  for ( i = 0; i < nitems; i++ )
  {
    if (_items[i]->itemType() == "Label")
    {
      if ( (_items[i]->name() != "Basic settings") && 
           (_items[i]->name() != "Advanced settings") )
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
  _confirm_toggle.setEnabled(confirm_changes);
  _color_toggle.setEnabled(enable_color);

  _layout_box.setChoice(layout);

  _editor_inp.setText(editor);
  _iclos_inp.setText(install_clos);
  _ivars_inp.setText(install_vars);
  _uclos_inp.setText(upgrade_clos);
  _uvars_inp.setText(upgrade_vars);

  _pmgr_box.setChoice(package_manager);

  _repo_inp.setText(repo_dir);
  _sync_inp.setText(sync_cmd);
  _inst_inp.setText(install_cmd);
  _upgr_inp.setText(upgrade_cmd);
}

void OptionsWindow::applySettings() const
{
  resolve_deps = _resolve_toggle.getBoolProp();
  confirm_changes = _confirm_toggle.getBoolProp();
  if (_color_toggle.getBoolProp()) { activate_color(); }
  else { deactivate_color(); }

  layout = _layout_box.getStringProp();

  editor = _editor_inp.getStringProp();
  install_clos = _iclos_inp.getStringProp();
  install_vars = _ivars_inp.getStringProp();
  upgrade_clos = _uclos_inp.getStringProp();
  upgrade_vars = _uvars_inp.getStringProp();

  package_manager = _pmgr_box.getStringProp();

  repo_dir = _repo_inp.getStringProp();
  sync_cmd = _sync_inp.getStringProp();
  install_cmd = _inst_inp.getStringProp();
  upgrade_cmd = _upgr_inp.getStringProp(); 
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
