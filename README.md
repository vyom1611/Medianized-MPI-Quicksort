# Medianized-MPI-Quicksort

A performance-optimized, parallel implementation of the Quicksort algorithm using MPI, emphasizing a median-based pivot selection mechanism.

---

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation and Usage](#installation-and-usage)
- [Performance Insights](#performance-insights)
- [Contribute](#contribute)
- [License](#license)
- [Acknowledgements](#acknowledgements)

---

## Introduction

Quicksort, though highly efficient in average-case scenarios, can be bottlenecked with large datasets. This project parallelizes the Quicksort algorithm with MPI and employs an optimal median-based pivot selection to enhance its performance, making it suitable for distributed environments.

---

## Features

- **Median-Based Pivot Selection:**
  - Extracts local medians from segments of data.
  - Aggregates to decide an overall median, ensuring efficient partitioning.

- **Optimized Parallel Quicksort:**
  - Data Decomposition: Distributes data evenly across processes.
  - Reduced Communication Overhead: Uses MPI functionalities to minimize inter-process communication.
  - Dynamic Process Segregation: Segregates processes based on their relative data values to efficiently tackle data chunks.

---

## Installation and Usage

*Requirements:* MPI installed

```bash
# Clone the repository
git clone https://github.com/[YourUserName]/QuantumLeapQS.git

# Navigate into the directory
cd QuantumLeapQS

# Compile the main file
mpicc -o mpi_qs main.c

# Run the program with desired number of processes and parameters
mpirun -np [Number of Processes] ./mpi_qs [Total Number of Integers] [Output File Name]

