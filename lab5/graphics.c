#include <lcom/lcf.h>
#include "graphics.h"

int (set_to_video_mode) (uint16_t submode){
    reg86_t reg86;
    //set up with 0s
    memset(&reg86, 0, sizeof(reg86));

    reg86.intno = 0x10;

    //setting to video (0x0003 for text)
    reg86.ax = 0x4F02;
    //BIT 14 is linear mem mapping
    reg86.bx = submode | BIT(14);

    if(sys_int86(&reg86) != 0){
        printf("sys_int86 error");
        return 1;
    }
    return 0;
}

int (set_to_text_mode) (){
    reg86_t reg86;
    //set up with 0s
    memset(&reg86, 0, sizeof(reg86));



    //setting to video (0x0003 for text)
    reg86.ah = 0x00;
    reg86.al = 0x03;
    //reg86.ax = 0x0003;
    //BIT 14 is linear mem mapping
    reg86.bx = 0x0000;

    reg86.intno = 0x10;

    if(sys_int86(&reg86) != 0){
        printf("sys_int86 error");
        return 1;
    }
    return 0;
}
