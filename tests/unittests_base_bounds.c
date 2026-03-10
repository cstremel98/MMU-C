/**
 * Unit Tests for Base-and-Bounds Memory Management Functions
 *
 * These tests verify the basic functionality of base-and-bounds helper functions.
 * They use simple, clear test cases to help students understand the expected behavior.
 * Reserve tests will handle edge cases and prevent hardcoding.
 */

#include <criterion/criterion.h>
#include <stdlib.h>
#include <time.h>
#include "defines.h"
#include "memory_management_unit.h"
#include "student_code.h"
#include "helper_functions.h"

/**
 * Test Suite: Address Calculation Functions
 * These functions handle virtual-to-physical address translation for base-and-bounds
 */

Test(BaseBounds, calculate_physical_address) {
  // Test with simple base and virtual addresses where calculation is clear

  // Base 0, virtual address 0: physical should be 0
  BaseAddress base = 0;
  VirtualAddress va = 0;
  PhysicalAddress expected_pa = 0;
  PhysicalAddress actual_pa = base_bounds__calculate_physical_address(base, va);
  cr_assert_eq(actual_pa, expected_pa,
    "Physical address should be 0 when base=0 and va=0");

  // Base 1000, virtual address 500: physical should be 1500
  base = 1000;
  va = 500;
  expected_pa = 1500;
  actual_pa = base_bounds__calculate_physical_address(base, va);
  cr_assert_eq(actual_pa, expected_pa,
    "Physical address should be base + virtual address (%d + %d = %d)",
    base, va, expected_pa);

  // Base at page boundary, virtual in middle
  base = 4 * PAGE_SIZE;  // Start of page 4
  va = 512;              // Middle of region
  expected_pa = base + va;
  actual_pa = base_bounds__calculate_physical_address(base, va);
  cr_assert_eq(actual_pa, expected_pa,
    "Should correctly calculate PA = base + VA");

  // Test the formula directly
  base = 0x2000;
  va = 0x1234;
  expected_pa = base + va;
  actual_pa = base_bounds__calculate_physical_address(base, va);
  cr_assert_eq(actual_pa, expected_pa,
    "Physical address should equal base + virtual address");
}
Test(BaseBounds, check_bounds) {
  // Test address validation against bounds register

  BoundLimit bound = 1000;  // Region size of 1000 bytes

  // Address 0 should be valid (within bounds)
  VirtualAddress va = 0;
  cr_assert(base_bounds__check_bounds(bound, va),
    "Address 0 should be within bounds of %d", bound);

  // Address at bound-1 should be valid (last valid address)
  va = bound - 1;  // 999
  cr_assert(base_bounds__check_bounds(bound, va),
    "Address %d should be within bounds of %d", va, bound);

  // Address equal to bound should be invalid (first invalid address)
  va = bound;  // 1000
  cr_assert(!base_bounds__check_bounds(bound, va),
    "Address %d should be outside bounds of %d", va, bound);

  // Address beyond bound should be invalid
  va = bound + 100;  // 1100
  cr_assert(!base_bounds__check_bounds(bound, va),
    "Address %d should be outside bounds of %d", va, bound);

  // Test with DEFAULT_ADDRESS_SPACE_SIZE
  bound = DEFAULT_ADDRESS_SPACE_SIZE;
  va = DEFAULT_ADDRESS_SPACE_SIZE - 1;  // Last valid address
  cr_assert(base_bounds__check_bounds(bound, va),
    "Should handle DEFAULT_ADDRESS_SPACE_SIZE bounds correctly");

  va = DEFAULT_ADDRESS_SPACE_SIZE;  // First invalid address
  cr_assert(!base_bounds__check_bounds(bound, va),
    "Should reject address at boundary");
}

Test(BaseBounds, find_free_region) {
  // Create and initialize MMU for testing
  MMU m = MMU__base_bounds__init();

  // Initially, first region should be available
  BaseAddress free_base = base_bounds__find_free_region(&m);
  cr_assert_eq(free_base, 0, "First available region should start at 0");
  // Mark first few regions as used
  m.memory_chunk_used[0] = true;
  m.memory_chunk_used[1] = true;
  m.memory_chunk_used[2] = true;

  // Next free region should be region 3
  free_base = base_bounds__find_free_region(&m);
  BaseAddress expected_base = 3 * DEFAULT_ADDRESS_SPACE_SIZE;
  cr_assert_eq(free_base, expected_base,
    "Next available region should start at %d", expected_base);
 
   MMU__destroy(&m);
}

Test(BaseBounds, create_new_address_space) {
  MMU m = MMU__base_bounds__init();

  // Should be able to create an address space
  ASID asid = create_new_address_space__base_bounds(&m);
  cr_assert_geq(asid, 0, "Should return valid ASID");
  cr_assert_lt(asid, MAX_ASIDS, "ASID should be within valid range");
  // Address space should be marked as in use
  cr_assert(m.address_spaces[asid].in_use, "Address space should be marked in use");

  // Registers should be set up with valid values
  BaseAddress base = m.address_spaces[asid].registers.base_bounds.base_register;
  BoundLimit bound = m.address_spaces[asid].registers.base_bounds.bound_register;

  cr_assert_geq(base, 0, "Base register should be valid");
  cr_assert_eq(bound, DEFAULT_ADDRESS_SPACE_SIZE,
    "Bound register should be set to DEFAULT_ADDRESS_SPACE_SIZE");

  // The corresponding physical region should be marked as used
  int region_index = base / DEFAULT_ADDRESS_SPACE_SIZE;
  cr_assert(m.memory_chunk_used[region_index],
    "Physical region %d should be marked as used", region_index);
  
  MMU__destroy(&m);
}

