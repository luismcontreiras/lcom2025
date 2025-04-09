#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "kbc.h"


int kbc_hook_id = 1;
u_int8_t array_scancodes[2];
uint32_t sys_inb_count = 0;
uint8_t size = 0;
uint8_t status;
static uint8_t idle_time = 0;      // Idle time counter (in seconds)



// Subscribe to KBC interrupts
int subscribe_kbc_interrupts() {
    return sys_irqsetpolicy(IRQ1, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id);
}

// Unsubscribe from KBC interrupts
int unsubscribe_kbc_interrupts() {
    return sys_irqrmpolicy(&kbc_hook_id);
}

int read_status_register(uint8_t *status){
    return util_sys_inb(KBC_ST_REG, status); 
    // Reads the value at I/O port 0x64 (the Status Register)
    // and then
    // Stores the value in the memory location pointed to by status
}

int read_output_buffer(uint8_t *data){
    return util_sys_inb(KBC_OUT_BUF, data);
}


void handle_scancode(uint8_t scancode){
    if(scancode==0xE0){ //2 bytes scancode
        
        array_scancodes[0] = scancode;
        size=1;
    }else{ // single byte scancode
        if(size==1){
            array_scancodes[1]=scancode;
            size=2;
        }else{
            array_scancodes[0]=scancode;
            size=1;
        }
    }

    if ((size > 0) && ((size == 1) || (size == 2))) {
        bool is_make_code = !(array_scancodes[0] & 0x80);
        kbd_print_scancode(is_make_code, size, array_scancodes);
        size=0;
    }


}


void(kbc_ih)() {

    //read the status register 
    // pag 5 do lab3  
    if(read_status_register(&status)!=0){
        printf("Error reading status register\n");
        return;
    }

    // check if there was some communications error
    if(status & (KBC_PAR_ERROR | KBC_TIMEOUT_ERROR)){
        printf("Comunication error\n");
        return;
    }
    //msm q haja erro temos q libertar o OUT_BUF descartar o sys_inb

    

    if (status & KBC_OBF) {
        uint8_t scancode;

        // Read the scancode from the output buffer
        if (read_output_buffer(&scancode) != OK) {
            printf("Error reading output buffer\n");
            return;
        }

        //Pass the scancode to the scancode handler
        handle_scancode(scancode);
    }
    idle_time = 0;

}

void timer_ih() {
    idle_time++; // Increment idle time counter
}
    

int enable_keyboard_interrupts() {
    uint8_t cmd_byte;

    // Read the current command byte
    if (read_command_byte(&cmd_byte) != OK) {
        return 1; // Error
    }

    // Set bit 0 to enable keyboard interrupts
    cmd_byte |= 0x01;

    // Write the updated command byte back to the KBC
    if (write_command_byte(cmd_byte) != OK) {
        return 1; // Error
    }

    return 0; // Success
}   


int read_command_byte(uint8_t *cmd_byte) {
    // Issue the "Read Command Byte" command (0x20)
    if (write_kbc_command(0x20) != OK) {
        return 1; // Error
    }

    // Read the command byte from the output buffer
    if (read_output_buffer(cmd_byte) != OK) {
        return 1; // Error
    }

    return 0; // Success
}

int write_command_byte(uint8_t cmd_byte) {
    // Issue the "Write Command Byte" command (0x60)
    if (write_kbc_command(0x60) != OK) {
        return 1; // Error
    }

    // Write the new command byte to the input buffer
    if (sys_outb(KBC_IN_BUF, cmd_byte) != OK) {
        return 1; // Error
    }

    return 0; // Success
}

int write_kbc_command(uint8_t cmd) {
    uint8_t status;

    // Wait until the input buffer is not full
    while (true) {
        if (read_status_register(&status) != OK) {
            return 1; // Error
        }
        if (!(status & KBC_IBF)) {
            break;
        }
        tickdelay(micros_to_ticks(DELAY_US)); // Delay to avoid busy-waiting
    }

    // Write the command to the KBC command register
    if (sys_outb(KBC_CMD_REG, cmd) != OK) {
        return 1; // Error
    }

    return 0; // Success
}

    /*
    if(status & KBC_OUT_BUF){ //Check if the output buffer is full
        uint8_t scancode;

        if(scancode==0xE0){ //2 bytes scancode
            //esperar pelo segundo scancode

        }else{ // single byte scancode

        }
    }

    //fazer um prinf %x 
    //scancode handler

    //libertar OUT_BUF
*/


// global array uint8_t[2]


//multiple attemps => ciclo for

//Sleeps no caso ddos milisegundos
