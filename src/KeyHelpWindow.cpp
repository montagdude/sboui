#include <string>
#include "HelpItem.h"
#include "KeyHelpWindow.h"

/*******************************************************************************

Constructor

*******************************************************************************/
KeyHelpWindow::KeyHelpWindow() { createList(); }

/*******************************************************************************

Constructs list to display

*******************************************************************************/
void KeyHelpWindow::createList()
{
  addItem(new HelpItem("Main window", "", true, false));
  addItem(new HelpItem("Activate left/top list", "Left arrow"));
  addItem(new HelpItem("Activate menubar", "F9"));
  addItem(new HelpItem("Activate right/bottom list", "Right arrow"));
  addItem(new HelpItem("Filter", "f"));
  addItem(new HelpItem("Show keyboard shortcuts", "?"));
  addItem(new HelpItem("Options", "o"));
  addItem(new HelpItem("Quick search in active list", "Ctrl-s"));
  addItem(new HelpItem("Quit", "q"));
  addItem(new HelpItem("Search", "/"));
  addItem(new HelpItem("Switch active list", "Tab"));
  addItem(new HelpItem("Sync / Update repository", "s"));
  addItem(new HelpItem("Toggle vertical/horizontal layout", "l"));
  addItem(new HelpItem("Upgrade all", "Ctrl-u"));
  addItem(new HelpItem("View command line output", "c"));

  addItem(new HelpItem("space1", "", false, true));

  addItem(new HelpItem("Lists & input boxes", "", true, false));
  addItem(new HelpItem("Go back / Cancel", "Esc"));
  addItem(new HelpItem("Jump to beginning", "Home"));
  addItem(new HelpItem("Jump to end", "End"));
  addItem(new HelpItem("Next input field", "Tab"));
  addItem(new HelpItem("Previous input field", "Shift+Tab"));
  addItem(new HelpItem("Pull down combo box", "Space"));
  addItem(new HelpItem("Scroll down", "Down arrow"));
  addItem(new HelpItem("Scroll up", "Up arrow"));
  addItem(new HelpItem("Scroll down a page", "Page down"));
  addItem(new HelpItem("Scroll up a page", "Page up"));
  addItem(new HelpItem("Select highlighted / Accept selection", "Enter"));
  addItem(new HelpItem("Toggle check box", "Space"));

  addItem(new HelpItem("space2", "", false, true));

  addItem(new HelpItem("Tagging", "", true, false));
  addItem(new HelpItem("Install tagged", "i"));
  addItem(new HelpItem("Reinstall tagged", "e"));
  addItem(new HelpItem("Remove tagged", "r"));
  addItem(new HelpItem("Tag selected", "t"));
  addItem(new HelpItem("Upgrade tagged", "u"));
   
  // Determine width needed to display shortcut

  shortcutWidth();
}
