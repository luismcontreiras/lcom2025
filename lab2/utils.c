#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  return lsb ? (*lsb = val & 0xFF, 0) : 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  return msb ? (*msb = val >> 8, 0) : 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t val32;
  int ret = sys_inb(port,&val32);
  *value = 0xFF & val32;
  return ret;
}
