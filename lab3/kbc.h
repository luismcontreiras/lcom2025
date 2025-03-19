#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>

#define KBC_ST_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_PAR_ERROR 0x80 // 1000 0000 bit 7
#define KBC_TIMEOUT_ERROR 0x40 // 0100 0000 bit 6
#define IRQ1 1 
#define KBC_OBF 0x01  

extern uint8_t size;
extern uint8_t array_scancodes[2];
extern uint32_t sys_inb_count;

int read_status_register(uint8_t *status);

int read_output_buffer(uint8_t *data);

void handle_scancode(uint8_t scancode);


int subscribe_kbc_interrupts();
int unsubscribe_kbc_interrupts();


#endif
