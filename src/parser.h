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

