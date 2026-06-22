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
                      v1.0.1 | Owned By Ch4120N
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

inline Config parseArgs(int argc, char* argv[]) {
    Config cfg;

    if (argc < 2) {
        printUsage(argv[0]);
        std::exit(1);
    }

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    int positional = 0;
    for (size_t i = 0; i < args.size(); ++i) {
        const auto& a = args[i];

        if (a == "-h" || a == "--help") {
            printUsage(argv[0]);
            std::exit(0);
        }
        else if (a == "-t" || a == "--threads") {
            if (++i >= args.size()) { std::cerr << "Error: --threads requires a value\n"; std::exit(1); }
            cfg.threads = std::atoi(args[i].c_str());
        }
        else if (a == "-d" || a == "--no-dedup") {
            cfg.deduplicate = false;
        }
        else if (a == "-s" || a == "--sort") {
            cfg.sortOutput = true;
        }
        else if (a == "-c" || a == "--count") {
            cfg.countOnly = true;
        }
        else if (a == "-m" || a == "--min-length") {
            if (++i >= args.size()) { std::cerr << "Error: --min-length requires a value\n"; std::exit(1); }
            cfg.minLength = std::stoull(args[i]);
        }
        else if (a == "-M" || a == "--max-length") {
            if (++i >= args.size()) { std::cerr << "Error: --max-length requires a value\n"; std::exit(1); }
            cfg.maxLength = std::stoull(args[i]);
        }
        else if (a == "--lowercase") {
            cfg.lowercase = true;
        }
        else if (a == "--uppercase") {
            cfg.uppercase = true;
        }
        else if (a == "--no-trim") {
            cfg.trimSpaces = false;
        }
        else if (a == "--keep-empty") {
            cfg.skipEmpty = false;
        }
        else if (a == "-v" || a == "--verbose") {
            cfg.verbose = true;
        }
        else if (a == "-q" || a == "--quiet") {
            cfg.quiet = true;
            cfg.showProgress = false;
        }
        else if (a == "--no-progress") {
            cfg.showProgress = false;
        }
        else if (a[0] == '-') {
            std::cerr << "Error: Unknown option '" << a << "'\n";
            std::exit(1);
        }
        else {
            switch (positional) {
                case 0: cfg.inputFile  = a; break;
                case 1: cfg.outputFile = a; break;
                case 2: cfg.symbol     = a; break;
                case 3: cfg.index      = std::atoi(a.c_str()); break;
                default:
                    std::cerr << "Error: Too many positional arguments\n";
                    std::exit(1);
            }
            ++positional;
        }
    }

    if (positional < 4) {
        std::cerr << "Error: Missing required arguments.\n\n";
        printUsage(argv[0]);
        std::exit(1);
    }

    if (cfg.threads <= 0) {
        cfg.threads = static_cast<int>(std::thread::hardware_concurrency());
        if (cfg.threads <= 0) cfg.threads = 4;
    }

    return cfg;
}