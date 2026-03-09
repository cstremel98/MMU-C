# CST334 Programming Assignment 3: Memory Management Unit (MMU)

## Assignment Overview

In this project we're going to build two different types of MMUs to understand how virtual memory works! 

This assignment implements two fundamental memory management approaches. 
You will build both a base-and-bounds MMU and a paging MMU that provide the same external interface but use completely different internal mechanisms.

**Part 1: Base-and-Bounds MMU** - Start here!
**Part 2: Paging MMU** - Build on Part 1 concepts

### Learning Objectives

By completing this assignment, you will understand:
- The difference between virtual and physical addresses
- How memory management units translate addresses
- Two different approaches to implementing virtual memory
- How multiple processes can have separate address spaces
- The trade-offs between simple and complex memory management systems

### Project Structure

The assignment uses a unified MMU interface that supports multiple address spaces through Address Space IDs (ASIDs). Both memory management approaches work with the same external functions (`read_byte` and `write_byte`) but implement different internal translation mechanisms.

**Key Concept**: Programs only see virtual addresses. The MMU handles all translation to physical memory internally.

## Implementation Tasks

### Part 1: Base-and-Bounds Memory Management

Implement simple memory management where each process gets a contiguous region of physical memory defined by base address and size limit.

**Core Functions:**
```c
ASID create_new_address_space__base_bounds(MMU* m);
bool base_and_bounds__is_valid(const MMU* m, ASID asid, VirtualAddress va);
PhysicalAddress translate_address__base_bounds(const MMU* m, ASID asid, VirtualAddress va);
```

**How it works:**
- Each process gets a contiguous region in physical memory
- Translation: `physical_address = base_register + virtual_address`
- Validation: Virtual address must be within the allocated region

### Part 2: Paging Memory Management

Implement page table-based memory management where virtual memory is divided into fixed-size pages that can map to any physical page frames.

**Core Functions:**
```c
ASID create_new_address_space__paging(MMU* m);
bool paging__is_valid(const MMU* m, ASID asid, VirtualAddress va);
PhysicalAddress translate_address__paging(const MMU* m, ASID asid, VirtualAddress va);
```

**Helper Functions:**
```c
// Address manipulation
VPN paging__get_vpn_from_va(VirtualAddress va);
PFN paging__convert_PageTableEntry_to_PFN(PageTableEntry pte);

// Page table management
PageTableEntry paging__get_pagetableentry(const MMU* m, ASID asid, VPN vpn);
PFN paging__map_page(const MMU* m, ASID asid, VPN vpn, bool can_read, bool can_write, bool can_exec);
PFN paging__find_free_page(const MMU* m);

// Permission checking
bool paging__is_entry_valid(PageTableEntry pte);
bool paging__is_read_enabled(PageTableEntry pte);
bool paging__is_write_enabled(PageTableEntry pte);
bool paging__is_execute_enabled(PageTableEntry pte);
```

**How it works:**
- Virtual addresses are split into Virtual Page Number (VPN) and offset
- Page table maps VPNs to Physical Frame Numbers (PFNs)
- Translation: Look up PFN, combine with offset to get physical address

## Getting Started

### Key Files to Understand

Start by examining these files to understand the project structure:

- **[`memory_management_unit.h`](./include/memory_management_unit.h)** - Defines the MMU structure and overall architecture
- **[`defines.h`](./include/defines.h)** - Contains all type definitions and system constants
- **[`student_code.h`](./include/student_code.h)** - Function signatures you need to implement

### Recommended Implementation Order

**Start with Part 1 (Base-and-Bounds):**
1. Read through the header files to understand the data structures
2. Implement `create_new_address_space__base_bounds()` - sets up address space
3. Implement `base_and_bounds__is_valid()` - simple bounds checking
4. Implement `translate_address__base_bounds()` - basic arithmetic translation
5. Test with `read_byte`/`write_byte` functions

**Then Move to Part 2 (Paging):**
1. Start with the helper functions (bit manipulation and checking functions)
2. Implement `paging__get_vpn_from_va()` and page table entry functions
3. Implement `create_new_address_space__paging()` and `paging__is_valid()`
4. Implement `translate_address__paging()` - the most complex function
5. Test thoroughly with both unit tests and end-to-end tests

