#include <string>
#include "ListItem.h"

/*******************************************************************************

Constructor

*******************************************************************************/
ListItem::ListItem() { _name = ""; }

/*******************************************************************************

Set/get properties

*******************************************************************************/
void ListItem::setName(const std::string & name) { _name = name; }
const std::string & ListItem::name() const { return _name; }
