#include <string>
#include <vector>
#include <curses.h>
#include <cmath>     // floor
#include <algorithm> // max
#include "Color.h"
#include "settings.h"
#include "string_util.h"
#include "PackageInfoBox.h"

/*******************************************************************************

Prints message in box within defined margins

*******************************************************************************/
void PackageInfoBox::redrawMessage() const
{
  unsigned int i, max_lines, len;
  int rows, cols, left;
  double mid;
  std::vector<std::string> splitmsg;  

  getmaxyx(_win, rows, cols);
  splitmsg = split(_message, '\n');

  max_lines = splitmsg.size();
  for ( i = 0; i < max_lines; i++ )
  {
    len = splitmsg[i].size();
    mid = double(cols-2)/2.0;
    left = std::floor(mid - double(len)/2.0) + 1;
    wmove(_win, 3+_margin_v+i, 1);
    if (_centered) { printSpaces(left-1); }
    wprintw(_win, splitmsg[i].c_str());
  }
}

/*******************************************************************************

Constructors

*******************************************************************************/
PackageInfoBox::PackageInfoBox()
{
  int color_pair;

  _name = "Package info";
  _info = "Enter: Dismiss";
  _centered = false;
  color_pair = colors.getPair("fg_popup", "bg_popup");
  if (color_pair != -1) { setColor(color_pair); }
}

PackageInfoBox::PackageInfoBox(WINDOW *win)
{
  int color_pair;

  _win = win;
  _name = "Package info";
  _info = "Enter: Dismiss";
  _centered = false;
  color_pair = colors.getPair("fg_popup", "bg_popup");
  if (color_pair != -1) { setColor(color_pair); }
}

/*******************************************************************************

Get attributes

*******************************************************************************/
void PackageInfoBox::minimumSize(int & height, int & width) const
{
  // Always use the preferred size, or else the displayed info gets squished.
  // The user will just have to make their window wider if it's too small.
  
  preferredSize(height, width);
}

void PackageInfoBox::preferredSize(int & height, int & width) const
{
  int reserved_rows, reserved_cols, msg_width;
  std::vector<std::string> splitmsg;
  unsigned int i, nlines;

  reserved_rows = 6 + 2*_margin_v;
  reserved_cols = 2;

  // Preferred width -- pick some reasonable number (message width 50)

  width = std::max(_name.size(), _info.size()) + reserved_cols;
  width = std::max(width, int(30+2+2*_margin_h));
  msg_width = width-2-2*_margin_h;

  // Required height based on width and message

  splitmsg = split(_message, '\n');
  nlines = splitmsg.size(); 
  height = reserved_rows + nlines;

  // Check width and adjust height accordingly

  for ( i = 0; i < nlines; i++ )
  {
    if (int(splitmsg[i].size()) > msg_width)
      msg_width = splitmsg[i].size(); 
  }
  width = msg_width+2+2*_margin_h;
  splitmsg = split(_message, '\n');
  nlines = splitmsg.size(); 
  height = reserved_rows + nlines;
}
