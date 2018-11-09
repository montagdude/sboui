#pragma once

#include <string>
#include <vector>

/*******************************************************************************

Class to read and process command-line arguments

*******************************************************************************/
class CLOParser {

  private:

    std::vector<std::string> _argv_str;
    std::string _input_file;
    bool _sync, _upgrade_all, _upgradable;


    /* Converts CLOs to vector of strings */

    void readCLOs(int argc, char *argv[]);

  public:

    /* Constructor */

    CLOParser();

    /* Checks CLOs for errors */

    int checkCLOs(int argc, char *argv[], const std::string & version);

    /* Prints various information messages */

    void printUsage() const;
    void printVersion(const std::string & version) const;
    void printHelp() const;

    /* Custom input file information */

    bool requestInputFile() const;
    const std::string & inputFile() const;

    /* Query other possible inputs */

    bool sync() const;
    bool upgradeAll() const;
    bool upgradable() const;
};
