#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int timer_hook_id = 0;
int counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{
  // gets the configuration info on st
  uint8_t config;
  if (timer_get_conf(timer, &config) != 0)
    return 1;
  // prepare the same config to receive new values (LSB and MSB)
  config = (config & 0x0F) | TIMER_LSB_MSB;

  // create the new value based on the given frequency
  uint32_t val = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  if (util_get_LSB(val, &lsb) != 0)
    return 1;
  if (util_get_MSB(val, &msb) != 0)
    return 1;

  // tells the timer we will send the lsb and msb
  if (sys_outb(TIMER_CTRL, config) != 0)
    return 1;

  // order matters
  // sends the lsb and the msb
  if (sys_outb(TIMER_0 + timer, lsb) != 0)
    return 1;
  if (sys_outb(TIMER_0 + timer, msb))
    return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no)
{
  *bit_no = BIT(timer_hook_id);
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int(timer_unsubscribe_int)()
{
  return sys_irqrmpolicy(&timer_hook_id);
}

void(timer_int_handler)()
{
  counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st)
{
  // construct readback
  uint8_t RBC = (TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
  // send readback "request"
  if (sys_outb(TIMER_CTRL, RBC) != 0)
    return 1;
  // read readback return
  if (util_sys_inb(TIMER_0 + timer, st))
    return 1;
  return 0;
}

/*
st is a binary representing the status byte

the status byte is divided as

[7]     - Output
[6]     - Null Count
[5,4]   - Type of Access
[3,2,1] - Programmed Mode
[0]     - BCD ("bool"
0 0 00 000 0
*/

//

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{

  union timer_status_field_val val;

  // all
  /* The status byte */
  if (field == tsf_all)
  {
    val.byte = st;
  }

  /* The initialization mode */
  // type of access (bits 5,4)

  else if (field == tsf_initial)
  {

    // 11[11] 0000
    //
    // 0000 1111
    //
    // 0000 0011 - &
    // 0000 0011
    st = (st >> 4) & 0x03;
    if (st >= 1 && st <= 3)
      val.in_mode = st;
    else
      (val.in_mode = 0);
  }
  // mode
  /* The counting mode: 0, 1,.., 5 */
  else if (field == tsf_mode)
  {

    st = (st >> 1) & 0x07;

    // 0000 1100
    // 0000 0110

    // 0000 0110
    // 0000 0111

    // 0000 0110 - 6

    if (st == 6)
      val.count_mode = 2;
    else if (st == 7)
      val.count_mode = 3;
    else
      val.count_mode = st;
  }
  // base
  /* The counting base, true if BCD */
  else if (field == tsf_base)
  {
    // bit 0
    st = st & 0x01;
    val.bcd = st;
  }

  timer_print_config(timer, field, val);

  return 1;
}
