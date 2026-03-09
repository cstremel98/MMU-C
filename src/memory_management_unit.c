
#include "memory_management_unit.h"
#include "student_code.h"
#include <stdlib.h>
#include <string.h>

MMU MMU__base_bounds__init() {
  MMU m = {0};

  // Allocate physical memory
  m.physical_memory = (char*)calloc(PHYSICAL_MEMORY_SIZE, sizeof(char));

  // Set MMU type
  m.type = BASE_AND_BOUNDS;

  // Initialize all address spaces as unused
  for (int i = 0; i < MAX_ASIDS; i++) {
    m.address_spaces[i].asid = i;
    m.address_spaces[i].in_use = false;
    m.address_spaces[i].registers.base_bounds.base_register = 0;
    m.address_spaces[i].registers.base_bounds.bound_register = 0;
  }

  // Initialize memory tracking for base-and-bounds (tracks regions)
  int num_regions = PHYSICAL_MEMORY_SIZE / DEFAULT_ADDRESS_SPACE_SIZE;
  m.memory_chunk_used = (bool*)calloc(num_regions, sizeof(bool));

  // Set function pointers to base-and-bounds implementations
  m.write_byte = write_byte;
  m.read_byte = read_byte;
  m.create_new_address_space = create_new_address_space__base_bounds;
  m.is_valid = base_and_bounds__is_valid;
  m.translate_address = translate_address__base_bounds;

  return m;
}

MMU MMU__pagetable__init() {
  MMU m = {0};

  // Allocate physical memory
  m.physical_memory = (char*)calloc(PHYSICAL_MEMORY_SIZE, sizeof(char));

  // Set MMU type
  m.type = PAGING;

  // Initialize all address spaces as unused
  for (int i = 0; i < MAX_ASIDS; i++) {
    m.address_spaces[i].asid = i;
    m.address_spaces[i].in_use = false;
    m.address_spaces[i].registers.paging.page_table_pointer = NULL;
  }

  // Initialize memory tracking for paging (tracks page frames)
  m.memory_chunk_used = (bool*)calloc(NUM_FRAMES, sizeof(bool));

  // Set function pointers to paging implementations
  m.write_byte = write_byte;
  m.read_byte = read_byte;
  m.create_new_address_space = create_new_address_space__paging;
  m.is_valid = paging__is_valid;
  m.translate_address = translate_address__paging;

  return m;
}

void MMU__destroy(MMU* m) {
  // Free physical memory
  if (m->physical_memory != NULL) {
    free(m->physical_memory);
  }

  // Free unified memory tracking array
  if (m->memory_chunk_used != NULL) {
    free(m->memory_chunk_used);
  }

  // Free MMU-specific resources
  if (m->type == PAGING) {
    // Free page tables for each address space
    for (int i = 0; i < MAX_ASIDS; i++) {
      if (m->address_spaces[i].in_use &&
          m->address_spaces[i].registers.paging.page_table_pointer != NULL) {
        free(m->address_spaces[i].registers.paging.page_table_pointer);
      }
    }
  }
  // Base-and-bounds doesn't need additional per-address-space cleanup
}
