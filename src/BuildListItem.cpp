#include <string>
#include "ListItem.h"
#include "BuildListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
BuildListItem::BuildListItem() 
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
void BuildListItem::setCategory(const std::string & category) 
{ 
  _category = category;
}

void BuildListItem::setInstalledVersion(const std::string & installed_version)
{ 
  _installed_version = installed_version;
}

void BuildListItem::setAvailableVersion(const std::string & available_version)
{ 
  _available_version = available_version;
}

void BuildListItem::setTagged(bool tagged) { _tagged = tagged; }
void BuildListItem::setInstalled(bool installed) 
{ 
  _installed = installed; 
}

/*******************************************************************************

Get properties

*******************************************************************************/
const std::string & BuildListItem::category() const { return _category; }
const std::string & BuildListItem::installedVersion() const
{ 
  return _installed_version;
}

const std::string & BuildListItem::availableVersion() const
{ 
  return _available_version;
}
bool BuildListItem::tagged() const { return _tagged; }
bool BuildListItem::installed() const { return _installed; }
