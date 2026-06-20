# ChComboParser v1.0.0

**Owner & Lead Developer:** [Ch4120N](https://github.com/Ch4120N)  
⚡ **High-Performance Multi-Threaded Combo List Parser**

`ChComboParser` is a blazing-fast, multi-threaded C++ utility designed to parse, extract, filter, and clean credential combination lists (combos). Whether you need to extract usernames, passwords, or specific fields from massive datasets, this tool handles it efficiently with minimal memory overhead.

---

## ✨ Features

- 🚀 **Multi-Threaded Processing:** Automatically detects CPU cores and splits files into optimized chunks for parallel processing.
- 🧹 **Smart Deduplication:** Hash-set based duplicate removal to ensure clean output lists.
- 📊 **Detailed Statistics:** Real-time progress bar and comprehensive post-run statistics (lines processed, skipped, throughput).
- 📏 **Advanced Filtering:** Filter results by minimum/maximum string length, skip empty fields, and trim whitespace.
- 🔤 **String Transformations:** Convert extracted fields to lowercase or uppercase on the fly.
- 🔀 **Sorting & Counting:** Sort output alphabetically or run in "count-only" mode to analyze lists without writing to disk.
- 🛠️ **Flexible Delimiters:** Works with any delimiter (`:`, `@`, `|`, `;`, etc.) and supports any field index.

---

## 🎯 Made For (Target Audience)

`ChComboParser` was specifically crafted by **Ch4120N** to support the cybersecurity community. It is an essential utility for:

- 🛡️ **Penetration Testers & Red Teamers:** Rapidly cleaning, deduplicating, and formatting combo lists for authorized password spraying and credential stuffing assessments.
- 🔍 **Security Researchers & Threat Intel Analysts:** Processing massive leaked datasets to extract specific fields (e.g., isolating passwords to study cracking patterns or extracting emails for OSINT).
- 🏢 **Blue Teamers & System Administrators:** Auditing internal user directories against known breach databases to quickly identify and remediate compromised accounts.
- 🐛 **Bug Bounty Hunters:** Efficiently filtering and preparing wordlists to test for weak or default credentials within authorized scopes.
- 🎓 **CTF Players & Cybersecurity Students:** Parsing and manipulating credential formats for Capture The Flag competitions and educational labs.

---

## 🛠️ Build Instructions

### Prerequisites
- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake (version 3.14 or higher)

### Compilation

```bash
# Clone the repository
git clone https://github.com/Ch4120N/ChComboParser.git
cd ChComboParser

# Create build directory and configure
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compile
make -j$(nproc)

# (Optional) Install to system path
sudo make install
```

---

## 📖 Usage

### Syntax
```bash
ChComboParser <INPUT_FILE> <OUTPUT_FILE> <SYMBOL> <INDEX> [OPTIONS]
```

### Quick Examples

**1. Extract Passwords (Index 1) using `:` delimiter:**
```bash
./ChComboParser combos.txt passwords.txt : 1
```

**2. Extract Usernames (Index 0) using `:` delimiter:**
```bash
./ChComboParser combos.txt usernames.txt : 0
```

**3. Process with 8 threads, sort output, and remove duplicates:**
```bash
./ChComboParser combos.txt clean_pass.txt : 1 -t 8 -s
```

**4. Filter passwords between 8 and 64 characters:**
```bash
./ChComboParser combos.txt filtered.txt : 1 -m 8 -M 64
```

**5. Count unique passwords without saving to a file (Verbose mode):**
```bash
./ChComboParser combos.txt - : 1 -c -v
```

**6. Output directly to standard output (stdout):**
```bash
./ChComboParser combos.txt - : 1 | grep "admin"
```

---

## ⚙️ CLI Options

| Flag | Description | Default |
| :--- | :--- | :--- |
| `-t, --threads <N>` | Number of worker threads | Auto-detect |
| `-d, --no-dedup` | Disable duplicate removal | Enabled |
| `-s, --sort` | Sort output alphabetically | Disabled |
| `-c, --count` | Count only, don't write output file | Disabled |
| `-m, --min-length <N>` | Minimum output string length | 0 (No limit) |
| `-M, --max-length <N>` | Maximum output string length | 0 (No limit) |
| `--lowercase` | Convert output to lowercase | Disabled |
| `--uppercase` | Convert output to uppercase | Disabled |
| `--no-trim` | Don't trim whitespace from fields | Trims spaces |
| `--keep-empty` | Keep empty extracted fields | Skips empty |
| `-v, --verbose` | Show detailed statistics | Disabled |
| `-q, --quiet` | Suppress all non-essential output | Disabled |
| `--no-progress` | Disable the progress bar | Enabled |
| `-h, --help` | Show the help message | - |

---

## 🏗️ Architecture

`ChComboParser` is designed for maximum throughput on large files (GBs in size):
1. **Memory Mapping / Bulk Read:** Reads the entire file into memory (or streams in chunks for massive files).
2. **Chunk Splitting:** Divides the file into `Threads * 4` chunks, ensuring splits only happen at newline boundaries to prevent corrupted lines.
3. **Parallel Parsing:** Each thread parses its chunk independently, applying filters and transformations, and stores results in a thread-local vector (zero lock contention).
4. **Merge & Dedup:** The main thread merges the local vectors and uses an `unordered_set` for O(1) duplicate checking.
5. **Output:** Writes the final cleaned list to the specified file or stdout.

---

## ⚠️ Disclaimer

This tool is developed by **Ch4120N** for educational purposes and cybersecurity learning. It is intended to help security researchers, penetration testers, and system administrators analyze and clean credential datasets. 

*Do not use this tool for malicious purposes or unauthorized access. Always ensure you have explicit permission before testing or analyzing credentials.*

---

## 📜 License

Distributed under the **GPL v3** License. See `LICENSE` for more information.

**Developed with ❤️ by Ch4120N**