### Testing Your Code

This project follows the standard build system used in previous assignments:

- **`make`** - Builds and runs all tests
- **`make unit_tests`** - Builds the test executable
- **`make debug`** - Builds and launches the debug executable with gdb

Both memory management systems use the same external interface (`read_byte` and `write_byte`), allowing you to test them identically despite their different internal implementations.

### System Configuration

The memory system is configured for educational purposes:
- **Physical memory**: 64KB total (64 page frames of 1KB each)
- **Virtual address space**: 4MB per process (4096 virtual pages)
- **Base-and-bounds regions**: 4KB per process (physical memory / max processes)
- **Page size**: 1KB (1024 bytes)
- **Maximum processes**: 16 concurrent address spaces

### Page Table Entry Format

Page table entries contain both addressing information and permission bits:
- **Page Frame Number (PFN)**: Lower bits specify which physical page frame
- **Valid bit**: MSB indicates if the page is mapped
- **Permission bits**: Read, Write, and Execute permissions stored in high-order bits

The helper functions you implement will extract and manipulate these bit fields.

## Grading Breakdown

**Total Points: 100**

| Function | Points | Description | Header |
|----------|--------|-------------|---------|
| **Base-and-Bounds Implementation (50 points)** | | | |
| `create_new_address_space__base_bounds` | 15 | Set up new process address space | [`student_code.h`](./include/student_code.h) |
| `base_and_bounds__is_valid` | 15 | Check if virtual address is within bounds | [`student_code.h`](./include/student_code.h) |
| `translate_address__base_bounds` | 20 | Convert virtual to physical address | [`student_code.h`](./include/student_code.h) |
| **Paging Core Implementation (15 points)** | | | |
| `create_new_address_space__paging` | 5 | Set up new page table | [`student_code.h`](./include/student_code.h) |
| `paging__is_valid` | 5 | Check if virtual address maps to valid page | [`student_code.h`](./include/student_code.h) |
| `translate_address__paging` | 5 | Convert virtual to physical via page table | [`student_code.h`](./include/student_code.h) |
| **Paging Helper Functions (20 points)** | | | |
| `paging__is_entry_valid` | 10* | Check if page table entry is valid | [`student_code.h`](./include/student_code.h) |
| `paging__is_read_enabled` | | Check if page allows read access | [`student_code.h`](./include/student_code.h) |
| `paging__is_write_enabled` | | Check if page allows write access | [`student_code.h`](./include/student_code.h) |
| `paging__is_execute_enabled` | | Check if page allows execute access | [`student_code.h`](./include/student_code.h) |
| `paging__get_vpn_from_va` | 10* | Extract Virtual Page Number from address | [`student_code.h`](./include/student_code.h) |
| `paging__convert_PageTableEntry_to_PFN` | | Extract Page Frame Number from entry | [`student_code.h`](./include/student_code.h) |
| `paging__find_free_page` | | Find available physical page frame | [`student_code.h`](./include/student_code.h) |
| `paging__map_page` | | Create new page table mapping | [`student_code.h`](./include/student_code.h) |
| `paging__get_pagetableentry` | | Look up page table entry for VPN | [`student_code.h`](./include/student_code.h) |
| **End-to-End Integration (15 points)** | | | |
| `write_byte` | 10* | Write data using base-and-bounds MMU | [`student_code.h`](./include/student_code.h) |
| `read_byte` | | Read data using base-and-bounds MMU | [`student_code.h`](./include/student_code.h) |
| `write_byte` | 5* | Write data using paging MMU | [`student_code.h`](./include/student_code.h) |
| `read_byte` | | Read data using paging MMU | [`student_code.h`](./include/student_code.h) |

_*Points are shared across grouped functions_

**Strategy for Success:** Implementing just the base-and-bounds functions (50 points) plus basic paging helpers (10 points) gets you to 60 points. Full credit requires mastering the complete paging system.

## What to submit

You'll be submitting ***only*** `student_code.c` and `student_code.h`.
Any other files changed will be lost during the grading process.

## Conclusion

Good luck!
