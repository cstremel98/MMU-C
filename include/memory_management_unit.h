#ifndef MEMORY_MANAGEMENT_UNIT_H
#define MEMORY_MANAGEMENT_UNIT_H

#include "defines.h"
#include "stdlib.h"
#include "stdio.h"

/**
 * Memory Management Unit (MMU) Implementation
 *
 * ARCHITECTURE OVERVIEW:
 * The MMU provides a unified interface for both base-and-bounds and paging memory management.
 * Programs only work with virtual addresses - the MMU handles all translation to physical memory.
 *
 * VIRTUAL vs PHYSICAL ADDRESSES:
 * - Virtual addresses: What programs see and use (in read_byte/write_byte calls)
 * - Physical addresses: Actual offsets into the physical_memory array (internal only)
 * - Students implement translation functions that convert virtual → physical
 *
 * EXTERNAL INTERFACE (what programs call):
 * - read_byte(mmu, asid, virtual_address) → data
 * - write_byte(mmu, asid, virtual_address, data) → success/failure
 *
 * INTERNAL IMPLEMENTATION FLOW:
 * 1. validate_address(asid, virtual_address) → check if access is allowed
 * 2. translate_address(asid, virtual_address) → physical_offset
 * 3. access physical_memory[physical_offset]
 *
 * MULTI-PROCESS SUPPORT:
 * Each Address Space ID (ASID) represents a separate process with its own virtual memory space.
 * The MMU can manage up to MAX_ASIDS concurrent address spaces.
 */


/**
 * AddressSpace represents a single process's virtual memory space.
 * Each address space has its own virtual-to-physical mapping mechanism.
 */
typedef struct AddressSpace {
  ASID asid;                      // Unique identifier for this address space
  bool in_use;                    // Whether this address space slot is currently allocated

  // MMU-specific registers (only one set is used depending on MMU type)
  union {
    struct {
      BaseAddress base_register;   // Starting offset into physical_memory for this process
      BoundLimit bound_register;   // Size limit of allocated memory region (in bytes)
    } base_bounds;

    struct {
      PageTableEntry* page_table_pointer;  // Pointer to the page table for address translation
    } paging;
  } registers;
} AddressSpace;

/**
 * Memory Management Unit (MMU) struct
 *
 * This structure provides a unified interface for both base-and-bounds and paging systems.
 * The same struct is used for both approaches, with different function pointer implementations
 * and different parts of the global_state union being used.
 */
typedef struct MMU MMU;
typedef struct MMU {
  // Core MMU configuration
  char* physical_memory;                     // Pointer to the actual physical memory array
  enum MMU_type {
    BASE_AND_BOUNDS,                         // Simple base + bound memory management
    PAGING                                   // Page table-based virtual memory
  } type;

  // Per-process address spaces (each process gets one entry)
  AddressSpace address_spaces[MAX_ASIDS];    // Array of address spaces for multiple processes

  // Unified resource tracking (meaning depends on MMU type)
  // Base-bounds: tracks regions of DEFAULT_ADDRESS_SPACE_SIZE bytes
  // Paging: tracks page frames of PAGE_SIZE bytes
  bool* memory_chunk_used;

  // External interface functions (what programs call)
  bool (*write_byte)(const MMU* mmu, ASID asid, VirtualAddress va, char val);
  char (*read_byte)(const MMU* mmu, ASID asid, VirtualAddress va);

  // Internal MMU implementation functions (students implement these)
  ASID (*create_new_address_space)(MMU* mmu);
  bool (*is_valid)(const MMU* mmu, ASID asid, VirtualAddress va);
  PhysicalAddress (*translate_address)(const MMU* mmu, ASID asid, VirtualAddress va);
} MMU;


/**
 * Initialize a new base-and-bounds MMU
 *
 * Sets up the MMU struct with base-and-bounds-specific function pointers and
 * allocates the necessary data structures (physical memory, ASID tracking, etc.).
 *
 * @return Initialized MMU struct configured for base-and-bounds
 */
MMU MMU__base_bounds__init();

/**
 * Initialize a new paging-based MMU
 *
 * Sets up the MMU struct with paging-specific function pointers and allocates
 * the necessary data structures (physical memory, page tracking arrays, etc.).
 *
 * @return Initialized MMU struct configured for paging
 */
MMU MMU__pagetable__init();


/**
 * Clean up and deallocate an MMU
 *
 * Frees all dynamically allocated memory associated with the MMU including
 * physical memory, page tables, and tracking arrays.
 *
 * @param m MMU to destroy
 */
void MMU__destroy(MMU* m);

#endif //MEMORY_MANAGEMENT_UNIT_H
