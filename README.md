<p align="center">
  <img src="https://img.shields.io/badge/Version-1.0.0-blue?style=for-the-badge" alt="Version">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17">
  <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey?style=for-the-badge" alt="Platform">
  <img src="https://img.shields.io/badge/License-MIT-green?style=for-the-badge" alt="License">
  <img src="https://img.shields.io/badge/Author-Ch4120N-red?style=for-the-badge" alt="Author">
</p>

<h1 align="center">⚡ ChComboParser</h1>
<p align="center">
  <b>The Ultimate High-Performance, Multi-Threaded Combo List Parser & Credential Extractor</b><br>
  <i>Blazing fast C++17 utility for parsing, cleaning, deduplicating, and filtering user:pass combolists.</i>
</p>

<p align="center">
  <a href="#-quick-start">Quick Start</a> •
  <a href="#-usage--examples">Usage</a> •
  <a href="#-installation">Installation</a> •
  <a href="#-features">Features</a> •
  <a href="https://github.com/Ch4120N/ChComboParser/releases">Download Binaries</a>
</p>

---

## 📝 Overview

**ChComboParser** is an enterprise-grade, multi-threaded command-line tool written in modern C++17. Designed specifically for the cybersecurity community, it rapidly parses massive credential combination lists (combos). Whether you need to **extract passwords from a combo list**, **split usernames and passwords**, **remove duplicate credentials**, or **filter wordlists by length**, ChComboParser handles gigabytes of data in seconds with minimal memory overhead.

