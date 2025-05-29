#include "mouse.h"

int mouse_hook_id = 2;
uint8_t byte_index = 0;
uint8_t array[3];
uint8_t c_byte;
struct packet pp;

int (mouse_subscribe_int)(uint8_t *bit_no) {
    *bit_no = BIT(mouse_hook_id);
    return sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
  }
  
int (mouse_unsubscribe_int)() {
return sys_irqrmpolicy(&mouse_hook_id);
}
  
void (mouse_ih)() {
    uint8_t status;
    
    // Read the KBC status register
    if (read_status_register(&status) != 0) {
        printf("Error reading status register\n");
        return;
    }
    
    // Check for communication errors
    if (status & (KBC_PAR_ERROR | KBC_TIMEOUT_ERROR)) {
        printf("Mouse communication error\n");
        return;
    }
    
    // Check if output buffer is full AND the data is from the mouse (AUX bit set)
    if ((status & KBC_OBF) && (status & KBC_AUX)) {
        if (read_output_buffer(&c_byte) != 0) {
            printf("Error reading from output buffer\n");
            return;
        }
    } else {
        // No mouse data available or not for us
        return;
    }
}

void (mouse_sync_bytes)() {
    if (byte_index == 0) {
        array[0] = c_byte;
        byte_index++;
    }
    else if (byte_index > 0) {
        array[byte_index] = c_byte;
        byte_index++;
    }
}

void (mouse_bytes_to_packet)() {
    pp.bytes[0] = array[0];
    pp.bytes[1] = array[1];
    pp.bytes[2] = array[2];
    pp.lb = (pp.bytes[0] & BIT(0)) >> 0;
    pp.rb = (pp.bytes[0] & BIT(1)) >> 1;
    pp.mb = (pp.bytes[0] & BIT(2)) >> 2;
    pp.x_ov = (pp.bytes[0] & BIT(6)) >> 6;
    pp.y_ov = (pp.bytes[0] & BIT(7)) >> 7;

    // Correct sign extension for delta_x
    if (pp.bytes[0] & BIT(4))
        pp.delta_x = (int16_t)pp.bytes[1] | 0xFF00;
    else
        pp.delta_x = (int16_t)pp.bytes[1] & 0x00FF;

    // Correct sign extension for delta_y
    if (pp.bytes[0] & BIT(5))
        pp.delta_y = (int16_t)pp.bytes[2] | 0xFF00;
    else
        pp.delta_y = (int16_t)pp.bytes[2] & 0x00FF;
}

int (mouse_write)(uint8_t cmd) {
    uint8_t attempts = 10;
    uint8_t status;

    // Temporarily disable mouse interrupts
    if (sys_irqdisable(&mouse_hook_id) != 0) return 1;

    do {
        attempts--;
        if (write_kbc_command_port(KBC_CMD_REG, WRITE_BYTE_MOUSE) != 0) {
            sys_irqenable(&mouse_hook_id); // Re-enable before returning
            return 1;
        }
        if (write_kbc_command_port(KBC_IN_BUF, cmd) != 0) {
            sys_irqenable(&mouse_hook_id); // Re-enable before returning
            return 1;
        }
        tickdelay(micros_to_ticks(20000));
        if (util_sys_inb(KBC_OUT_BUF, &status) != 0) {
            sys_irqenable(&mouse_hook_id); // Re-enable before returning
            return 1;
        }
        if (status == ACK) {
            // Re-enable mouse interrupts before returning
            sys_irqenable(&mouse_hook_id);
            return 0;
        }
    } while (status != ACK && attempts > 0);

    // Re-enable mouse interrupts before returning
    sys_irqenable(&mouse_hook_id);
    return 1;
}

