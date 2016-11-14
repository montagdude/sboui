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
