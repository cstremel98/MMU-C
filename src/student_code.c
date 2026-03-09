#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "student_code.h"


/**
 * =============================================================================
 * PHASE 1: SIMPLE HELPER FUNCTIONS (START HERE!)
 * =============================================================================
 */

/**
 * BASE-AND-BOUNDS HELPER FUNCTIONS
 */

PhysicalAddress base_bounds__calculate_physical_address(BaseAddress base, VirtualAddress va) {
  // todo
  return 0;
}

bool base_bounds__check_bounds(BoundLimit bound, VirtualAddress va) {
  // todo
  return false;
}


/**
 * PAGING BIT MANIPULATION HELPERS
 */

VPN paging__get_vpn_from_va(VirtualAddress va) {
  // todo
  return 0;
}

bool paging__is_entry_valid(PageTableEntry pte) {
  // todo
  return false;
}

bool paging__is_read_enabled(PageTableEntry pte) {
  // todo
  return false;
}

bool paging__is_write_enabled(PageTableEntry pte) {
  // todo
  return false;
}

bool paging__is_execute_enabled(PageTableEntry pte) {
  // todo
  return false;
}

PFN paging__convert_PageTableEntry_to_PFN(PageTableEntry pte)  {
  // todo
  return 0;
}


/**
 * =============================================================================
 * PHASE 2: MEMORY MANAGEMENT FUNCTIONS
 * =============================================================================
 */

/**
 * BASE-AND-BOUNDS MEMORY MANAGEMENT
 */

BaseAddress base_bounds__find_free_region(MMU* m) {
  // todo
  return 0;
}

ASID create_new_address_space__base_bounds(MMU* m) {
  // todo
  return MAX_ASIDS;
}

void destroy_address_space__base_bounds(MMU* m, ASID asid) {
  // todo
}


/**
 * PAGING MEMORY MANAGEMENT
 */

PFN paging__find_free_page(const MMU* m)  {
  // todo
  return -1;
}

ASID create_new_address_space__paging(MMU* m) {
  // todo
  return MAX_ASIDS;
}

void destroy_address_space__paging(MMU* m, ASID asid) {
  // todo
}


/**
 * =============================================================================
 * PHASE 3: ADDRESS TRANSLATION FUNCTIONS
 * =============================================================================
 */

/**
 * BASE-AND-BOUNDS TRANSLATION
 */

bool base_and_bounds__is_valid(const MMU* m, ASID asid, VirtualAddress va) {
  // todo
  return false;
}

PhysicalAddress translate_address__base_bounds(const MMU* m, ASID asid, VirtualAddress va) {
  // todo
  return 0;
}


/**
 * PAGING TRANSLATION
 */

PageTableEntry paging__get_pagetableentry(const MMU* m, ASID asid, VPN vpn) {
  // todo
  return 0;
}

PFN paging__map_page(const MMU* m, ASID asid, VPN vpn, bool can_read, bool can_write, bool can_exec) {
  // todo
  return -1;
}

bool paging__is_valid(const MMU* m, ASID asid, VirtualAddress va) {
  // todo
  return false;
}

PhysicalAddress translate_address__paging(const MMU* m, ASID asid, VirtualAddress va) {
  // todo
  return 0;
}


/**
 * =============================================================================
 * PHASE 4: EXTERNAL INTERFACE FUNCTIONS (FINAL INTEGRATION!)
 * =============================================================================
 */

char read_byte(const MMU* m, ASID asid, VirtualAddress va) {
  // todo
  return '\0';
}

bool write_byte(const MMU* m, ASID asid, VirtualAddress va, char val) {
  // todo
  return false;
}