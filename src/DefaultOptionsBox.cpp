#include <string>
#include <curses.h>
#include "Color.h"
#include "ToggleInput.h"
#include "DefaultOptionsBox.h"

/*******************************************************************************

Constructor

*******************************************************************************/
DefaultOptionsBox::DefaultOptionsBox()
{
  _firstprint = _header_rows;
  setPackageManager("sbopkg");

  addItem(&_repoitem);
  _repoitem.setName("repo_dir");
  _repoitem.setEnabled(true);
  _repoitem.setWidth(30);
  _repoitem.setPosition(3,1);

  addItem(&_syncitem);
  _syncitem.setName("sync_cmd");
  _syncitem.setEnabled(true);
  _syncitem.setWidth(30);
  _syncitem.setPosition(4,1);

  addItem(&_installitem);
  _installitem.setName("install_cmd");
  _installitem.setEnabled(true);
  _installitem.setWidth(30);
  _installitem.setPosition(5,1);

  addItem(&_upgradeitem);
  _upgradeitem.setName("upgrade_cmd");
  _upgradeitem.setEnabled(true);
  _upgradeitem.setWidth(30);
  _upgradeitem.setPosition(6,1);

  addItem(&_reinstallitem);
  _reinstallitem.setName("reinstall_cmd");
  _reinstallitem.setEnabled(true);
  _reinstallitem.setWidth(30);
  _reinstallitem.setPosition(7,1);
}

/*******************************************************************************

Set attributes

*******************************************************************************/
int DefaultOptionsBox::setPackageManager(const std::string & pkg_mgr)
{
  if (pkg_mgr == "sbopkg")
  {
    _repo_dir = "/var/lib/sbopkg/SBo/14.2";
    _sync_cmd = "sbopkg -r";
    _install_cmd = "sbopkg -B -i";
    _upgrade_cmd = "sbopkg -B -i";
    _reinstall_cmd = "sbopkg -B -i";
  }
  else if (pkg_mgr == "sbotools")
  {
//FIXME: Add -r to sbotools reinstall command if he fixes issue #72 on GitHub
    _repo_dir = "/usr/sbo/repo";
    _sync_cmd = "sbosnap update";
    _install_cmd = "sboinstall -r";
    _upgrade_cmd = "sboupgrade -r";
    _reinstall_cmd = "sboinstall --reinstall -r";
  }
  else if (pkg_mgr == "built-in")
  {
    _repo_dir = "/var/lib/sboui/repo";
    _sync_cmd = "sboui-backend update";
    _install_cmd = "sboui-backend install -f";
    _upgrade_cmd = "sboui-backend install -f";
    _reinstall_cmd = "sboui-backend install -f";
  }
  else if (pkg_mgr == "custom")
  {
    _repo_dir = "";
    _sync_cmd = "";
    _install_cmd = "";
    _upgrade_cmd = "";
    _reinstall_cmd = "";
  }
  else { return 1; }

  _pkg_mgr = pkg_mgr;
  _msg = "Apply defaults for " + _pkg_mgr + "?";

  return 0;
}
  
/*******************************************************************************

Get attributes

*******************************************************************************/
bool DefaultOptionsBox::setRepoDir() const { return _repoitem.enabled(); }
bool DefaultOptionsBox::setSyncCmd() const { return _syncitem.enabled(); }
bool DefaultOptionsBox::setInstallCmd() const { return _installitem.enabled(); }
bool DefaultOptionsBox::setUpgradeCmd() const { return _upgradeitem.enabled(); }
bool DefaultOptionsBox::setReinstallCmd() const
{
  return _reinstallitem.enabled();
}
const std::string & DefaultOptionsBox::repoDir() const { return _repo_dir; }
const std::string & DefaultOptionsBox::syncCmd() const { return _sync_cmd; }
const std::string & DefaultOptionsBox::installCmd() const
{
  return _install_cmd;
}
const std::string & DefaultOptionsBox::upgradeCmd() const
{
  return _upgrade_cmd;
}
const std::string & DefaultOptionsBox::reinstallCmd() const
{
  return _reinstall_cmd;
}
