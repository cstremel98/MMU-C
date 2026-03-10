#ifndef DEFINES_H
#define DEFINES_H

#include "stdint.h"
#include "stdbool.h"

// System configuration
#define NUM_BITS_IN_BYTE 8
#define MAX_ASIDS 16                // Maximum number of concurrent address spaces

// Base-and-bounds system configuration
#define MAX_BOUND_SIZE PHYSICAL_MEMORY_SIZE                          // Maximum size a memory region can be
#define DEFAULT_ADDRESS_SPACE_SIZE (PHYSICAL_MEMORY_SIZE / MAX_ASIDS)  // Default size for new address spaces

// Page table configuration - all derived values calculated from these core parameters
#define PAGETABLE_ENTRY_TYPE uint64_t
#define NUM_OFFSET_BITS 10          // Bits for offset within a page (2^10 = 1024 byte pages)
#define NUM_VPN_BITS 12             // Bits for Virtual Page Number (2^12 = 4096 virtual pages)
#define NUM_PFN_BITS 6              // Bits for Page Frame Number (2^6 = 64 physical frames = 64KB total)
#define NUM_METADATA_BITS 4         // Bits reserved for metadata (valid, read, write, execute)

// Derived system sizes (calculated from above parameters)
#define PAGE_SIZE ((1<<NUM_OFFSET_BITS))                              // Size of each page in bytes
#define NUM_PAGES ((1<<NUM_VPN_BITS))                                 // Total virtual pages per address space
#define NUM_FRAMES ((1<<NUM_PFN_BITS))                                // Total physical page frames available
#define PHYSICAL_MEMORY_SIZE ((1<<(NUM_PFN_BITS + NUM_OFFSET_BITS)))  // Total physical memory size

// Page table entry metadata bit positions (stored in MSB of page table entries)
#define VALID_BIT (NUM_BITS_IN_BYTE*sizeof(AddressBasis) - 0)  // Page is valid/allocated
#define READ_BIT (NUM_BITS_IN_BYTE*sizeof(AddressBasis) - 1)   // Page can be read
#define WRITE_BIT (NUM_BITS_IN_BYTE*sizeof(AddressBasis) - 2)  // Page can be written
#define EXEC_BIT (NUM_BITS_IN_BYTE*sizeof(AddressBasis) - 3)   // Page can be executed


typedef uint8_t ASID;

typedef PAGETABLE_ENTRY_TYPE AddressBasis;

typedef AddressBasis AddressMask;       // Bit masks for extracting address components
typedef AddressBasis VirtualAddress;    // Addresses as seen by programs (what programs use)
typedef AddressBasis PhysicalAddress;   // Real addresses in physical memory (what hardware uses)
typedef AddressBasis OFFSET;            // Offset within a page (both base-bounds and paging)

typedef AddressBasis VPN;               // Virtual Page Number (high bits of virtual address in paging)
typedef AddressBasis PFN;               // Page Frame Number (physical page identifier, may include metadata bits)

typedef AddressBasis BaseAddress;       // Base address of allocated memory region
typedef AddressBasis BoundLimit;        // Size limit of memory region (in bytes)
typedef AddressBasis BoundOffset;       // Offset within a memory region

typedef struct Page {
  char data[PAGE_SIZE];                 // Raw page data
} Page;
typedef Page PageFrame;                 // Alias for clarity - same as Page but represents physical memory

typedef PFN PageTableEntry;                 // Page table entries store PFN + metadata bits
typedef PageTableEntry PageDirectoryEntry;  // Page directory entries (same format as PTE)
typedef Page PageTableChunk;                // Pages that contain page table data


#endif //DEFINES_H
