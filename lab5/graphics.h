//
// Created by framoz on 4/30/25.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <lcom/lcf.h>


vbe_mode_info_t mode_info;

int(set_to_video_mode)(uint16_t submode);
int(set_to_text_mode)();
int(set_frame_buffer)(uint16_t mode);
int(set_color)(uint32_t color, uint32_t *new_color);


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

#endif // GRAPHICS_H
