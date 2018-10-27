#include <string>
#include "ListItem.h"
#include "CategoryListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
CategoryListItem::CategoryListItem() 
{ 
  _name = ""; 
  addBoolProp("tagged", false);
  addProp("category", "");
}
