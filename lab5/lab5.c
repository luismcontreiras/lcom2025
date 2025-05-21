// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include "graphics.h"
#include <stdint.h>
#include <stdio.h>

#include "kbc.h"

// Any header files included below this line should have been created by you

extern vbe_mode_info_t mode_info;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  // reg86 to send bios instructions

  if (set_to_video_mode(mode) != 0) {
    printf("Failed to set video mode\n");
    return 1;
  };

  sleep(delay);

  if (vg_exit() != 0) {
    printf("Failed to exit video mode\n");
    return 1;
  }

  printf("video_test_init finished\n");

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  /* To be completed */
  printf("%s(0x%03X, %u, %u, %u, %u, 0x%08x): under construction\n",
         __func__, mode, x, y, width, height, color);

  // map video with frame buffer
  if (set_frame_buffer(mode) != 0)
    return 1;

  // set graphic mode
  if (set_to_video_mode(mode) != 0) {
    printf("Failed to set video mode\n");
    return 1;
  };

  // normalization of color for different color modes
  uint32_t new_color;
  
  if(set_color(color, &new_color) != 0) return 1;

  
  // drwa rectangle (might need to normalize colors for different modes)
  vg_draw_rectangle(x, y, width, height, new_color);
  // use esc to terminate
  // vg_exit();
   wait_for_esc();
  
  if (vg_exit() != 0) return 1;
  //return 1;
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
  printf("%s(0x%03x, %u, 0x%08x, %d): under construction\n", __func__,
         mode, no_rectangles, first, step);

  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t xi, uint16_t yi) {
    printf("Starting video_test_xpm()\n");

    uint16_t mode = 0x105;

     if (set_frame_buffer(mode) != OK) {
        printf("Failed to map frame buffer\n");
        vg_exit();
        return 1;
    }

    if (set_to_video_mode(mode) != 0) {
        printf("Failed to set video mode\n");
        return 1;
    }

    if (vbe_get_mode_info(mode, &mode_info) != 0) {
        printf("Failed to get VBE mode info\n");
        vg_exit();
        return 1;
    }

   
    xpm_image_t img;
    uint8_t* pixmap = xpm_load(xpm, XPM_INDEXED, &img);

    if (pixmap == NULL) {
        printf("Failed to load XPM image\n");
        vg_exit();
        return 1;
    }

    int width = img.width;
    int height = img.height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint8_t color_index = pixmap[y * width + x];

            if (color_index != 0) { 
                vg_draw_pixel(xi + x, yi + y, color_index);
            }
        }
    }

    // Step 6: Wait for ESC key
    wait_for_esc();

    // Step 7: Clean up
    free(pixmap);
    if (vg_exit() != 0) {
        printf("Failed to exit graphics mode\n");
        return 1;
    }

    printf("video_test_xpm finished successfully\n");
    return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* This year you do not need to implement this */
  printf("%s(): under construction\n", __func__);

  return 1;
}
