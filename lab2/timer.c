#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_subscribe_int)(uint8_t *bit_no)
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_unsubscribe_int)()
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void(timer_int_handler)()
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
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

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{

  union timer_status_field_val val;

  // all
  /* The status byte */
  if (field == 0)
  {
    val.byte = st;
   
  }

  /* The initialization mode */
  // type of access (bits 5,4)

  else if (field == 1)
  { 
    
    //11[11] 0000
    //
    //0000 1111
    //
    //0000 0011 - &
    //0000 0011
    st = (st >> 4) & 0x03;
    if(st>= 1 && st <= 3) val.in_mode = st;
    else(val.in_mode = 0);
    
  }
  // mode
  /* The counting mode: 0, 1,.., 5 */
  else if (field == 2)
  {
    
    st = (st >> 1) & 0x07;
    
    //0000 1100
    //0000 0110

    //0000 0110    
    //0000 0111

    //0000 0110 - 6

    if(st == 6) val.count_mode = 2;
    else if(st == 7) val.count_mode = 3;
    else val.count_mode = st;
    
  }
  // base
  /* The counting base, true if BCD */
  else if (field == 3)
  {
    //bit 0
    st = st & 0x01;
    val.bcd = st;
  }

  timer_print_config(timer, field, val);

  return 1;
}
