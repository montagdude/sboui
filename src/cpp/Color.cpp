#include <vector>
#include <string> 
#include <curses.h>
#include "Color.h"

/*******************************************************************************

Gets base color name (red, green, etc.) and determines whether color is
"bright"

*******************************************************************************/
void Color::baseColorName(const std::string & colorname, std::string & basename,
                          bool & bright) const
{
  size_t len;

  len = colorname.size();

  if ( (len < 6) || (colorname.substr(0,6) != "bright") )
  {
    bright = false;
    basename = colorname;
  }
  else
  {
    bright = true;
    basename = colorname.substr(6,len-6);
  }
}

/*******************************************************************************

Translate string color (red, green, etc.) into ncurses color.

*******************************************************************************/
int Color::ncursesColor(const std::string & basename) const
{
  int ncurses_color;

  if (basename == "black") { ncurses_color = COLOR_BLACK; }
  else if (basename == "red") { ncurses_color = COLOR_RED; }
  else if (basename == "green") { ncurses_color = COLOR_GREEN; }
  else if (basename == "yellow") { ncurses_color = COLOR_YELLOW; }
  else if (basename == "blue") { ncurses_color = COLOR_BLUE; }
  else if (basename == "magenta") { ncurses_color = COLOR_MAGENTA; }
  else if (basename == "cyan") { ncurses_color = COLOR_CYAN; }
  else if (basename == "white") { ncurses_color = COLOR_WHITE; }
  else { ncurses_color = -1; }

  return ncurses_color;
}

/*******************************************************************************

Constructor

*******************************************************************************/
Color::Color()
{ 
  _colors.resize(0); 
  _active_pair = -1;
}

/*******************************************************************************

Create color pair by foreground & background colors

*******************************************************************************/
void Color::addPair(const std::string & fg, const std::string & bg)
{
  color_entry fg_entry, bg_entry;
  color_pair pair;
  unsigned int i, npairs;
  bool bright;
  std::string basename;
  int fg_color, bg_color;

  // If terminal does not support colors, do not add it

  if (! has_colors()) { return; }

  // First check if color already exists; don't add it if it does

  npairs = _colors.size();
  for ( i = 0; i < npairs; i++ )
  {
    if ( (_colors[i].fg.name == fg) && (_colors[i].bg.name == bg) ) { return; }
  }
       
  // Create a color entry and next COLOR_PAIR

  npairs++;

  baseColorName(fg, basename, bright); 
  pair.fg.name = fg;
  pair.fg.basename = basename;
  pair.fg.type = "fg";
  pair.fg.bright = bright;

  baseColorName(bg, basename, bright); 
  pair.bg.name = bg;
  pair.bg.basename = basename;
  pair.bg.type = "bg";
  pair.bg.bright = bright;

  pair.idx = npairs;

  fg_color = ncursesColor(pair.fg.basename);
  bg_color = ncursesColor(pair.bg.basename);

  // Allow "bright" colors by setting additional attributes. (Credit to mutt
  // source code for how this is done.)

  pair.use_bold = false;
  pair.use_blink = false;
  if (pair.fg.bright) { pair.use_bold = true; }
  if (pair.bg.bright)
  {
    if (COLORS < 16) { pair.use_blink = true; }
    else { bg_color += 8; }
  }
  _colors.push_back(pair);

  init_pair(pair.idx, fg_color, bg_color);
}

/*******************************************************************************

Get color pair from foreground and background names. Returns index in the
_colors vector or -1 if not found.

*******************************************************************************/
int Color::getPair(const std::string & fg, const std::string & bg) const
{
  int i, npairs, vec_idx;

  npairs = _colors.size();
  vec_idx = -1;
  for ( i = 0; i < npairs; i++ )
  {
    if ( (_colors[i].fg.name == fg) && (_colors[i].bg.name == bg) ) 
    { 
      vec_idx = i;
      break;
    }
  }

  return vec_idx;
}

/*******************************************************************************

Turn color on or off

*******************************************************************************/
int Color::turnOn(WINDOW *win, const std::string & fg, const std::string & bg)
{
  color_pair mypair;

  // Get color pair

  _active_pair = getPair(fg, bg);
  if (_active_pair == -1) { return 1; }
  mypair = _colors[_active_pair];

  // Turn on attributes

  wattron(win, COLOR_PAIR(mypair.idx));
  if (mypair.use_bold) { wattron(win, A_BOLD); }
  if (mypair.use_blink) { wattron(win, A_BLINK); }

  return 0;
}

int Color::turnOn(WINDOW *win, int vec_idx)
{
  color_pair mypair;

  if ( (vec_idx == -1) || (vec_idx >= int(_colors.size())) ) { return 1; }
  _active_pair = vec_idx;
  mypair = _colors[_active_pair];

  // Turn on attributes

  wattron(win, COLOR_PAIR(mypair.idx));
  if (mypair.use_bold) { wattron(win, A_BOLD); }
  if (mypair.use_blink) { wattron(win, A_BLINK); }

  return 0;
}

int Color::turnOff(WINDOW *win)
{
  color_pair mypair;

  if (_active_pair == -1) { return 1; }
  mypair = _colors[_active_pair];

  // Turn off attributes

  wattroff(win, COLOR_PAIR(mypair.idx));
  if (mypair.use_bold) { wattroff(win, A_BOLD); }
  if (mypair.use_blink) { wattroff(win, A_BLINK); }

  return 0;
}

int Color::setBackground(WINDOW *win, const std::string & fg,
                         const std::string & bg) const
{
  color_pair mypair;
  int vec_idx;

  // Get color pair

  vec_idx = getPair(fg, bg);
  if (vec_idx == -1) { return 1; }
  mypair = _colors[vec_idx];

  // Set background color

  wbkgd(win, COLOR_PAIR(mypair.idx));

  return 0;
}
