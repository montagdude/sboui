#include <string>
#include <vector>
#include <iostream>
#include "string_util.h"
#include "CLOParser.h"

/*******************************************************************************

Reads command line arguments and saves as vector of strings

*******************************************************************************/
void CLOParser::readCLOs(int argc, char *argv[])
{
  int i;

  _argv_str.resize(0);
  for ( i = 0; i < argc; i++ )
  {
    _argv_str.push_back(chararray_to_string(argv[i]));
  }
}

/*******************************************************************************

Constructor

*******************************************************************************/
CLOParser::CLOParser()
{
  _argv_str.resize(0);
  _input_file = "";
}

/*******************************************************************************

Checks CLOs for errors. Returns 0 on successful read, 1 if there are errors, or
-1 if the user asked to print help or version info.

*******************************************************************************/
int CLOParser::checkCLOs(int argc, char *argv[], const std::string & version)
{
  int i;

  // Save CLOs as vector of strings

  readCLOs(argc, argv);

  // Check for errors

  i = 1;
  while (i < argc)
  {
    if ( (_argv_str[i] == "-f") || (_argv_str[i] == "--file") )
    {
      if (i < argc-1)
      {
        _input_file = _argv_str[i+1];
        i += 2;
      }
      else
      {
        std::cerr << "Error: must specify a file with " << _argv_str[i]
                  << " argument." << std::endl;
        printUsage();
        return 1;
      }
    }
    else if (_argv_str[i] == "--help")
    {
      printHelp();
      return -1;
    }
    else if (_argv_str[i] == "--version")
    {
      printVersion(version);
      return -1;
    }
    else
    {
      printUsage();
      return 1;
    }
  }

  return 0;
}
   
/*******************************************************************************

Prints various information messages

*******************************************************************************/
void CLOParser::printUsage() const
{
  std::string exename = _argv_str[0];
  std::cout << "Usage: " << exename << " [OPTION]" << std::endl;
  std::cout << "Try '" << exename << " --help' for more information."
            << std::endl;
}

void CLOParser::printVersion(const std::string & version) const
{
  std::string exename = _argv_str[0];
  std::cout << exename << " " << version << std::endl;
}

void CLOParser::printHelp() const
{
  std::string exename = _argv_str[0];
  std::cout << "Usage: " << exename << " [OPTION]" << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -f, --file FILE  Specify a custom configuration file"
            << std::endl;
  std::cout << "  --help           Display usage information and exit"
            << std::endl;
  std::cout << "  --version        Display version number of sboui and exit"
            << std::endl;
  std::cout << std::endl;
  std::cout << "See `man sboui` and `man sboui.cfg` for more information."
            << std::endl;
}

/*******************************************************************************

Custom input file information

*******************************************************************************/
bool CLOParser::requestInputFile() const
{
  if (_input_file == "") { return false; }
  else { return true; }
}

const std::string & CLOParser::inputFile() const { return _input_file; }
