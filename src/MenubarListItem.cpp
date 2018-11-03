#include <string>
#include "ListItem.h"
#include "MenubarListItem.h"

/*******************************************************************************

Constructors

*******************************************************************************/
MenubarListItem::MenubarListItem() 
{ 
  _name = ""; 
  addProp("shortcut", "");
}
MenubarListItem::MenubarListItem(const std::string & name,
                                 const std::string & shortcut, int hotkey)
{ 
  _name = name; 
  addProp("shortcut", shortcut);
  setHotKey(hotkey);
}
