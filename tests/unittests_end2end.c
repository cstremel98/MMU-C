/**
 * End-to-End Integration Tests for MMU External Interface
 *
 * These tests verify the complete flow from the external API (read_byte/write_byte)
 * through MMU translation to physical memory access. They test both base-and-bounds
 * and paging systems to ensure the unified interface works correctly.
 */

#include <criterion/criterion.h>
#include <stdlib.h>
#include <time.h>
#include "defines.h"
#include "memory_management_unit.h"
#include "student_code.h"
#include "helper_functions.h"

/**
 * Test Suite: Base-and-Bounds Integration
 * These tests verify read_byte/write_byte work correctly with base-and-bounds MMU
 */

Test(End2End, base_bounds_write_read_basic) {
  // Create base-and-bounds MMU and address space
  MMU m = MMU__base_bounds__init();
  ASID asid = create_new_address_space__base_bounds(&m);
  cr_assert_geq(asid, 0, "Should successfully create address space");

  // Test writing and reading a single byte
  VirtualAddress va = 100;
  char test_value = 0x42;

  // Write byte through external interface
  bool write_success = write_byte(&m, asid, va, test_value);
  cr_assert(write_success, "Write should succeed for valid address");

  // Read byte back through external interface
  char read_value = read_byte(&m, asid, va);
  cr_assert_eq(read_value, test_value,
    "Read value should match written value (0x%02x)", test_value);

  MMU__destroy(&m);
}

Test(End2End, base_bounds_multiple_writes) {
  // Test writing multiple different values
  MMU m = MMU__base_bounds__init();
  ASID asid = create_new_address_space__base_bounds(&m);

  // Write different values to different addresses
  struct { VirtualAddress va; char value; } test_data[] = {
    {0, 0x11},
    {50, 0x22},
    {100, 0x33},
    {200, 0x44},
    {DEFAULT_ADDRESS_SPACE_SIZE - 1, 0x55}  // Last valid address
  };
  int num_tests = sizeof(test_data) / sizeof(test_data[0]);

  // Write all values
  for (int i = 0; i < num_tests; i++) {
    bool success = write_byte(&m, asid, test_data[i].va, test_data[i].value);
    cr_assert(success, "Write should succeed for address %d", test_data[i].va);
  }

  // Read all values back and verify
  for (int i = 0; i < num_tests; i++) {
    char read_value = read_byte(&m, asid, test_data[i].va);
    cr_assert_eq(read_value, test_data[i].value,
      "Address %d should contain 0x%02x, got 0x%02x",
      test_data[i].va, test_data[i].value, read_value);
  }

  MMU__destroy(&m);
}

Test(End2End, base_bounds_invalid_addresses) {
  // Test that invalid addresses are handled correctly
  MMU m = MMU__base_bounds__init();
  ASID asid = create_new_address_space__base_bounds(&m);

  // Try to write to invalid addresses (beyond bounds)
  VirtualAddress invalid_addresses[] = {
    DEFAULT_ADDRESS_SPACE_SIZE,      // At boundary
    DEFAULT_ADDRESS_SPACE_SIZE + 1,  // Just beyond
    DEFAULT_ADDRESS_SPACE_SIZE + 1000, // Far beyond
    0xFFFFFFFF                       // Very large address
  };
  int num_invalid = sizeof(invalid_addresses) / sizeof(invalid_addresses[0]);

  for (int i = 0; i < num_invalid; i++) {
    bool write_success = write_byte(&m, asid, invalid_addresses[i], 0x99);
    cr_assert(!write_success, "Write should fail for invalid address %d", invalid_addresses[i]);

    char read_value = read_byte(&m, asid, invalid_addresses[i]);
    cr_assert_eq(read_value, 0, "Read from invalid address should return 0");
  }

  MMU__destroy(&m);
}

