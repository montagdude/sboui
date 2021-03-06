#pragma once

#include <string>
#include <fstream>

/*******************************************************************************

Reads shell variables in a file

*******************************************************************************/
class ShellReader {

  private:

    std::ifstream _file;
    bool _file_open;

    // Check if variable name is found

    bool checkVarname(std::string & line, const std::string & varname) const;

    // Reads value of variable from file

    int readVariable(std::string & line, std::string & value);
    int readDefaultVariable(std::string & line, std::string & value);

  public:

    /* Constructor and destructor */

    ShellReader();
    ~ShellReader();

    /* Opens or closes a file */

    int open(const std::string & filename);
    int close();

    /* Reads a variable from the file and stores as string */

    int read(const std::string & varname, std::string & value,
             bool default_var=false);

    /* Rewinds to beginning of the file */

    int rewind();
};
