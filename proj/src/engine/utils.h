#ifndef _UTILS_H_
#define _UTILS_H_

#include <lcom/lcf.h>
#include <stdint.h>

/**
 * @brief Gets the least significant byte (LSB) of a 16-bit value.
 *
 * @param val The 16-bit value
 * @param lsb Pointer to store the LSB
 * @return 0 on success, 1 otherwise (e.g., if lsb is NULL)
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Gets the most significant byte (MSB) of a 16-bit value.
 *
 * @param val The 16-bit value
 * @param msb Pointer to store the MSB
 * @return 0 on success, 1 otherwise (e.g., if msb is NULL)
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Reads a byte from a specified port.
 *        Wrapper to sys_inb() that handles conversion to 8 bits.
 *
 * @param port The port to read from
 * @param value Pointer to store the read value
 * @return The success or failure of sys_inb()
 */
int(util_sys_inb)(int port, uint8_t *value);

#ifdef LAB3
extern unsigned int sys_inb_count; // Counter for LAB3
#endif

#endif /* _UTILS_H_ */
