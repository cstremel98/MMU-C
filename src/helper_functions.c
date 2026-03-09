#include "helper_functions.h"

AddressMask get_mask(unsigned int num_bits_to_mask) {
  AddressMask base = 0;
  base = ~base; // Invert all bits so they are all 1s
  return (base >> (NUM_BITS_IN_BYTE*sizeof(AddressBasis) - num_bits_to_mask)); // Shift all bits to the right until we are left with only the mask we want
}

AddressMask get_mask_single_bit(unsigned int bit_to_mask) {
  if (bit_to_mask == 0) {
    return 0;
  }
  if (bit_to_mask == 1) {
    return 1;
  }
  return get_mask(bit_to_mask) - get_mask(bit_to_mask-1);
}

bool is_bit_set(PageTableEntry pte, unsigned int bit_number) {
  AddressBasis mask = get_mask_single_bit(bit_number);
  return (pte & mask) != 0;
}

AddressMask set_bit(PageTableEntry pte, unsigned int bit_number) {
  AddressBasis mask = get_mask_single_bit(bit_number);
  return pte | mask;
}