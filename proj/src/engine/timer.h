#ifndef _TIMER_H_
#define _TIMER_H_

#include <lcom/lcf.h>
#include <stdint.h>

#include "i8254.h"

/** @defgroup timer Timer
 * @{
 *
 * Functions for using the timer
 */

/**
 * @brief Global variable for the timer hook id
 */
extern int hook_id_timer;

/**
 * @brief Global counter for timer interrupts
 */
extern int counter;

/**
 * @brief Changes the operating frequency of a timer
 * 
 * Must use the read-back command so that it does not change 
 * the 4 LSBs of the timer's control word.
 * 
 * @param timer Timer to configure (0, 1, or 2)
 * @param freq Frequency to set for the timer
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_set_frequency)(uint8_t timer, uint32_t freq);

/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_unsubscribe_int)();

/**
 * @brief Timer 0 interrupt handler
 *
 * Increments the counter variable on each interrupt
 */
void(timer_int_handler)();

/**
 * @brief Reads the configuration of the specified timer
 *
 * @param timer Timer whose configuration to read (0, 1, or 2)
 * @param st    Address of memory position to be filled with the timer configuration
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_get_conf)(uint8_t timer, uint8_t *st);

/**
 * @brief Displays timer configuration
 * 
 * Displays, in a human-friendly way, the configuration of a timer
 * based on the specified field.
 * 
 * @param timer Timer whose configuration to display (0, 1, or 2)
 * @param st    Status byte with the timer configuration
 * @param field Specifies the timer status field to display
 * @return Return 0 upon success and non-zero otherwise
 */
int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);

/**@}*/

#endif /* _TIMER_H_ */
