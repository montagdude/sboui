#pragma once

#include <string>
#include "TextInput.h"
#include "ToggleInput.h"
#include "InputBox.h"

/*******************************************************************************

Dialog to apply default options when the user changes the package manager

*******************************************************************************/
class DefaultOptionsBox: public InputBox {

  private:

    std::string _pkg_mgr;
    std::string _repo_dir, _sync_cmd, _install_cmd, _upgrade_cmd,
                _reinstall_cmd;
    ToggleInput _repoitem, _syncitem, _installitem, _upgradeitem,
                _reinstallitem;

  public:

    /* Constructor */

    DefaultOptionsBox();

    /* Set attributes */

    int setPackageManager(const std::string & pkg_mgr);

    /* Get attributes */

    bool setRepoDir() const;
    bool setSyncCmd() const;
    bool setInstallCmd() const;
    bool setUpgradeCmd() const;
    bool setReinstallCmd() const;

    const std::string & repoDir() const;
    const std::string & syncCmd() const;
    const std::string & installCmd() const;
    const std::string & upgradeCmd() const;
    const std::string & reinstallCmd() const;
};
