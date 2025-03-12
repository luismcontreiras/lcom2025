#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#define KBC_ST_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_PAR_ERROR 0x80 // 1000 0000 bit 7
#define KBC_TIMEOUT_ERROR 0x40 // 0100 0000 bit 6


int read_status_register(uint8_t *status){
    return util_sys_inb(KBC_ST_REG, status); 
    // Reads the value at I/O port 0x64 (the Status Register)
    // and then
    // Stores the value in the memory location pointed to by status
}

int read_output_buffer(uint8_t *data){
    return util_sys_inb(KBC_OUT_BUF, data);
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
        print("Comunication error\n");
        return;
    }

    
    if(status & KBC_OUT_BUF){ //Check if the output buffer is full
        uint8_t scancode;

        if(scancode==0xE0){ //2 bytes scancode
            //esperar pelo segundo scancode

        }else{ // single byte scancode

        }
    }

    //libertar OUT_BUF


}


// global array uint8_t[2]


//multiple attemps => ciclo for

//Sleeps no caso ddos milisegundos