Test(End2End, base_bounds_multiple_address_spaces) {
  // Test that different address spaces are properly isolated
  MMU m = MMU__base_bounds__init();

  // Create multiple address spaces
  ASID asid1 = create_new_address_space__base_bounds(&m);
  ASID asid2 = create_new_address_space__base_bounds(&m);
  cr_assert_geq(asid1, 0, "Should create first address space");
  cr_assert_geq(asid2, 0, "Should create second address space");
  cr_assert_neq(asid1, asid2, "Address spaces should be different");

  // Write different values to the same virtual address in each space
  VirtualAddress va = 50;
  char value1 = 0xAA;
  char value2 = 0xBB;

  cr_assert(write_byte(&m, asid1, va, value1), "Write to space 1 should succeed");
  cr_assert(write_byte(&m, asid2, va, value2), "Write to space 2 should succeed");

  // Read back from each space - should get different values
  char read1 = read_byte(&m, asid1, va);
  char read2 = read_byte(&m, asid2, va);

  cr_assert_eq(read1, value1, "Space 1 should contain its value (0x%02x)", value1);
  cr_assert_eq(read2, value2, "Space 2 should contain its value (0x%02x)", value2);
  cr_assert_neq(read1, read2, "Different spaces should contain different values");

  MMU__destroy(&m);
}

/**
 * Test Suite: Paging Integration
 * These tests verify read_byte/write_byte work correctly with paging MMU
 */

Test(End2End, paging_write_read_basic) {
  // Create paging MMU and address space
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);
  cr_assert_geq(asid, 0, "Should successfully create address space");

  // Map a page first (paging requires explicit mapping)
  VPN vpn = 0;
  PFN pfn = paging__map_page(&m, asid, vpn, true, true, false);
  cr_assert_geq(pfn, 0, "Should successfully map page");

  // Test writing and reading within the mapped page
  VirtualAddress va = 100;  // In page 0
  char test_value = 0x73;

  // Write byte through external interface
  bool write_success = write_byte(&m, asid, va, test_value);
  cr_assert(write_success, "Write should succeed for mapped page");

  // Read byte back through external interface
  char read_value = read_byte(&m, asid, va);
  cr_assert_eq(read_value, test_value,
    "Read value should match written value (0x%02x)", test_value);

  MMU__destroy(&m);
}

Test(End2End, paging_multiple_pages) {
  // Test writing to multiple mapped pages
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);

  // Map several pages with different permissions
  VPN vpn1 = 0, vpn2 = 2, vpn3 = 5;
  paging__map_page(&m, asid, vpn1, true, true, false);   // Read/write
  paging__map_page(&m, asid, vpn2, true, true, false);   // Read/write
  paging__map_page(&m, asid, vpn3, true, false, false);  // Read-only

  // Test data for different pages
  struct { VirtualAddress va; char value; bool should_write; } test_data[] = {
    {vpn1 * PAGE_SIZE + 50, 0x11, true},   // Page 0, should work
    {vpn2 * PAGE_SIZE + 200, 0x22, true},  // Page 2, should work
    {vpn3 * PAGE_SIZE + 100, 0x33, false}, // Page 5, read-only (if permissions enforced)
  };
  int num_tests = sizeof(test_data) / sizeof(test_data[0]);

  // Write to all mapped pages
  for (int i = 0; i < num_tests; i++) {
    bool success = write_byte(&m, asid, test_data[i].va, test_data[i].value);
    if (test_data[i].should_write) {
      cr_assert(success, "Write should succeed for writable page at VA %d", test_data[i].va);
    }
    // Note: Read-only permission checking might not be implemented in basic version
  }

  // Read back from readable pages
  for (int i = 0; i < num_tests; i++) {
    if (test_data[i].should_write) {  // Only check pages we successfully wrote to
      char read_value = read_byte(&m, asid, test_data[i].va);
      cr_assert_eq(read_value, test_data[i].value,
        "Page should contain written value 0x%02x", test_data[i].value);
    }
  }

  MMU__destroy(&m);
}

Test(End2End, paging_unmapped_pages) {
  // Test that accessing unmapped pages fails correctly
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);

  // Map only page 0
  paging__map_page(&m, asid, 0, true, true, false);

  // Try to access unmapped pages
  VirtualAddress unmapped_addresses[] = {
    1 * PAGE_SIZE,     // Page 1 (unmapped)
    3 * PAGE_SIZE,     // Page 3 (unmapped)
    10 * PAGE_SIZE + 500, // Page 10 (unmapped)
  };
  int num_unmapped = sizeof(unmapped_addresses) / sizeof(unmapped_addresses[0]);

  for (int i = 0; i < num_unmapped; i++) {
    bool write_success = write_byte(&m, asid, unmapped_addresses[i], 0x99);
    cr_assert(!write_success, "Write should fail for unmapped page at VA %d", unmapped_addresses[i]);

    char read_value = read_byte(&m, asid, unmapped_addresses[i]);
    cr_assert_eq(read_value, 0, "Read from unmapped page should return 0");
  }

  // Verify mapped page still works
  cr_assert(write_byte(&m, asid, 50, 0x42), "Mapped page should still work");
  cr_assert_eq(read_byte(&m, asid, 50), 0x42, "Mapped page should contain written data");

  MMU__destroy(&m);
}

