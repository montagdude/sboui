#include <string>
#include "ListItem.h"
#include "CategoryListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
CategoryListItem::CategoryListItem() 
{ 
  _name = ""; 
  _tagged = false;
}

/*******************************************************************************

Set/get properties

*******************************************************************************/
void CategoryListItem::setTagged(bool tagged) { _tagged = tagged; }
bool CategoryListItem::tagged() const { return _tagged; }

/*******************************************************************************

The following are included to satisfy the compiler, but they do nothing for
CategoryListItems

*******************************************************************************/
void CategoryListItem::setInstalled(bool installed) {}
bool CategoryListItem::installed() const { return false; }
