#include <string>
#include <vector>
#include <curses.h>
#include "Color.h"

/*******************************************************************************

Translate string color (red, green, etc.) into ncurses color

*******************************************************************************/
int Color::ncursesColor(const std::string & colorname) const
{
  int ncurses_color;

  if (colorname == "black") { ncurses_color = COLOR_BLACK; }
  else if (colorname == "red") { ncurses_color = COLOR_RED; }
  else if (colorname == "green") { ncurses_color = COLOR_GREEN; }
  else if (colorname == "yellow") { ncurses_color = COLOR_YELLOW; }
  else if (colorname == "blue") { ncurses_color = COLOR_BLUE; }
  else if (colorname == "magenta") { ncurses_color = COLOR_MAGENTA; }
  else if (colorname == "cyan") { ncurses_color = COLOR_CYAN; }
  else if (colorname == "white") { ncurses_color = COLOR_WHITE; }
  else { ncurses_color = -1; }

  return ncurses_color;
}

/*******************************************************************************

Constructor

*******************************************************************************/
Color::Color() { _colors.resize(0); }

/*******************************************************************************

Create color pair by foreground & background colors

*******************************************************************************/
void Color::addPair(const std::string & fg, const std::string & bg)
{
  color_entry color;
  unsigned int i, npairs;
  int fg_color, bg_color;

  // If terminal does not support colors, do not add it

  if (! has_colors()) { return; }

  // First check if color already exists; don't add it if it does

  npairs = _colors.size();
  for ( i = 0; i < npairs; i++ )
  {
    if ( (_colors[i].fg == fg) && (_colors[i].bg == bg) ) { return; }
  }
       
  // Create a color entry and next COLOR_PAIR

  npairs++;
  color.fg = fg;
  color.bg = bg;
  color.pair = npairs;
  _colors.push_back(color);

  fg_color = ncursesColor(fg);
  bg_color = ncursesColor(bg);
  start_color();
  init_pair(color.pair, fg_color, bg_color);
}

/*******************************************************************************

Access color pair by foreground & background colors. Returns -1 if color pair
has not been created.

*******************************************************************************/
int Color::pair(const std::string & fg, const std::string & bg) const
{
  unsigned int i, npairs;
  int pair;

  npairs = _colors.size();
  pair = -1;
  for ( i = 0; i < npairs; i++ )
  {
    if ( (_colors[i].fg == fg) && (_colors[i].bg == bg) ) 
    { 
      pair = _colors[i].pair;
      break;
    }
  }

  return pair;
}
