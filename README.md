[README (4).md](https://github.com/user-attachments/files/28297874/README.4.md)

# ⚡ C++ Lossless Compression Engine (Huffman Architecture)

![C++17](https://img.shields.io/badge/C%2B%2B17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Algorithms](https://img.shields.io/badge/Algorithms-O(N_log_N)-red?style=for-the-badge)
![Memory](https://img.shields.io/badge/Memory-Bitwise_Serialization-blue?style=for-the-badge)

A high-performance, lossless file compression and decompression engine written in pure C++. This system utilizes **Huffman Coding** and low-level bitwise operations to dynamically serialize text data into custom raw binary, achieving up to a **50-60% reduction** in file footprint without relying on external libraries.

## 🏗️ System Architecture

This engine bypasses standard 8-bit ASCII encoding by dynamically allocating variable-length prefix codes based on character frequency. The architecture is split into three core phases:

### 1. The Queue Payload & Tree Construction
* **Frequency Mapping:** The engine reads the input stream byte-by-byte, constructing a deterministic map of character occurrences.
* **The Min-Heap Engine:** An optimized `std::priority_queue` is deployed to act as a custom Min-Heap. The tree is built deterministically by continuously extracting and merging the two lowest-frequency nodes until a single root remains. 
* **Complexity:** Strictly bounded to **$O(N \log N)$** time complexity for tree construction, where $N$ is the number of unique characters.

### 2. Prefix Code Generation
* A Depth-First Search (DFS) recursively traverses the newly built Huffman Tree to generate optimal binary prefix codes. High-frequency characters are assigned ultra-short codes (e.g., `01`), while low-frequency characters receive longer paths. No code is a prefix of another, ensuring zero ambiguity during decompression.

### 3. Bitwise Serialization (The Core Engine)
* **Compression:** To prevent memory bloat, prefix string codes are not written as literal characters. The engine uses bitwise left-shifts (`<<`) and bitwise OR (`|`) to manually pack 1s and 0s into raw 8-bit `unsigned char` memory blocks before flushing them to a `.bin` file.
* **Decompression:** The decoder reads the `.bin` file via bitwise right-shifts (`>>`) and bitwise AND (`&`), mathematically walking down the reconstructed tree to reproduce the exact original byte stream.
* **Padding Protection:** The total character count is cached as a kill-switch to prevent the decompressor from reading trailing garbage bits used to pad the final 8-bit memory block.

## 📊 Performance Metrics
* **Time Complexity:** $O(N \log N)$ for encoding and $O(K)$ for decoding (where $K$ is the total number of bits).
* **Space Complexity:** $O(N)$ auxiliary space for the Tree and Hash Maps.
* **Empirical Compression Ratio:** Successfully reduced highly repetitive string datasets by **~56%** (e.g., 55 bytes compressed to 24 bytes).

## ⚙️ Compilation & Execution

This engine is designed to run natively via terminal with optimal compiler flags.

**1. Compile the Source Code:**
Instruct the C++ compiler to optimize for execution speed (`-O2`).
```bash
g++ -std=c++17 -O2 main.cpp -o compressor

