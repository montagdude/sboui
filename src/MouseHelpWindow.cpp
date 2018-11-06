#include <vector>
#include <string>
#include "HelpItem.h"
#include "MouseHelpWindow.h"

/*******************************************************************************

Constructor

*******************************************************************************/
MouseHelpWindow::MouseHelpWindow()
{
  _name = "Mouse bindings";
  setLabels("Action", "Binding");
}

/*******************************************************************************

Constructs list to display

*******************************************************************************/
void MouseHelpWindow::createList()
{
  addItem(new HelpItem("Accept selection", "Double left click"));
  addItem(new HelpItem("Change selection", "Left click"));
  addItem(new HelpItem("Scrolling", "Click arrows / scroll area"));
#if NCURSES_MOUSE_VERSION > 1
  addItem(new HelpItem("Scrolling", "Mouse wheel"));
#endif
  addItem(new HelpItem("Tag SlackBuild or category", "Right click"));
   
  // Determine width needed to display shortcuts

  shortcutWidth();
}
