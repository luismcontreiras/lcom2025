//
// Created by framoz on 4/30/25.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <lcom/lcf.h>


vbe_mode_info_t mode_info;
extern uint8_t *video_mem;  // Make video memory accessible

int(set_to_video_mode)(uint16_t submode);
int(set_to_text_mode)();
int(set_frame_buffer)(uint16_t mode);
int(set_color)(uint32_t color, uint32_t *new_color);


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

// Double buffering functions
int (vg_draw_pixel_buffer)(uint8_t *buffer, uint16_t x, uint16_t y, uint32_t color, uint16_t screen_width, uint8_t bytes_per_pixel);
int (vg_draw_rectangle_buffer)(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint16_t screen_width, uint8_t bytes_per_pixel);
int (vg_clear_buffer)(uint8_t *buffer, uint32_t size, uint32_t color, uint8_t bytes_per_pixel);
int (vg_copy_buffer)(uint8_t *dest, uint8_t *src, uint32_t size);

int (draw_xpm)(uint16_t xi, uint16_t yi,xpm_map_t xpm);

#endif // GRAPHICS_H
