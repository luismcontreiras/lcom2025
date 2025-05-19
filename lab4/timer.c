#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id_timer = 0;
int counter = 0;

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{
  uint8_t config;
  if (timer_get_conf(timer, &config) != 0)
    return 1;

  uint8_t timer_sel;
  switch (timer) {
    case 0: timer_sel = TIMER_SEL0; break;
    case 1: timer_sel = TIMER_SEL1; break;
    case 2: timer_sel = TIMER_SEL2; break;
    default: return 1;
  }

  // Preserve mode and BCD bits, set correct timer and access mode
  uint8_t control = timer_sel | TIMER_LSB_MSB | (config & 0x0F);

  uint32_t val = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  if (util_get_LSB(val, &lsb) != 0)
    return 1;
  if (util_get_MSB(val, &msb) != 0)
    return 1;

  if (sys_outb(TIMER_CTRL, control) != 0)
    return 1;
  if (sys_outb(TIMER_0 + timer, lsb) != 0)
    return 1;
  if (sys_outb(TIMER_0 + timer, msb) != 0)
    return 1;

  return 0;
}

int(timer_subscribe_int)(uint8_t *bit_no)
{
  *bit_no = BIT(hook_id_timer);
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer);
}

int(timer_unsubscribe_int)()
{
  return sys_irqrmpolicy(&hook_id_timer);
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
