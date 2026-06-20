// -*- coding: UTF-8 -*-
/*
 ____  _____  _    _  ____  ____  ____  ____     ____  _  _     ___  _   _  __  __  ___   ___  _  _ 
(  _ \(  _  )( \/\/ )( ___)(  _ \( ___)(  _ \   (  _ \( \/ )   / __)( )_( )/. |/  )(__ \ / _ \( \( )
 )___/ )(_)(  )    (  )__)  )   / )__)  )(_) )   ) _ < \  /   ( (__  ) _ ((_  _))(  / _/( (_) ))  ( 
(__)  (_____)(__/\__)(____)(_)\_)(____)(____/   (____/ (__)    \___)(_) (_) (_)(__)(____)\___/(_)\_)

OWNER: Ch4120N
Project: ChComboParser
*/

#include "cli.h"
#include "threadpool.h"
#include "parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>


namespace fs = std::filesystem;

static std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open input file: " << path << "\n";
        std::exit(1);
    }
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string content(size, '\0');
    file.read(content.data(), size);
    return content;
}

static std::string formatBytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    while (size >= 1024.0 && unit < 3) { size /= 1024.0; unit++; }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}

static std::string formatDuration(double seconds) {
    if (seconds < 1.0) return std::to_string(static_cast<int>(seconds * 1000)) + " ms";
    if (seconds < 60.0) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << seconds << " s";
        return oss.str();
    }
    int mins = static_cast<int>(seconds) / 60;
    double secs = seconds - mins * 60;
    std::ostringstream oss;
    oss << mins << "m " << std::fixed << std::setprecision(1) << secs << "s";
    return oss.str();
}