Developed by **[Ch4120N](https://github.com/Ch4120N)**, this tool eliminates the need for slow, single-threaded Python or Bash scripts when dealing with million-line credential dumps.

---

## 🎯 Who is ChComboParser For?

This tool is an essential part of the offensive and defensive security toolkit, specifically built for:

- 🛡️ **Penetration Testers & Red Teamers:** Rapidly clean, format, and deduplicate combo lists for authorized password spraying and credential stuffing assessments.
- 🔍 **OSINT & Threat Intel Analysts:** Process massive leaked databases to extract specific fields (e.g., isolating passwords to study cracking patterns or extracting emails/domains).
- 🏢 **Blue Teamers & SysAdmins:** Audit internal Active Directory user directories against known breach databases to quickly identify compromised accounts.
- 🐛 **Bug Bounty Hunters:** Efficiently filter and prepare custom wordlists to test for weak, default, or reused credentials within authorized scopes.
- 🎓 **CTF Players:** Parse and manipulate credential formats quickly during time-sensitive Capture The Flag competitions.

---

## ✨ Key Features

- 🚀 **Blazing Fast Multi-Threading:** Automatically detects CPU cores and splits files into optimized memory chunks for parallel processing. Processes millions of lines in seconds.
- 🧹 **Smart Deduplication:** High-speed hash-set based duplicate removal to ensure your final wordlists and combo lists are 100% clean.
- 📏 **Advanced Length Filtering:** Easily filter out weak passwords by setting minimum/maximum string length constraints (e.g., keep only 8-64 char passwords).
- 🔤 **On-the-Fly Transformations:** Convert extracted fields to lowercase or uppercase, and automatically trim whitespace without needing secondary tools like `tr` or `sed`.
- 🔀 **Flexible Delimiters & Indexing:** Works with any separator (`:`, `@`, `|`, `;`, `,`) and allows extraction from any column index (e.g., extracting the 3rd field from `email:pass:recovery`).
- 📊 **Real-Time Analytics:** Beautiful CLI progress bar and comprehensive post-run statistics (lines processed, skipped, throughput speed).
- 💾 **Zero-Lock Architecture:** Thread-local storage ensures zero mutex contention during the parsing phase, maximizing CPU utilization.

---

## 🚀 Quick Start

### Download Pre-compiled Binaries (No installation required)
Grab the latest release for your OS from the **[Releases Page](https://github.com/Ch4120N/ChComboParser/releases)**.

```bash
# Windows (Command Prompt / PowerShell)
ChComboParser-Windows-x64.exe combos.txt passwords.txt : 1

# Linux / macOS (Terminal)
chmod +x ChComboParser-Linux-x64
./ChComboParser-Linux-x64 combos.txt passwords.txt : 1
```

---

## 🛠️ Installation & Building from Source

If you prefer to compile from source, ChComboParser is 100% cross-platform.

### Prerequisites
- **C++17 Compiler:** GCC 9+, Clang 9+, or MSVC 2019+
- **CMake:** Version 3.14 or higher

### 🐧 Linux & 🍎 macOS
```bash
git clone https://github.com/Ch4120N/ChComboParser.git
cd ChComboParser
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)  # Use $(sysctl -n hw.ncpu) on macOS
```

### 🪟 Windows (Visual Studio)
Open the **x64 Native Tools Command Prompt**:
```cmd
git clone https://github.com/Ch4120N/ChComboParser.git
cd ChComboParser
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

---

## 📖 Usage & Command Examples

### Basic Syntax
```bash
ChComboParser <INPUT_FILE> <OUTPUT_FILE> <SYMBOL> <INDEX> [OPTIONS]
```

### Common Workflows

**1. Extract Passwords (Standard `user:pass` format):**
Extracts the second field (index 1) separated by a colon.
```bash
./ChComboParser combos.txt passwords.txt : 1
```

**2. Extract Usernames / Emails:**
Extracts the first field (index 0).
```bash
./ChComboParser combos.txt usernames.txt : 0
```

**3. Clean and Deduplicate a Massive Wordlist:**
Sorts the output and removes all duplicates automatically.
```bash
./ChComboParser dirty_list.txt clean_list.txt : 1 -s
```

**4. Filter by Password Length (e.g., Corporate Policy Compliance):**
Extracts passwords that are strictly between 8 and 64 characters long.
```bash
./ChComboParser combos.txt strong_pass.txt : 1 -m 8 -M 64
```

**5. Parse Complex Formats (e.g., `email|password|security_answer`):**
Extract the security answer (index 2) using a pipe `|` delimiter.
```bash
./ChComboParser complex_dump.txt answers.txt "|" 2
```

**6. Count Unique Entries Without Saving to Disk:**
Runs in memory-only mode and outputs verbose statistics.
```bash
./ChComboParser combos.txt - : 1 -c -v
```

---

## ⚙️ CLI Arguments Reference

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

## 🏗️ Under the Hood (Architecture)

ChComboParser is engineered for maximum I/O and CPU throughput:
1. **Bulk Memory Read:** Reads the target file directly into RAM to eliminate disk I/O bottlenecks.
2. **Smart Chunking:** Divides the file into `Threads * 4` chunks. The splitter intelligently scans for newline characters (`\n`) to ensure no line is ever split across two threads.
3. **Zero-Contention Parsing:** Each thread processes its chunk independently, applying regex-free string splitting, filtering, and transformations. Results are stored in thread-local vectors.
4. **O(1) Merge & Dedup:** The main thread merges the local vectors using a pre-allocated `std::unordered_set` for lightning-fast duplicate checking.
5. **Optimized Output:** Writes the final sanitized list to disk using buffered binary I/O.

---

## ❤️ Support the Project

If you find this tool useful, consider donating:

| Cryptocurrency | Address                                      |
| :------------- | :------------------------------------------- |
| **BTC**        | `bc1ql4syps7qpa3djqrxwht3g66tldyh4j7qsyjkq0` |
| **ETH**        | `0xfddbd535a4ad28792cbebceee3d6982d774e6d13` |
| **USDT**       | `3Cq6HRQsiwZFmPEQfG9eJkZE2QGChvf2VN`         |

> 💖 Your support helps keep the project alive and improving!

---

## ⚠️ Legal & Disclaimer

This tool is developed by **Ch4120N** strictly for **educational purposes, cybersecurity learning, and authorized security testing**. It is intended to help security researchers, penetration testers, and system administrators analyze and clean credential datasets.

🚨 **Do not use this tool for malicious purposes, unauthorized access, or illegal credential stuffing.** Always ensure you have explicit, written permission before testing or analyzing credentials against any system or network. The author assumes no liability for misuse of this software.

---

## 🤝 Contributing & Support

Found a bug? Have a feature request? 
- 🐛 [Open an Issue](https://github.com/Ch4120N/ChComboParser/issues)
- 💡 [Start a Discussion](https://github.com/Ch4120N/ChComboParser/discussions)

---

<p align="center">
  <b>Developed with ❤️ by <a href="https://github.com/Ch4120N">Ch4120N</a></b><br>
  <i>If this tool helped your workflow, consider giving the repository a ⭐ Star!</i>
</p>

---

<!-- 
======================================================
SEO & SEARCH KEYWORDS (Do not remove)
======================================================
combo list parser, credential extractor, user pass splitter, combo list deduplicator, 
multi-threaded combo parser, C++ cybersecurity tools, penetration testing utilities, 
red team wordlist tools, OSINT data cleaning, credential stuffing analysis, combolist filter, 
password list extractor, ChComboParser, Ch4120N tools, extract passwords from combo list, 
split user and pass, clean combo list, deduplicate wordlist, fast combo parser, 
C++17 CLI tool, infosec utilities, bug bounty wordlist, active directory password audit,
credential dumping parser, leak database extractor, combo list sorter, password length filter.
======================================================
-->