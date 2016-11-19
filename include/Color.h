#pragma once

#include <string>
#include <vector>

struct color_entry {
  std::string fg, bg;
  int pair;
};

/*******************************************************************************

Manages colors for ncurses, making color pairs easy to reference by foreground
and background color.

*******************************************************************************/
class Color {

  private:

    std::vector<color_entry> _colors;

    /* Translate string color (red, green, etc.) into ncurses variable */

    int ncursesColor(const std::string & colorname) const;

  public:

    /* Constructors */

    Color();

    /* Create or access color pair by foreground & background colors */

    void addPair(const std::string & fg, const std::string & bg);
    int pair(const std::string & fg, const std::string & bg) const;
};
