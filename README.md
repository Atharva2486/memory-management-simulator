# Memory Management Simulator

## Overview

This project is a **Memory Management Simulator** that models how an OS
manages physical and virtual memory, including:

- Dynamic memory allocation strategies
- Internal and external fragmentation
- Virtual memory using demand paging
- Multi-level cache simulation

---

# Features

## Physical Memory Management

- Contiguous physical memory model
- Byte-addressable memory
- Linked-list based allocator (linear allocator)
- Automatic block splitting and coalescing
- Allocation success and failure tracking

### Allocation Strategies (Linear Allocator)
- First Fit
- Best Fit
- Worst Fit

### Fragmentation Metrics
- Internal fragmentation (bytes and percentage)
- External fragmentation (bytes and percentage)
- Memory utilization


## Buddy Allocator
- Power-of-two allocation
- Recursive block splitting
- XOR-based buddy computation
- Recursive merging on free
- Separate free lists per order

## Virtual Memory (Paging)
- Paging-based virtual memory
- Per-process virtual address space
- Per-process page tables
- Demand paging behavior
- Page faults and page hits
- FIFO and LRU page replacement
- Disk read/write simulation
- External fragmentation always **zero** (by design)


## Cache Simulation
- Two-level cache hierarchy (L1 → L2)
- Configurable cache size
- Configurable block (cache line) size
- Configurable associativity
- Replacement policies:
  - FIFO
  - LRU
- Hit and miss statistics per cache level

---

## Address Translation & Cache Flow
Virtual Address → Page Table → Physical Address → L1 Cache → L2 Cache → Physical Memory

---
## Demo videos
Terminal demo recording videos are available on https://drive.google.com/drive/folders/1pwxfafyGVS3bTE9h8tjxfFJ45sunA1WG?usp=sharing

---

## Build Instructions
### Clean and Build
```bash
make clean
make
```
### Run
```bash
./mmsim
```
### Initialization (Mandatory)

The simulator must be initialized before any other command.
```bash
init
```
Required inputs:
```bash
Physical memory size (bytes)
Page size (bytes)
L1 cache size
L1 block size
L1 associativity
L2 cache size
L2 block size
L2 associativity
```
## Command Summary
### Global
```bash
help
exit
```
### Physical Memory Commands (pm)
Help
```bash
help
```
Allocate Memory
```bash
pm alloc <bytes>
```
Free Memory
```bash
pm free id <id>
pm free addr <address>
```
Dump Memory Layout
```bash
pm dump
```
Memory Statistics
```bash
pm stats
```
Set Allocator Type
```bash
pm set allocator linear
pm set allocator buddy
```
Set Fit Strategy (Linear Allocator Only)
```bash
pm set fit first
pm set fit best
pm set fit worst
```
### Virtual Memory Commands (vm)
Help
```bash
help
```
Create Process
```
vm create <bytes>
```
Access Virtual Memory
```
vm create <bytes>
```
Dump Page Table
```
vm dump pagetable <pid>
```
Dump Frame Table
```
vm dump frames
```
Virtual Memory Statistics
```
vm stats
```
Set Page Replacement Policy
```
vm set page_policy fifo
vm set page_policy lru
```
### Cache Commands
Set Cache Replacement Policy
```
cache set policy fifo
cache set policy lru
```
Cache Statistics
```
cache_stats
```
---

#### Notes
- when the memory size is initialized in beginning, both the physical memory (supporting malloc [based on best fit, first fit, etc], free, etc commands) and the memory based on virtual memory (supports demand paging) is initialized together. thus physical memory and virtual memory are two independent memory systems.
- Allocation does not trigger paging or cache activity. Paging and cache behavior occur only on memory access. Subsystems are modular and explicitly initialized
