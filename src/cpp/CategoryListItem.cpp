#include <string>
#include "ListItem.h"
#include "CategoryListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
CategoryListItem::CategoryListItem() 
{ 
  _name = ""; 
  addProp("category", "");
  addBoolProp("tagged", false);
}
