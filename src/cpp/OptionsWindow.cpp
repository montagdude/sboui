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

using namespace backend;
using namespace color;

/*******************************************************************************

Draws border and title

*******************************************************************************/
void OptionsWindow::redrawFrame() const
{
  unsigned int rows, cols, namelen, i;
  int left, right;
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

  // Right border

  for ( i = 1; i < rows-1; i++ ) { mvwaddch(_win, i, cols-1, ACS_VLINE); }

  // Bottom border

  wmove(_win, rows-1, 1);
  for ( i = 1; i < cols-1; i++ ) { waddch(_win, ACS_HLINE); }
}

/*******************************************************************************

Constructor and destructor

*******************************************************************************/
OptionsWindow::OptionsWindow()
{
  _reserved_rows = 2;
  _msg = "Options";

  // Label for basic settings

  _basic.setColor(colors.getPair(header, bg_normal));
  addItem(&_basic);
  _items[0]->setName("Basic settings");
  _items[0]->setPosition(1,1);
  _items[0]->setWidth(_items[0]->name().size());

  // Toggle inputs

  addItem(&_resolve_toggle);
  _items[1]->setName("Resolve dependencies");
  _items[1]->setPosition(3,1);
  _items[1]->setWidth(_items[1]->name().size());

  addItem(&_confirm_toggle);
  _items[2]->setName("Ask for confirmation before applying changes");
  _items[2]->setPosition(4,1);
  _items[2]->setWidth(_items[2]->name().size());

  // Labels for basic settings

  addItem(new Label());
  _items[3]->setName("Editor");
  _items[3]->setPosition(6,1);
  _items[3]->setWidth(24);

  addItem(new Label());
  _items[4]->setName("Additional install CLOs");
  _items[4]->setPosition(8,1);
  _items[4]->setWidth(24);

  addItem(new Label());
  _items[5]->setName("Install environment vars");
  _items[5]->setPosition(9,1);
  _items[5]->setWidth(24);

  addItem(new Label());
  _items[6]->setName("Additional upgrade CLOs");
  _items[6]->setPosition(10,1);
  _items[6]->setWidth(24);

  addItem(new Label());
  _items[7]->setName("Upgrade environment vars");
  _items[7]->setPosition(11,1);
  _items[7]->setWidth(24);

  // Text input boxes for basic settings (note: widths get changed by 
  // placeWindow)

  addItem(&_editor_inp);
  _items[8]->setPosition(6,26);
  _items[8]->setWidth(20);

  addItem(&_iopts_inp);
  _items[9]->setPosition(8,26);
  _items[9]->setWidth(20);

  addItem(&_ivars_inp);
  _items[10]->setPosition(9,26);
  _items[10]->setWidth(20);

  addItem(&_uopts_inp);
  _items[11]->setPosition(10,26);
  _items[11]->setWidth(20);

  addItem(&_uvars_inp);
  _items[12]->setPosition(11,26);
  _items[12]->setWidth(20);

  // Advanced settings

  _advanced.setColor(colors.getPair(header, bg_normal));
  addItem(&_advanced);  
  _items[13]->setName("Advanced settings");
  _items[13]->setPosition(13,1);
  _items[13]->setWidth(_items[13]->name().size());
  
  // Labels for advanced settings

  addItem(new Label());
  _items[14]->setName("Package manager");
  _items[14]->setPosition(15,1);
  _items[14]->setWidth(20);

  addItem(new Label());
  _items[15]->setName("Repository directory");
  _items[15]->setPosition(16,1);
  _items[15]->setWidth(20);

  addItem(new Label());
  _items[16]->setName("Sync command");
  _items[16]->setPosition(17,1);
  _items[16]->setWidth(20);

  addItem(new Label());
  _items[17]->setName("Install command");
  _items[17]->setPosition(18,1);
  _items[17]->setWidth(20);

  addItem(new Label());
  _items[18]->setName("Upgrade command");
  _items[18]->setPosition(19,1);
  _items[18]->setWidth(20);

  // Text input boxes for advanced settings (note: widths get changed by 
  // placeWindow)

  addItem(&_repo_inp);
  _items[19]->setPosition(16,26);
  _items[19]->setWidth(20);

  addItem(&_sync_inp);
  _items[20]->setPosition(17,26);
  _items[20]->setWidth(20);

  addItem(&_inst_inp);
  _items[21]->setPosition(18,26);
  _items[21]->setWidth(20);

  addItem(&_upgr_inp);
  _items[22]->setPosition(19,26);
  _items[22]->setWidth(20);
}

OptionsWindow::~OptionsWindow()
{
  unsigned int i;
  
  for ( i = 3; i < 8; i++ ) { delete _items[i]; }
  for ( i = 14; i < 19; i++ ) { delete _items[i]; }
}

/*******************************************************************************

Read and apply settings

*******************************************************************************/
void OptionsWindow::readSettings()
{
  _resolve_toggle.setEnabled(resolve_deps);
  _confirm_toggle.setEnabled(confirm_changes);

  _editor_inp.setText(editor);
  _iopts_inp.setText(install_opts);
  _ivars_inp.setText(install_vars);
  _uopts_inp.setText(upgrade_opts);
  _uvars_inp.setText(upgrade_vars);

  _repo_inp.setText(repo_dir);
  _sync_inp.setText(sync_cmd);
  _inst_inp.setText(install_cmd);
  _upgr_inp.setText(upgrade_cmd);
}

void OptionsWindow::applySettings() const
{
  resolve_deps = _resolve_toggle.getBoolProp();
  confirm_changes = _confirm_toggle.getBoolProp();

  editor = _editor_inp.getStringProp();
  install_opts = _iopts_inp.getStringProp();
  install_vars = _ivars_inp.getStringProp();
  upgrade_opts = _uopts_inp.getStringProp();
  upgrade_vars = _uvars_inp.getStringProp();

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
  unsigned int i;

  getmaxyx(stdscr, rows, cols);
  mvwin(_win, 2, 0);
  wresize(_win, rows-4, cols);

  for ( i = 3; i < 13; i++ )
  {
    _items[i]->setWidth(cols-1-_items[i]->posx());
  }

  for ( i = 19; i < 23; i++ )
  {
    _items[i]->setWidth(cols-1-_items[i]->posx());
  }
}
