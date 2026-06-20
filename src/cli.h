#pragma once

// Onwer: Ch4120N
// Project: ChComboParser

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <thread>

struct Config {
    std::string inputFile;
    std::string outputFile;
    std::string symbol       = ":";
    int         index        = 1;       // 0 = username, 1 = password
    int         threads      = 0;       // 0 = auto-detect
    bool        deduplicate  = true;
    bool        sortOutput   = false;
    bool        countOnly    = false;
    bool        verbose      = false;
    bool        quiet        = false;
    bool        trimSpaces   = true;
    bool        skipEmpty    = true;
    bool        lowercase    = false;
    bool        uppercase    = false;
    size_t      minLength    = 0;
    size_t      maxLength    = 0;       // 0 = no limit
    bool        showProgress = true;
};

inline void printBanner() {
    std::cout << R"(
  __  _  _  __  __  __  __  ___   __  ___  __   ___   ___  ___  ___  
 / _)( )( )/ _)/  \(  \/  )(  ,) /  \(  ,\(  ) (  ,) / __)(  _)(  ,) 
( (_  )__(( (_( () ))    (  ) ,\( () )) _//__\  )  \ \__ \ ) _) )  \ 
 \__)(_)(_)\__)\__/(_/\/\_)(___/ \__/(_) (_)(_)(_)\_)(___/(___)(_)\_)
                      v1.0.0 | Owned By Ch4120N
)" << std::endl;
}

inline void printUsage(const char* prog) {
    printBanner();
    std::cout << "USAGE: " << prog
              << " <INPUT_FILE> <OUTPUT_FILE> <SYMBOL> <INDEX> [OPTIONS]\n\n"
              << "See for more information & source code:\n"
              << "  GitHub.com/Ch4120N/ChComboParser\n\n"
              << "REQUIRED:\n"
              << "  INPUT_FILE              Path to input combo file\n"
              << "  OUTPUT_FILE             Path to output file (use '-' for stdout)\n"
              << "  SYMBOL                  Delimiter symbol (e.g., ':', '@', '|')\n"
              << "  INDEX                   Field index to extract (0-based)\n"
              << "                            0 = first field (usually username)\n"
              << "                            1 = second field (usually password)\n\n"
              << "OPTIONS:\n"
              << "  -t, --threads <N>       Number of worker threads (default: auto)\n"
              << "  -d, --no-dedup          Disable duplicate removal\n"
              << "  -s, --sort              Sort output alphabetically\n"
              << "  -c, --count             Count only, don't write output file\n"
              << "  -m, --min-length <N>    Minimum output string length\n"
              << "  -M, --max-length <N>    Maximum output string length\n"
              << "  --lowercase             Convert output to lowercase\n"
              << "  --uppercase             Convert output to uppercase\n"
              << "  --no-trim               Don't trim whitespace from fields\n"
              << "  --keep-empty            Keep empty extracted fields\n"
              << "  -v, --verbose           Verbose output with statistics\n"
              << "  -q, --quiet             Suppress all non-essential output\n"
              << "  --no-progress           Disable progress bar\n"
              << "  -h, --help              Show this help message\n\n"
              << "EXAMPLES:\n"
              << "  " << prog << " combos.txt passwords.txt : 1\n"
              << "  " << prog << " combos.txt usernames.txt : 0\n"
              << "  " << prog << " combos.txt pass.txt @ 1 -t 8 -s\n"
              << "  " << prog << " combos.txt pass.txt : 1 -m 8 -M 64 --sort\n"
              << "  " << prog << " combos.txt - : 1 -c -v\n"
              << std::endl;
}
