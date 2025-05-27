#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>

#define KBC_ST_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_PAR_ERROR 0x80 // 1000 0000 bit 7
#define KBC_TIMEOUT_ERROR 0x40 // 0100 0000 bit 6
#define IRQ1 1 
#define IRQ_TIMER    0
#define KBC_OBF 0x01  
#define KBC_AUX 0x20
#define DELAY_US 20000 // 20 milliseconds

#define KBC_IN_BUF  0x60  // Input Buffer
#define KBC_CMD_REG 0x64  // Command Register
#define KBC_IBF     0x02  // Input Buffer Full (bit 1)

// Arrow key scancodes
#define ARROW_UP_SCANCODE    0x48
#define ARROW_DOWN_SCANCODE  0x50
#define ARROW_LEFT_SCANCODE  0x4B
#define ARROW_RIGHT_SCANCODE 0x4D
#define ESC_SCANCODE         0x81


extern uint32_t sys_inb_count;
extern uint8_t size;
extern uint8_t array_scancodes[2];
extern uint8_t idle_time;      // Idle time counter (in seconds)
extern uint8_t last_scancode;  // Last processed scancode for movement detection
extern bool new_scancode_ready; // Flag indicating a new scancode was processed

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
int wait_for_esc();



#endif
