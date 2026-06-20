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
