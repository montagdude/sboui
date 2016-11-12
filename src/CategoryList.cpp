#include <vector>
#include <string>
#include "ListBox.h"
#include "CategoryList.h"

/*******************************************************************************

Constructors

*******************************************************************************/
CategoryList::CategoryList()
{
  _win = NULL;
  _buildlistwin = NULL;
  _name = "";
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _buildlists.resize(0);
}

CategoryList::CategoryList(WINDOW *win, WINDOW *buildlistwin,
                           const std::string & name)
{
  _win = win;
  _buildlistwin = buildlistwin;
  _name = name;
  _items.resize(0);
  _highlight = 0;
  _firstprint = 0;
  _prevhighlight = 0;
  _activated = true;
  _buildlists.resize(0);
}

/*******************************************************************************

Set attributes

*******************************************************************************/
void CategoryList::setBuildListWindow(WINDOW *buildlistwin)
{ 
  _buildlistwin = buildlistwin;
}

void CategoryList::addCategory(const std::string & name,
                               const std::vector<std::string> & builds)
{
  unsigned int i, nbuilds;
  ListBox buildlist;

  addItem(name);
  nbuilds = builds.size();
  buildlist.setWindow(_buildlistwin);
  buildlist.setName("SlackBuilds");
  buildlist.setActivated(false);
  for ( i = 0; i < nbuilds; i++ ) { buildlist.addItem(builds[i]); }
  _buildlists.push_back(buildlist);
}
   
/*******************************************************************************

User interaction loop

*******************************************************************************/
std::string CategoryList::exec()
{
  int ch, check_redraw;
  bool getting_input, getting_input_right;
  std::string redraw_type, retval, selection;

  const int MY_ESC = 27;

  // Don't bother if there are no items

  if (_items.size() == 0) { return "EMPTY"; }

  getting_input = true;
  redraw_type = "all";

  // Highlight first entry on first display

  if (_highlight == 0) { highlightFirst(); }

  // Handle key input events

  while (getting_input)
  {
    // Redraw menu elements as needed

    if (redraw_type == "all")
    {
      wclear(_win);
      redrawFrame();
    }
    if ( (redraw_type == "all") || (redraw_type == "items") ) { 
                                                              redrawAllItems(); }
    else if (redraw_type == "changed") { redrawChangedItems(); }
    wrefresh(_win);

    // The SlackBuilds list normally changes, so we'll just always redraw it

    _buildlists[_highlight].draw();

    if (! getting_input) { break; }

    // Get user input

    switch (ch = getch()) {

      // Enter key: return name of highlighted item

      case '\n':
      case '\r':
      case KEY_ENTER:
        getting_input = false;
        retval = _items[_highlight].name; 
        redraw_type = "none";
        break;

      // Right key: activate ListBox for highlighted category

      case KEY_RIGHT:
        redraw_type = "all";
        setActivated(false);
        draw();
        _buildlists[_highlight].setActivated(true);
        getting_input_right = true;

        // Enter user input loop for SlackBuild list

        while (getting_input_right)
        {
          selection = _buildlists[_highlight].exec();

          // Left key: switch back to category list

          if (selection == ListBox::keyLeftSignal)
          {
            _buildlists[_highlight].setActivated(false);
            _buildlists[_highlight].draw();
            getting_input_right = false;
            setActivated(true);
            redraw_type = "changed";
          }

          // Other inputs: pass back to caller for action

          else
          {
            getting_input = false;
            retval = selection;
          }
        }
        break;

      // Arrows/Home/End/PgUp/Dn: change highlighted value
      // FIXME: should check for redrawing just the arrows

      case KEY_UP:
        check_redraw = highlightPrevious();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_DOWN:
        check_redraw = highlightNext();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_PPAGE:
        check_redraw = highlightPreviousPage();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_NPAGE:
        check_redraw = highlightNextPage();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_HOME:
        check_redraw = highlightFirst();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;
      case KEY_END:
        check_redraw = highlightLast();
        if (check_redraw == 1) { redraw_type = "all"; }
        else { redraw_type = "changed"; }
        break;

      // Resize signal: redraw (may not work with some curses implementations)

      case KEY_RESIZE:
        getting_input = false;
        retval = resizeSignal;
        break;

      // Toggle item tag

      case 't':
        getting_input = false;
        retval = tagSignal;
        toggleItemTag(_highlight);
        check_redraw = highlightNext();
        break;

      // Quit key

      case MY_ESC:
        getting_input = false;
        retval = quitSignal;
        redraw_type = "none";
        break;

      default:
        redraw_type = "none";
        break;
    }
  }
  return retval;
}
