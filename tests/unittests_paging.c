/**
 * Unit Tests for Paging Memory Management Functions
 *
 * These tests verify the basic functionality of paging helper functions.
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

Test(Paging, is_entry_valid) {
  PageTableEntry pte = 0x1234;  // Some data but no valid bit
  cr_assert(!paging__is_entry_valid(pte), "Entry without valid bit should be invalid");

  pte = pte | get_mask_single_bit(VALID_BIT);
  cr_assert(paging__is_entry_valid(pte), "Entry with valid bit should be valid");
}

Test(Paging, is_read_enabled) {
  // Test with read disabled
  PageTableEntry pte = get_mask_single_bit(VALID_BIT);  // Valid but no read
  cr_assert(!paging__is_read_enabled(pte), "Entry without read bit should not allow reads");

  // Test with read enabled
  pte = pte | get_mask_single_bit(READ_BIT);
  cr_assert(paging__is_read_enabled(pte), "Entry with read bit should allow reads");
}

Test(Paging, is_write_enabled) {
  // Test with write disabled
  PageTableEntry pte = get_mask_single_bit(VALID_BIT);  // Valid but no write
  cr_assert(!paging__is_write_enabled(pte), "Entry without write bit should not allow writes");

  // Test with write enabled
  pte = pte | get_mask_single_bit(WRITE_BIT);
  cr_assert(paging__is_write_enabled(pte), "Entry with write bit should allow writes");
}

Test(Paging, is_execute_enabled) {
  // Test with execute disabled
  PageTableEntry pte = get_mask_single_bit(VALID_BIT);  // Valid but no execute
  cr_assert(!paging__is_execute_enabled(pte), "Entry without execute bit should not allow execution");

  // Test with execute enabled
  pte = pte | get_mask_single_bit(EXEC_BIT);
  cr_assert(paging__is_execute_enabled(pte), "Entry with execute bit should allow execution");
}

Test(Paging, get_vpn_from_va) {
  // Test with simple virtual addresses where VPN calculation is clear

  // Address 0: VPN should be 0
  VirtualAddress va = 0;
  VPN expected_vpn = 0;
  cr_assert_eq(paging__get_vpn_from_va(va), expected_vpn,
    "VPN of address 0 should be 0");

  // Address equal to page size: VPN should be 1
  va = PAGE_SIZE;  // 1024 bytes = start of page 1
  expected_vpn = 1;
  cr_assert_eq(paging__get_vpn_from_va(va), expected_vpn,
    "VPN of address %d should be 1", PAGE_SIZE);

  // Address in middle of page 2
  va = 2 * PAGE_SIZE + 512;  // Page 2, offset 512
  expected_vpn = 2;
  cr_assert_eq(paging__get_vpn_from_va(va), expected_vpn,
    "VPN of address %d should be 2", va);

  // Test the bit shifting formula directly
  va = 0x12345;  // Arbitrary address
  expected_vpn = va >> NUM_OFFSET_BITS;
  cr_assert_eq(paging__get_vpn_from_va(va), expected_vpn,
    "VPN should equal va >> NUM_OFFSET_BITS");
}
Test(Paging, convert_PageTableEntry_to_PFN) {
  // Test extracting PFN from page table entry with metadata bits

  // Simple case: PFN 5 with all permission bits set
  PFN original_pfn = 5;
  PageTableEntry pte = original_pfn
                     | get_mask_single_bit(VALID_BIT)
                     | get_mask_single_bit(READ_BIT)
                     | get_mask_single_bit(WRITE_BIT)
                     | get_mask_single_bit(EXEC_BIT);

  // Verify metadata bits are present
  cr_assert_neq(pte, original_pfn, "PTE should have metadata bits added");

  // Verify PFN extraction removes metadata
  PFN extracted_pfn = paging__convert_PageTableEntry_to_PFN(pte);
  cr_assert_eq(extracted_pfn, original_pfn,
    "Extracted PFN should match original PFN %d", original_pfn);

  // Test with different PFN value
  original_pfn = 42;
  pte = original_pfn | get_mask_single_bit(VALID_BIT);
  extracted_pfn = paging__convert_PageTableEntry_to_PFN(pte);
  cr_assert_eq(extracted_pfn, original_pfn,
    "Should extract PFN %d correctly", original_pfn);
}

Test(Paging, find_free_page) {
  // Create and initialize MMU for testing
  MMU m = MMU__pagetable__init();

  // Initially, first page should be available
  PFN free_page = paging__find_free_page(&m);
  cr_assert_eq(free_page, 0, "First available page should be 0");

  // Mark first few pages as used
  m.memory_chunk_used[0] = true;
  m.memory_chunk_used[1] = true;
  m.memory_chunk_used[2] = true;

  // Next free page should be 3
  free_page = paging__find_free_page(&m);
  cr_assert_eq(free_page, 3, "Next available page should be 3");
  MMU__destroy(&m);
}

Test(Paging, get_pagetableentry) {
  // Create MMU and set up an address space
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);
  cr_assert_geq(asid, 0, "Should successfully create address space");
  // Set up some test page table entries
  VPN test_vpn = 5;
  PageTableEntry test_entry = 0x1234 | get_mask_single_bit(VALID_BIT);
  m.address_spaces[asid].registers.paging.page_table_pointer[test_vpn] = test_entry;

  // Retrieve the entry
  PageTableEntry retrieved = paging__get_pagetableentry(&m, asid, test_vpn);
  cr_assert_eq(retrieved, test_entry,
    "Should retrieve the correct page table entry");
  
  MMU__destroy(&m);
}
/*
Test(Paging, map_page) {
  // Create MMU and address space
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);
  cr_assert_geq(asid, 0, "Should successfully create address space");

  // Map a page with specific permissions
  VPN vpn_to_map = 10;
  bool can_read = true;
  bool can_write = false;
  bool can_exec = true;

  PFN mapped_pfn = paging__map_page(&m, asid, vpn_to_map, can_read, can_write, can_exec);
  cr_assert_geq(mapped_pfn, 0, "Should successfully map page and return valid PFN");

  // Verify the page table entry was created correctly
  PageTableEntry pte = m.address_spaces[asid].registers.paging.page_table_pointer[vpn_to_map];

  cr_assert(paging__is_entry_valid(pte), "Mapped page should be valid");
  cr_assert_eq(paging__is_read_enabled(pte), can_read, "Read permission should match");
  cr_assert_eq(paging__is_write_enabled(pte), can_write, "Write permission should match");
  cr_assert_eq(paging__is_execute_enabled(pte), can_exec, "Execute permission should match");

  // Verify PFN is correct
  PFN extracted_pfn = paging__convert_PageTableEntry_to_PFN(pte);
  cr_assert_eq(extracted_pfn, mapped_pfn, "PFN in page table should match returned PFN");

  // Verify physical page is marked as used
  cr_assert(m.memory_chunk_used[mapped_pfn],
    "Physical page %d should be marked as used", mapped_pfn);

  MMU__destroy(&m);
}


Test(Paging, create_new_address_space) {
  MMU m = MMU__pagetable__init();

  // Should be able to create an address space
  ASID asid = create_new_address_space__paging(&m);
  cr_assert_geq(asid, 0, "Should return valid ASID");
  cr_assert_lt(asid, MAX_ASIDS, "ASID should be within valid range");

  // Address space should be marked as in use
  cr_assert(m.address_spaces[asid].in_use, "Address space should be marked in use");

  // Page table should be allocated
  cr_assert_neq(m.address_spaces[asid].registers.paging.page_table_pointer, NULL,
    "Page table should be allocated");

  MMU__destroy(&m);
}

Test(Paging, translate_address_basic) {
  // This is a basic test - more complex scenarios in reserve tests
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);

  // Map a page first
  VPN vpn = 0;
  PFN pfn = paging__map_page(&m, asid, vpn, true, true, false);

  // Test translation of address in that page
  VirtualAddress va = 0 * PAGE_SIZE + 100;  // Page 0, offset 100
  PhysicalAddress expected_pa = pfn * PAGE_SIZE + 100;
  PhysicalAddress actual_pa = translate_address__paging(&m, asid, va);

  cr_assert_eq(actual_pa, expected_pa,
    "Physical address should equal (PFN * PAGE_SIZE) + offset");

  MMU__destroy(&m);
}

Test(Paging, is_valid_basic) {
  MMU m = MMU__pagetable__init();
  ASID asid = create_new_address_space__paging(&m);

  // Initially, no pages are mapped, so addresses should be invalid
  VirtualAddress va = 0;
  cr_assert(!paging__is_valid(&m, asid, va), "Unmapped address should be invalid");

  // Map page 0
  VPN vpn = 0;
  paging__map_page(&m, asid, vpn, true, true, false);

  // Now addresses in page 0 should be valid
  cr_assert(paging__is_valid(&m, asid, va), "Mapped address should be valid");

  // But addresses in unmapped pages should still be invalid
  VirtualAddress unmapped_va = 1 * PAGE_SIZE;  // Page 1
  cr_assert(!paging__is_valid(&m, asid, unmapped_va), "Unmapped page should be invalid");

  MMU__destroy(&m);
}

Test(Paging, destroy_address_space) {
  MMU m = MMU__pagetable__init();

  // Create an address space and map some pages
  ASID asid = create_new_address_space__paging(&m);
  cr_assert_geq(asid, 0, "Should successfully create address space");

  // Map a couple of pages to test cleanup
  VPN vpn1 = 3;
  VPN vpn2 = 7;
  PFN pfn1 = paging__map_page(&m, asid, vpn1, true, false, false);
  PFN pfn2 = paging__map_page(&m, asid, vpn2, false, true, true);

  // Verify setup
  cr_assert(m.address_spaces[asid].in_use, "Address space should be in use");
  cr_assert_neq(m.address_spaces[asid].registers.paging.page_table_pointer, NULL,
    "Page table should be allocated");
  cr_assert(m.memory_chunk_used[pfn1], "Physical page 1 should be marked as used");
  cr_assert(m.memory_chunk_used[pfn2], "Physical page 2 should be marked as used");

  // Destroy the address space
  destroy_address_space__paging(&m, asid);

  // Verify cleanup
  cr_assert(!m.address_spaces[asid].in_use, "Address space should no longer be in use");
  cr_assert_eq(m.address_spaces[asid].registers.paging.page_table_pointer, NULL,
    "Page table pointer should be cleared");
  cr_assert(!m.memory_chunk_used[pfn1], "Physical page 1 should be freed");
  cr_assert(!m.memory_chunk_used[pfn2], "Physical page 2 should be freed");

  MMU__destroy(&m);
}*/