Test(End2End, paging_multiple_address_spaces) {
  // Test that different paging address spaces are properly isolated
  MMU m = MMU__pagetable__init();

  // Create multiple address spaces
  ASID asid1 = create_new_address_space__paging(&m);
  ASID asid2 = create_new_address_space__paging(&m);
  cr_assert_geq(asid1, 0, "Should create first address space");
  cr_assert_geq(asid2, 0, "Should create second address space");

  // Map the same VPN in both address spaces (should get different PFNs)
  VPN vpn = 0;
  PFN pfn1 = paging__map_page(&m, asid1, vpn, true, true, false);
  PFN pfn2 = paging__map_page(&m, asid2, vpn, true, true, false);
  cr_assert_neq(pfn1, pfn2, "Same VPN should map to different PFNs in different spaces");

  // Write different values to the same virtual address
  VirtualAddress va = 100;
  char value1 = 0xCC;
  char value2 = 0xDD;

  cr_assert(write_byte(&m, asid1, va, value1), "Write to space 1 should succeed");
  cr_assert(write_byte(&m, asid2, va, value2), "Write to space 2 should succeed");

  // Read back from each space - should get different values
  char read1 = read_byte(&m, asid1, va);
  char read2 = read_byte(&m, asid2, va);

  cr_assert_eq(read1, value1, "Space 1 should contain its value (0x%02x)", value1);
  cr_assert_eq(read2, value2, "Space 2 should contain its value (0x%02x)", value2);
  cr_assert_neq(read1, read2, "Different spaces should contain different values");

  MMU__destroy(&m);
}

/**
 * Test Suite: Mixed System Tests
 * These tests verify behavior across different scenarios
 */

Test(End2End, invalid_asid_handling) {
  // Test behavior with invalid ASIDs
  MMU m_base = MMU__base_bounds__init();
  MMU m_paging = MMU__pagetable__init();

  ASID invalid_asids[] = {MAX_ASIDS, MAX_ASIDS + 1, 255};
  int num_invalid = sizeof(invalid_asids) / sizeof(invalid_asids[0]);

  for (int i = 0; i < num_invalid; i++) {
    // Test invalid ASID with base-and-bounds
    cr_assert(!write_byte(&m_base, invalid_asids[i], 100, 0x99),
      "Base-bounds write should fail for invalid ASID %d", invalid_asids[i]);
    cr_assert_eq(read_byte(&m_base, invalid_asids[i], 100), 0,
      "Base-bounds read should return 0 for invalid ASID %d", invalid_asids[i]);

    // Test invalid ASID with paging
    cr_assert(!write_byte(&m_paging, invalid_asids[i], 100, 0x99),
      "Paging write should fail for invalid ASID %d", invalid_asids[i]);
    cr_assert_eq(read_byte(&m_paging, invalid_asids[i], 100), 0,
      "Paging read should return 0 for invalid ASID %d", invalid_asids[i]);
  }

  MMU__destroy(&m_base);
  MMU__destroy(&m_paging);
}

Test(End2End, stress_test_small) {
  // Small stress test with multiple operations
  MMU m = MMU__base_bounds__init();
  ASID asid = create_new_address_space__base_bounds(&m);

  // Write pattern to many addresses
  int num_addresses = 50;
  for (int i = 0; i < num_addresses; i++) {
    VirtualAddress va = i * 10;  // Spread out addresses
    char value = (char)(i + 1);  // Unique values

    if (va < DEFAULT_ADDRESS_SPACE_SIZE) {  // Stay within bounds
      cr_assert(write_byte(&m, asid, va, value),
        "Write should succeed for address %d", va);
    }
  }

  // Read back and verify pattern
  for (int i = 0; i < num_addresses; i++) {
    VirtualAddress va = i * 10;
    char expected_value = (char)(i + 1);

    if (va < DEFAULT_ADDRESS_SPACE_SIZE) {  // Stay within bounds
      char actual_value = read_byte(&m, asid, va);
      cr_assert_eq(actual_value, expected_value,
        "Address %d should contain value %d", va, expected_value);
    }
  }

  MMU__destroy(&m);
}