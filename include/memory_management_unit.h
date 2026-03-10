#ifndef MEMORY_MANAGEMENT_UNIT_H
#define MEMORY_MANAGEMENT_UNIT_H

#include "defines.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct AddressSpace {
  ASID asid;                      // Unique identifier for this address space
  bool in_use;                    // Whether this address space slot is currently allocated

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

typedef struct MMU MMU;
typedef struct MMU {
  char* physical_memory;                     // Pointer to the actual physical memory array
  enum MMU_type {
    BASE_AND_BOUNDS,                         // Simple base + bound memory management
    PAGING                                   // Page table-based virtual memory
  } type;

  AddressSpace address_spaces[MAX_ASIDS];    // Array of address spaces for multiple processes

  bool* memory_chunk_used;

  bool (*write_byte)(const MMU* mmu, ASID asid, VirtualAddress va, char val);
  char (*read_byte)(const MMU* mmu, ASID asid, VirtualAddress va);

  ASID (*create_new_address_space)(MMU* mmu);
  bool (*is_valid)(const MMU* mmu, ASID asid, VirtualAddress va);
  PhysicalAddress (*translate_address)(const MMU* mmu, ASID asid, VirtualAddress va);
} MMU;


MMU MMU__base_bounds__init();

MMU MMU__pagetable__init();

void MMU__destroy(MMU* m);

#endif //MEMORY_MANAGEMENT_UNIT_H
