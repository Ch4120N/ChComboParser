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

class ProgressBar {
private:
    size_t total_;
    bool enabled_;
    std::atomic<int> lastPercent_;
    std::mutex mutex_;
public:
    ProgressBar(size_t total, bool enabled)
        : total_(total), enabled_(enabled), lastPercent_(-1) {}

    void update(size_t current) {
        if (!enabled_ || total_ == 0) return;
        int pct = static_cast<int>((current * 100) / total_);
        if (pct > 100) pct = 100;
        if (pct == lastPercent_.load()) return;

        std::lock_guard<std::mutex> lock(mutex_);
        // Double-check after acquiring lock
        if (pct == lastPercent_.load()) return;
        lastPercent_ = pct;

        int barWidth = 40;
        int pos = barWidth * pct / 100;

        std::cerr << "\r  [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cerr << '#';
            else if (i == pos) std::cerr << '>';
            else std::cerr << '-';
        }
        std::cerr << "] " << pct << "%";
        std::cerr.flush();
    }

    void finish() {
        if (!enabled_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        std::cerr << "\r";
        int barWidth = 40;
        std::cerr << "  [";
        for (int i = 0; i < barWidth; ++i) std::cerr << '#';
        std::cerr << "] 100%\n";
    }
};

struct Chunk {
    const char* data;
    size_t      length;
};

inline std::vector<Chunk> splitIntoChunks(const std::string& fileContent, size_t numChunks) {
    std::vector<Chunk> chunks;
    if (fileContent.empty() || numChunks == 0) return chunks;

    size_t totalSize = fileContent.size();
    size_t chunkSize = totalSize / numChunks;
    if (chunkSize == 0) chunkSize = totalSize;

    const char* data = fileContent.data();
    size_t start = 0;

    for (size_t i = 0; i < numChunks && start < totalSize; ++i) {
        size_t end;
        if (i == numChunks - 1) {
            end = totalSize;
        } else {
            end = start + chunkSize;
            // Move end to next newline to avoid splitting a line
            while (end < totalSize && data[end] != '\n') ++end;
            if (end < totalSize) ++end; // include the newline
        }
        if (end > start) {
            chunks.push_back({data + start, end - start});
        }
        start = end;
    }
    return chunks;
}

inline void processLine(
    const char* lineStart,
    size_t lineLen,
    const std::string& symbol,
    int index,
    bool trimSpaces,
    bool skipEmpty,
    bool lowercase,
    bool uppercase,
    size_t minLength,
    size_t maxLength,
    ParseStats& stats,
    std::vector<std::string>& localResults)
{
    // Handle \r\n
    if (lineLen > 0 && lineStart[lineLen - 1] == '\r') --lineLen;
    
    stats.totalLines++;

    if (lineLen == 0) {
        if (skipEmpty) stats.skippedEmpty++;
        return;
    }

    std::string line(lineStart, lineLen);
    size_t symPos = line.find(symbol);

    if (symPos == std::string::npos) {
        stats.skippedNoSymbol++;
        return;
    }

    // Split by symbol — we only need the field at `index`
    int currentField = 0;
    size_t fieldStart = 0;
    bool found = false;
    std::string field;

    size_t searchPos = 0;
    while (true) {
        size_t nextSym = line.find(symbol, searchPos);
        if (currentField == index) {
            if (nextSym == std::string::npos) {
                field = line.substr(fieldStart);
            } else {
                field = line.substr(fieldStart, nextSym - fieldStart);
            }
            found = true;
            break;
        }
        if (nextSym == std::string::npos) break;
        fieldStart = nextSym + symbol.size();
        searchPos = fieldStart;
        currentField++;
    }

    if (!found) {
        stats.skippedIndexOOB++;
        return;
    }

    if (trimSpaces) field = trim(field);

    if (skipEmpty && field.empty()) {
        stats.skippedEmpty++;
        return;
    }

    // Length filter
    if (minLength > 0 && field.size() < minLength) {
        stats.skippedLength++;
        return;
    }
    if (maxLength > 0 && field.size() > maxLength) {
        stats.skippedLength++;
        return;
    }

    if (lowercase) toLowerInPlace(field);
    if (uppercase) toUpperInPlace(field);

    localResults.push_back(std::move(field));
    stats.validExtracted++;
}

// Core of the Parser

inline void processChunk(
    const Chunk& chunk,
    const std::string& symbol,
    int index,
    bool trimSpaces,
    bool skipEmpty,
    bool lowercase,
    bool uppercase,
    size_t minLength,
    size_t maxLength,
    ParseStats& stats,
    std::vector<std::string>& localResults,
    std::atomic<size_t>& bytesProcessed,
    ProgressBar& progress)
{
    const char* p   = chunk.data;
    const char* end = chunk.data + chunk.length;
    const char* lineStart = p;

    // Process all complete lines (ending with \n)
    while (p < end) {
        if (*p == '\n') {
            size_t lineLen = p - lineStart;
            processLine(lineStart, lineLen, symbol, index, trimSpaces, skipEmpty,
                       lowercase, uppercase, minLength, maxLength, stats, localResults);
            bytesProcessed += lineLen + 1; // +1 for the \n
            progress.update(bytesProcessed.load());
            lineStart = p + 1;
        }
        ++p;
    }

    // Handle the last line if it doesn't end with a newline
    if (lineStart < end) {
        size_t lineLen = end - lineStart;
        processLine(lineStart, lineLen, symbol, index, trimSpaces, skipEmpty,
                   lowercase, uppercase, minLength, maxLength, stats, localResults);
        bytesProcessed += lineLen; // No \n at the end
        progress.update(bytesProcessed.load());
    }
}