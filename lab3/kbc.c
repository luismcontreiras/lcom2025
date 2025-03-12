#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "kbc.h"





static u_int8_t array_scancodes[2];
uint8_t size = 0;


int read_status_register(uint8_t *status){
    return util_sys_inb(KBC_ST_REG, status); 
    // Reads the value at I/O port 0x64 (the Status Register)
    // and then
    // Stores the value in the memory location pointed to by status
}

int read_output_buffer(uint8_t *data){
    return util_sys_inb(KBC_OUT_BUF, data);
}


void handler_scancode(uint8_t scancode){
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

    if(size>0 & ((size==1)||(size==2))){
        bool is_break_code = (array_scancodes[0] & 0x80);
        bool is_make_code = !(array_scancodes[0] & 0x80);
        
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
        print("Comunication error\n");
        return;
    }
    //msm q haja erro temos q libertar o OUT_BUF descartar o sys_inb

    
    
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


}


// global array uint8_t[2]


//multiple attemps => ciclo for

//Sleeps no caso ddos milisegundos
