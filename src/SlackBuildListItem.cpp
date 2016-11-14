#include <string>
#include "ListItem.h"
#include "SlackBuildListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
SlackBuildListItem::SlackBuildListItem() 
{ 
  _name = ""; 
  _category = "";
  _installed_version = "";
  _available_version = "";
  _tagged = false;
  _installed = false;
}

/*******************************************************************************

Set properties

*******************************************************************************/
void SlackBuildListItem::setCategory(const std::string & category) 
{ 
  _category = category;
}

void SlackBuildListItem::setInstalledVersion(
                                          const std::string & installed_version)
{ 
  _installed_version = installed_version;
}

void SlackBuildListItem::setAvailableVersion(
                                          const std::string & available_version)
{ 
  _available_version = available_version;
}

void SlackBuildListItem::setTagged(bool tagged) { _tagged = tagged; }
void SlackBuildListItem::setInstalled(bool installed) 
{ 
  _installed = installed; 
}

/*******************************************************************************

Get properties

*******************************************************************************/
const std::string & SlackBuildListItem::category() const { return _category; }
const std::string & SlackBuildListItem::installedVersion() const
{ 
  return _installed_version;
}

const std::string & SlackBuildListItem::availableVersion() const
{ 
  return _available_version;
}
bool SlackBuildListItem::tagged() const { return _tagged; }
bool SlackBuildListItem::installed() const { return _installed; }
