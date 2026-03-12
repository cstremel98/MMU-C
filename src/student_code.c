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
	PhysicalAddress pa = base + va;
	return pa;
}

bool base_bounds__check_bounds(BoundLimit bound, VirtualAddress va) {
  if(!(va >= bound)) {
		return true;
	}
	return false;
}


/**
 * PAGING BIT MANIPULATION HELPERS
 */

VPN paging__get_vpn_from_va(VirtualAddress va) {
	VPN vpn = va >> 10;
	return vpn;
}

bool paging__is_entry_valid(PageTableEntry pte) {
  if((pte >> 63 & 1ULL) == 1) {
		return true;
	}
	return false;
}

bool paging__is_read_enabled(PageTableEntry pte) {
  if((pte >> 62 & 1ULL) == 1) {
		return true;
	}
	return false;
}

bool paging__is_write_enabled(PageTableEntry pte) {
  if((pte >> 61 & 1ULL) == 1) {
		return true;
	}
  return false;
}

bool paging__is_execute_enabled(PageTableEntry pte) {
  if((pte >> 60 & 1ULL) == 1) {
		return true;
	}
  return false;
}

PFN paging__convert_PageTableEntry_to_PFN(PageTableEntry pte)  {
	// Unsigned integer shift left, then shift right...newly created bits == 0
	return ((pte << 4) >> 4);
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
	int x = 0;
	
	//?
	for(int i=0; i<MAX_ASIDS; i++) {
		//First available region
		if(m->memory_chunk_used[i] == false && i == 0) {
			return 0;
		}
		//Used memory
		if(m->memory_chunk_used[i] == true) {
			//i+1 to match numbering
			x = i+1;
		}
	}
	x = x * DEFAULT_ADDRESS_SPACE_SIZE;
	
	return x;
}

ASID create_new_address_space__base_bounds(MMU* m) {
	for(int i = 0; i < MAX_ASIDS; i++) {
		if((m->address_spaces[i].in_use == false)) {	
			m->address_spaces[i].in_use = true;
			m->address_spaces[i].registers.base_bounds.base_register = i * DEFAULT_ADDRESS_SPACE_SIZE;
			m->address_spaces[i].registers.base_bounds.bound_register = DEFAULT_ADDRESS_SPACE_SIZE;
			m->memory_chunk_used[i] = true;

			return m->address_spaces[i].asid;
		} 
	}
	return MAX_ASIDS;
}

void destroy_address_space__base_bounds(MMU* m, ASID asid) {
	m->address_spaces[asid].in_use = false;
	m->memory_chunk_used[asid] = false;
	m->address_spaces[asid].registers.base_bounds.base_register = 0;
	m->address_spaces[asid].registers.base_bounds.bound_register = 0;
}


/**
 * PAGING MEMORY MANAGEMENT
 */

PFN paging__find_free_page(const MMU* m)  {
   for (int i = 0; i < MAX_ASIDS; i++) {
	 	if(m->memory_chunk_used[i] == false) {
			return i;
		}
  }
	return -1;
}

ASID create_new_address_space__paging(MMU* m) {
	for (int i = 0; i < MAX_ASIDS; i++) {
		if(m->address_spaces[i].in_use == false) {
			m->address_spaces[i].in_use = true;
			return m->address_spaces[i].asid;
		}
	}
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
	if (va >= DEFAULT_ADDRESS_SPACE_SIZE) {
		return false;
  } else if(va >= asid) {
		return true;
	}
	return false;
}

PhysicalAddress translate_address__base_bounds(const MMU* m, ASID asid, VirtualAddress va) {
	PhysicalAddress pa = base_bounds__calculate_physical_address(m->address_spaces[asid].registers.base_bounds.base_register, va);
	return pa;
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
