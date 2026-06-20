#pragma once

// Onwer: Ch4120N
// Project: ChComboParser

#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <chrono>

struct ParseStats {
    std::atomic<size_t> totalLines{0};
    std::atomic<size_t> validExtracted{0};
    std::atomic<size_t> skippedEmpty{0};
    std::atomic<size_t> skippedLength{0};
    std::atomic<size_t> skippedNoSymbol{0};
    std::atomic<size_t> skippedIndexOOB{0};
    std::atomic<size_t> duplicatesRemoved{0};
};

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

inline void toLowerInPlace(std::string& s) {
    for (auto& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

inline void toUpperInPlace(std::string& s) {
    for (auto& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

