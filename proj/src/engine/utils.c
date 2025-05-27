#include <lcom/lcf.h>

#include <stdint.h>
#include "kbc.h"
#include "utils.h"

#ifdef LAB3
unsigned int sys_inb_count = 0;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb)
{
  /*
    if lsb is valid -> get lowest 8 bits
    if lsb is not valid (ex.: nullpointer) returns 1
   */
  return lsb ? (*lsb = val & 0xFF, 0) : 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb)
{
  /*
    if msb is valid -> msb gets 8 highest bits
    if msb is not valid (nullpointer, etc) returns 1
   */
  return msb ? (*msb = (val >> 8) & 0xFF, 0) : 1;
}

int(util_sys_inb)(int port, uint8_t *value)
{
  /*
   create a new value to be used, that is 32 bits long and
   can be sent to sys_inb
   */
  uint32_t val32;
  int ret = sys_inb(port, &val32);
  /*
   The 32 bit value is passed by reference.
   we only need the lesser 8 bits
   */
  *value = 0xFF & val32;

  #ifdef LAB3
    sys_inb_count++; // Increment the counter only if LAB3 is defined
  #endif
  /*
   give the same return as the original sys_inb
   */
  return ret;
}