Test(BaseBounds, translate_address_basic) {
  // This is a basic test - more complex scenarios in reserve tests
  MMU m = MMU__base_bounds__init();
  ASID asid = create_new_address_space__base_bounds(&m);

  // Get the assigned base address
  BaseAddress base = m.address_spaces[asid].registers.base_bounds.base_register;

  // Test translation of various addresses
  VirtualAddress va = 0;
  PhysicalAddress expected_pa = base + va;
  PhysicalAddress actual_pa = translate_address__base_bounds(&m, asid, va);
  cr_assert_eq(actual_pa, expected_pa,
    "Physical address should equal base + virtual address");
  // Test with non-zero virtual address
  va = 1024;
  expected_pa = base + va;
  actual_pa = translate_address__base_bounds(&m, asid, va);
  cr_assert_eq(actual_pa, expected_pa,
    "Should correctly translate VA %d to PA %d", va, expected_pa);
  MMU__destroy(&m);
}

Test(BaseBounds, is_valid_basic) {
  MMU m = MMU__base_bounds__init();
  ASID asid = create_new_address_space__base_bounds(&m);

  // Address 0 should be valid (start of address space)
  VirtualAddress va = 0;
  cr_assert(base_and_bounds__is_valid(&m, asid, va),
    "Address 0 should be valid");
  // Address within the default size should be valid
  va = DEFAULT_ADDRESS_SPACE_SIZE / 2;
  cr_assert(base_and_bounds__is_valid(&m, asid, va),
    "Address in middle of space should be valid");

  // Last valid address should be valid
  va = DEFAULT_ADDRESS_SPACE_SIZE - 1;
  cr_assert(base_and_bounds__is_valid(&m, asid, va),
    "Last address in space should be valid");

  // Address at boundary should be invalid
  va = DEFAULT_ADDRESS_SPACE_SIZE;
  cr_assert(!base_and_bounds__is_valid(&m, asid, va),
    "Address at boundary should be invalid");

  // Address beyond boundary should be invalid
  va = DEFAULT_ADDRESS_SPACE_SIZE + 1000;
  cr_assert(!base_and_bounds__is_valid(&m, asid, va),
    "Address beyond boundary should be invalid");
  MMU__destroy(&m);
}

Test(BaseBounds, destroy_address_space) {
  MMU m = MMU__base_bounds__init();

  // Create an address space
  ASID asid = create_new_address_space__base_bounds(&m);
  BaseAddress base = m.address_spaces[asid].registers.base_bounds.base_register;
  int region_index = base / DEFAULT_ADDRESS_SPACE_SIZE;

  // Verify it's set up
  cr_assert(m.address_spaces[asid].in_use, "Address space should be in use");
  cr_assert(m.memory_chunk_used[region_index],
    "Physical region should be marked as used");

  // Destroy the address space
  destroy_address_space__base_bounds(&m, asid);
  // Verify cleanup
  cr_assert(!m.address_spaces[asid].in_use, "Address space should no longer be in use");
  cr_assert(!m.memory_chunk_used[region_index],
    "Physical region should be freed");

  // Registers should be cleared
  cr_assert_eq(m.address_spaces[asid].registers.base_bounds.base_register, 0,
    "Base register should be cleared");
  cr_assert_eq(m.address_spaces[asid].registers.base_bounds.bound_register, 0,
    "Bound register should be cleared");
  MMU__destroy(&m);
}

Test(BaseBounds, multiple_address_spaces) {
  MMU m = MMU__base_bounds__init();

  // Create multiple address spaces
  ASID asid1 = create_new_address_space__base_bounds(&m);
  ASID asid2 = create_new_address_space__base_bounds(&m);
  ASID asid3 = create_new_address_space__base_bounds(&m);

  // All should be valid and different
  cr_assert_geq(asid1, 0, "First ASID should be valid");
  cr_assert_geq(asid2, 0, "Second ASID should be valid");
  cr_assert_geq(asid3, 0, "Third ASID should be valid");
  cr_assert_neq(asid1, asid2, "ASIDs should be different");
  /*
  cr_assert_neq(asid2, asid3, "ASIDs should be different");
  cr_assert_neq(asid1, asid3, "ASIDs should be different");

  // Each should have different base addresses
  BaseAddress base1 = m.address_spaces[asid1].registers.base_bounds.base_register;
  BaseAddress base2 = m.address_spaces[asid2].registers.base_bounds.base_register;
  BaseAddress base3 = m.address_spaces[asid3].registers.base_bounds.base_register;

  cr_assert_neq(base1, base2, "Base addresses should be different");
  cr_assert_neq(base2, base3, "Base addresses should be different");
  cr_assert_neq(base1, base3, "Base addresses should be different");

  // Same virtual address should translate to different physical addresses
  VirtualAddress va = 100;
  PhysicalAddress pa1 = translate_address__base_bounds(&m, asid1, va);
  PhysicalAddress pa2 = translate_address__base_bounds(&m, asid2, va);
  PhysicalAddress pa3 = translate_address__base_bounds(&m, asid3, va);

  cr_assert_neq(pa1, pa2, "Same VA should map to different PAs in different spaces");
  cr_assert_neq(pa2, pa3, "Same VA should map to different PAs in different spaces");
  cr_assert_neq(pa1, pa3, "Same VA should map to different PAs in different spaces");

  // Verify the translations are correct
  cr_assert_eq(pa1, base1 + va, "Translation should be base + VA for space 1");
  cr_assert_eq(pa2, base2 + va, "Translation should be base + VA for space 2");
  cr_assert_eq(pa3, base3 + va, "Translation should be base + VA for space 3");
*/
  MMU__destroy(&m);
}
