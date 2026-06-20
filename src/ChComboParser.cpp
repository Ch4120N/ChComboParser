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

int main(int argc, char* argv[]) {
    Config cfg = parseArgs(argc, argv);

    if (!cfg.quiet) printBanner();

    if (!fs::exists(cfg.inputFile)) {
        std::cerr << "Error: Input file does not exist: " << cfg.inputFile << "\n";
        return 1;
    }

    size_t fileSize = fs::file_size(cfg.inputFile);
    if (fileSize == 0) {
        std::cerr << "Error: Input file is empty.\n";
        return 1;
    }

    if (!cfg.quiet) {
        std::cout << "  Input File  : " << cfg.inputFile << "\n"
                  << "  Output File : " << (cfg.countOnly ? "(count only)" : cfg.outputFile) << "\n"
                  << "  Symbol      : '" << cfg.symbol << "'\n"
                  << "  Index       : " << cfg.index << "\n"
                  << "  Threads     : " << cfg.threads << "\n"
                  << "  Deduplicate : " << (cfg.deduplicate ? "Yes" : "No") << "\n"
                  << "  File Size   : " << formatBytes(fileSize) << "\n\n";
    }

    // Reading the input file
    auto t0 = std::chrono::high_resolution_clock::now();

    if (!cfg.quiet) std::cout << "  [1/4] Reading input file...\n";
    std::string fileContent = readFile(cfg.inputFile);

    // Split the fileContent into chunks
    size_t numChunks = static_cast<size_t>(cfg.threads) * 4; // oversubscribe for balance
    std::vector<Chunk> chunks = splitIntoChunks(fileContent, numChunks);

    if (!cfg.quiet) std::cout << "  [2/4] Processing with " << cfg.threads
                              << " threads (" << chunks.size() << " chunks)...\n";
    
    ParseStats stats;
    std::atomic<size_t> bytesProcessed{0};
    ProgressBar progress(fileSize, cfg.showProgress && !cfg.quiet);

    // Each thread gets its own results vector
    std::vector<std::vector<std::string>> threadResults(chunks.size());

    {
        ThreadPool pool(cfg.threads);
        std::vector<std::future<void>> futures;

        for (size_t i = 0; i < chunks.size(); ++i) {
            futures.push_back(pool.enqueue([&, i] {
                processChunk(
                    chunks[i],
                    cfg.symbol,
                    cfg.index,
                    cfg.trimSpaces,
                    cfg.skipEmpty,
                    cfg.lowercase,
                    cfg.uppercase,
                    cfg.minLength,
                    cfg.maxLength,
                    stats,
                    threadResults[i],
                    bytesProcessed,
                    progress
                );
            }));
        }

        for (auto& f : futures) f.get();
    }

    progress.finish();

    if (!cfg.quiet) std::cout << "  [3/4] Merging results";
    if (cfg.deduplicate) std::cout << " & removing duplicates";
    std::cout << "...\n";

    std::vector<std::string> finalResults;

    if (cfg.deduplicate) {
        // Estimate total for reserve
        size_t total = 0;
        for (auto& v : threadResults) total += v.size();
        finalResults.reserve(total);

        std::unordered_set<std::string> seen;
        seen.reserve(total);

        for (auto& v : threadResults) {
            for (auto& s : v) {
                if (seen.insert(s).second) {
                    finalResults.push_back(std::move(s));
                }
            }
        }
        stats.duplicatesRemoved = total - finalResults.size();
    } else {
        for (auto& v : threadResults) {
            finalResults.insert(finalResults.end(),
                                std::make_move_iterator(v.begin()),
                                std::make_move_iterator(v.end()));
        }
    }

    if (cfg.sortOutput) {
        std::sort(finalResults.begin(), finalResults.end());
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(t1 - t0).count();

    if (!cfg.countOnly) {
        if (!cfg.quiet) std::cout << "  [4/4] Writing output to " << cfg.outputFile << "...\n";

        if (cfg.outputFile == "-") {
            for (const auto& line : finalResults) {
                std::cout << line << "\n";
            }
        } else {
            std::ofstream outFile(cfg.outputFile, std::ios::binary);
            if (!outFile.is_open()) {
                std::cerr << "Error: Cannot open output file: " << cfg.outputFile << "\n";
                return 1;
            }
            for (const auto& line : finalResults) {
                outFile << line << "\n";
            }
        }
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    double totalElapsed = std::chrono::duration<double>(t2 - t0).count();

    if (cfg.verbose || !cfg.quiet) {
        std::cout << "\n"
                  << "  +--------------------------------------+\n"
                  << "  |           FINAL STATISTICS           |\n"
                  << "  +--------------------------------------+\n"
                  << "  |  Total Lines Processed : " << std::setw(12) << stats.totalLines.load()          << " |\n"
                  << "  |  Valid Extracted       : " << std::setw(12) << stats.validExtracted.load()      << " |\n"
                  << "  |  Unique Results        : " << std::setw(12) << finalResults.size()               << " |\n"
                  << "  |  Duplicates Removed    : " << std::setw(12) << stats.duplicatesRemoved.load()   << " |\n"
                  << "  |  Skipped (no symbol)   : " << std::setw(12) << stats.skippedNoSymbol.load()     << " |\n"
                  << "  |  Skipped (index OOB)   : " << std::setw(12) << stats.skippedIndexOOB.load()     << " |\n"
                  << "  |  Skipped (empty)       : " << std::setw(12) << stats.skippedEmpty.load()        << " |\n"
                  << "  |  Skipped (length)      : " << std::setw(12) << stats.skippedLength.load()       << " |\n"
                  << "  +--------------------------------------+\n"
                  << "  |  Processing Time       : " << std::setw(12) << formatDuration(elapsed)            << " |\n"
                  << "  |  Total Time (w/ I/O)   : " << std::setw(12) << formatDuration(totalElapsed)       << " |\n"
                  << "  |  Throughput            : " << std::setw(12) << (formatBytes(static_cast<size_t>(fileSize / elapsed)) + "/s") << " |\n"
                  << "  +--------------------------------------+\n";
    }

    if (!cfg.quiet) {
        // Replaced the Unicode checkmark (✓) with [OK] for Windows compatibility
        std::cout << "\n  [OK] Done! " << finalResults.size()
                  << " unique entries in " << formatDuration(totalElapsed) << "\n\n";
    }

    return 0;
}