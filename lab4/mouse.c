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
  
void (mouse_ih)(){
    read_output_buffer(&c_byte);
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
    pp.delta_x = (int8_t)pp.bytes[1];
    pp.delta_y = (int8_t)pp.bytes[2];
}

int (mouse_write)(uint8_t cmd) {
    uint8_t attempts = 10;
    uint8_t status;

    do {
        attempts--;
        if (write_kbc_command(KBC_CMD_REG,WRITE_BYTE_MOUSE) != 0) return 1;
        if (write_kbc_command(KBC_IN_BUF, cmd) != 0) return 1;
        tickdelay(micros_to_ticks(20000));
        if (util_sys_inb(KBC_OUT_BUF, &status) != 0) return 1;
        if (status == ACK) return 0;
    } while (status != ACK && attempts > 0);

    return 1;
}
