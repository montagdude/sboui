#include <string>
#include <iostream>
#include <string>
#include <vector>
#include "string_util.h"
#include "curses.h"
#include "settings.h"
#include "MainWindow.h"

void print_usage(const std::string & exename)
{
  std::cout << exename << ": invalid command line options. Usage:" << std::endl;
  std::cout << exename << " [-f, --file FILE]" << std::endl;
}

int main(int argc, char *argv[])
{
  int i, check;
  std::vector<std::string> argv_str;
  std::string conf_file = "";

  // Process command line arguments

  for ( i = 0; i < argc; i++ )
  {
    argv_str.push_back(chararray_to_string(argv[i]));
  }

  if ( (argc == 2) || (argc > 3) )
  {
    print_usage(argv_str[0]);
    return 1;
  }
  else if (argc == 3)
  {
    if ( (argv_str[1] == "-f") || (argv_str[1] == "--file") )
      conf_file = argv_str[2];
    else
    {
      print_usage(argv_str[0]);
      return 1;
    }
  }

  // Read config files

  if (conf_file == "") { check = read_config(); }
  else { check = read_config(conf_file); }
  if (check != 0) { return check; }

  // Set up ncurses

  initscr();
  curs_set(0);
  raw();
  noecho();
  set_escdelay(25);
  keypad(stdscr, TRUE);
  if (settings::enable_color) { activate_color(); }
  else { deactivate_color(); }

  // User interaction loop

  MainWindow mainwindow;  // Call constructor after setting colors
  mainwindow.initialize();
  mainwindow.show();

  endwin();
  return 0;
}
