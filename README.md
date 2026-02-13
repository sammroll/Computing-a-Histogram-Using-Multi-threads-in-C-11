# Multi-Threaded File Histogram (C++11)

A multi-threaded C++11 application that computes the frequency distribution (histogram) of byte values from any text or binary file.  
This project compares two different parallel design approaches to evaluate performance and efficiency.

---

## Overview

This program reads a file and counts how many times each possible byte value (0–255) occurs.  
It uses **C++11 threads** to process the file in parallel and implements two different designs:

1. **Global Histogram Approach**
   - All threads update a shared global histogram.
   - Requires thread-safe access.

2. **Local Histogram Approach**
   - Each thread maintains its own histogram.
   - Results are merged into a final global histogram after all threads finish.

The goal is to compare both approaches and determine which design is more time-efficient.

---

## Features

- Supports **any file type** (text or binary)
- Uses **maximum available hardware threads**
- Divides file into equal segments for parallel processing
- Thread-safe histogram updates
- Outputs frequency counts for all byte values (0–255)
- Implements and compares two parallel designs

---

## Tech Stack

- C++11
- `std::thread`
- Visual Studio 2022 (x64 Console Application)
- Standard C++ libraries (`iostream`, `fstream`, `thread`, `vector`, `mutex`)

---

## How It Works

- The file is loaded into memory.
- The data is divided into segments based on the number of threads.
- Each thread processes its assigned portion.
- The program prints:
  1. Histogram using the **global shared array**
  2. Histogram using **merged local arrays**

Each histogram contains **256 bins**, one for each possible byte value. :contentReference[oaicite:0]{index=0}

---

