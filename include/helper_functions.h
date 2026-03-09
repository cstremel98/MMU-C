#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <stdbool.h>

#include "defines.h"

/**
 * (Helper Function) Gets a mask with the given number of bits set and all others unset.
 * For example, `get_mask(2)` would return 0b11
 * @param num_bits_to_mask
 * @return
 */
AddressMask get_mask(unsigned int num_bits_to_mask);

/**
 * (Helper Function) Get a mask with only a single bit set.
 * For example, `get_mask_single_bit(2)` would return 0b10
 * @param bit_to_mask
 * @return
 */
AddressMask get_mask_single_bit(unsigned int bit_to_mask);

/**
 * (Helper Function) Query whether a particular bit is set.
 * For example, `is_bit_set(0b10, 0)` would be false but `is_bit_set(0b10, 1)` would be true.
 * @param pte
 * @param bit_number
 * @return
 */
bool is_bit_set(PageTableEntry pte, unsigned int bit_number);

/**
 * (Helper Function) Sets the requested bit
 * For example, `set_bit(0b10, 0)` would result in 0b11
 * @param pte
 * @param bit_number
 * @return
 */
AddressMask set_bit(PageTableEntry pte, unsigned int bit_number);

#endif //HELPER_FUNCTIONS_H