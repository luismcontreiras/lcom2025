#ifndef ENGINE_TIMER_H_INCLUDED
#define ENGINE_TIMER_H_INCLUDED

#include <lcom/lcf.h>
#include "i8254.h"

/**
 * @brief Subscribe timer interrupts
 * 
 * @param bit_no Address of memory to be initialized with the bit number to be set in the mask
 * @return Return 0 upon success and non-zero otherwise
 */
int engine_timer_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int engine_timer_unsubscribe_int();

/**
 * @brief Timer interrupt handler
 */
void engine_timer_int_handler();

/**
 * @brief Set timer frequency
 * 
 * @param timer Timer to configure
 * @param freq Frequency to set
 * @return Return 0 upon success and non-zero otherwise
 */
int engine_timer_set_frequency(uint8_t timer, uint32_t freq);

#endif /* ENGINE_TIMER_H_INCLUDED */
