#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>


#define KBC_ST_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_PAR_ERROR 0x80 // 1000 0000 bit 7
#define KBC_TIMEOUT_ERROR 0x40 // 0100 0000 bit 6
#define IRQ1 1 
#define IRQ_MOUSE 12
#define IRQ_TIMER    0
#define KBC_OBF 0x01  
#define KBC_AUX 0x20
#define DELAY_US 20000 // 20 milliseconds

#define KBC_IN_BUF  0x60  // Input Buffer
#define KBC_CMD_REG 0x64  // Command Register
#define KBC_IBF     0x02  // Input Buffer Full (bit 1)

#define WRITE_BYTE_MOUSE 0xD4
#define ACK 0xFA    
#define NACK 0xFE
#define DISABLE_DATA_REPORTING 0xF5


extern uint32_t sys_inb_count;
extern uint8_t size;
extern uint8_t array_scancodes[2];
extern uint32_t sys_inb_count;
extern uint8_t idle_time;      // Idle time counter (in seconds)

int read_status_register(uint8_t *status);

int read_output_buffer(uint8_t *data);

void handle_scancode(uint8_t scancode);

int subscribe_kbc_interrupts();
int unsubscribe_kbc_interrupts();
void timer_ih();
int enable_keyboard_interrupts();
int read_command_byte(uint8_t *cmd_byte);
int write_command_byte(uint8_t cmd_byte);
int write_kbc_command(uint8_t cmd);
int subscribe_timer_interrupts();
int unsubscribe_timer_interrupts();



#endif
