// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include "graphics.h"
#include <stdint.h>
#include <stdio.h>

#include "kbc.h"
#include "timer.c"

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

  if (set_color(color, &new_color) != 0)
    return 1;

  // drwa rectangle (might need to normalize colors for different modes)
  vg_draw_rectangle(x, y, width, height, new_color);
  // use esc to terminate
  // vg_exit();
  wait_for_esc();

  if (vg_exit() != 0)
    return 1;
  // return 1;
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  if (set_frame_buffer(mode) != 0)
    return 1;

  if (set_to_video_mode(mode) != 0) {
    printf("Failed to set video mode\n");
    return 1;
  }

  uint16_t width = mode_info.XResolution / no_rectangles;
  uint16_t height = mode_info.YResolution / no_rectangles;

  for (uint8_t row = 0; row < no_rectangles; row++) {
    for (uint8_t col = 0; col < no_rectangles; col++) {
      uint32_t color;

      // DIrect color mode
      if (mode_info.MemoryModel == 0x06) {
        // Color formula

        uint32_t r_first = (first >> mode_info.RedFieldPosition) & ((1 << mode_info.RedMaskSize) - 1);
        uint32_t g_first = (first >> mode_info.GreenFieldPosition) & ((1 << mode_info.GreenMaskSize) - 1);
        uint32_t b_first = (first >> mode_info.BlueFieldPosition) & ((1 << mode_info.BlueMaskSize) - 1);

        uint32_t r = (r_first + col * step) % (1 << mode_info.RedMaskSize);
        uint32_t g = (g_first + row * step) % (1 << mode_info.GreenMaskSize);
        uint32_t b = (b_first + (col + row) * step) % (1 << mode_info.BlueMaskSize);

        color = (r << mode_info.RedFieldPosition) |
                (g << mode_info.GreenFieldPosition) |
                (b << mode_info.BlueFieldPosition);
      }
      // Other color modes
      else {

        color = (first + (row * no_rectangles + col) * step) % (1 << mode_info.BitsPerPixel);
      }

      // Draw rectangle with color from function
      if (vg_draw_rectangle(col * width, row * height, width, height, color) != 0) {
        vg_exit();
        return 1;
      }
    }
  }

  if (wait_for_esc() != 0) {
    vg_exit();
    return 1;
  }

  if (vg_exit() != 0)
    return 1;

  return 0;
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

  draw_xpm(xi, yi, xpm);

  // Step 6: Wait for ESC key
  wait_for_esc();

  // Step 7: Clean up

  if (vg_exit() != 0) {
    printf("Failed to exit graphics mode\n");
    return 1;
  }

  printf("video_test_xpm finished successfully\n");
  return 0;
}

// int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
//                      int16_t speed, uint8_t fr_rate) {

//   int ipc_status, r;
//   message msg;
  
//   uint16_t mode = 0x105;
//   uint8_t timer_bit_no, kbc_bit_no;

//   // Subscribe to keyboard interrupts

//   if (subscribe_kbc_interrupts(&kbc_bit_no) != 0) {
//     printf("Failed to subscribe to keyboard interrupts\n");
//     return 1;
//   }

//   if (timer_subscribe_int(&timer_bit_no) != 0) {
//     printf("Failed to subscribe to timer interrupts\n");
//     return 1;
//   }

//   if (timer_set_frequency(0, fr_rate) != 0) {
//     printf("Failed to set timer frequency\n");
//     return 1;
//   }

//   if (set_frame_buffer(mode) != 0) {
//     printf("Failed to map frame buffer\n");
//     vg_exit();
//     return 1;
//   }

//   if (set_to_video_mode(mode) != 0) {
//     printf("Failed to set video mode\n");
//     return 1;
//   }

//   if (draw_xpm(xi, yi, xpm) != 0) {
//     printf("Failed to draw xpm");
//     return 1;
//   };

//   while (array_scancodes[0] != 0x81 && (xi < xf || yi < yf)) {
//     // Get a request message
//     if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
//       printf("driver_receive failed with: %d", r);
//       continue;
//     }

//     if (is_ipc_notify(ipc_status)) {
//       switch (_ENDPOINT_P(msg.m_source)) {
//         case HARDWARE:
//           if (msg.m_notify.interrupts & BIT(1)) {
//             kbc_ih();
//           }
//           if (msg.m_notify.interrupts & BIT(0)) {
//             timer_int_handler();

//             if (vg_draw_rectangle(xi, yi, 100, 100, 0xFFFFFF) != 0) {
//               printf("Failed at screen clean\n");
//               return 1;
//             }

//             if (xi < xf) {
//               xi += speed;
//               if (xi > xf)
//                 xi = xf;
//             }
//             if (yi < yf) {
//               yi += speed;
//               if (yi > yf)
//                 yi = yf;
//             }

//             if (draw_xpm(xi, yi, xpm) != 0) {
//               printf("Failed to draw xpm");
//               return 1;
//             };
//           }       
//       }
//     }
//   }

//    if (vg_exit() != 0) {
//     printf("Failed to exit graphics mode\n");
//     return 1;
//   }

//   if (timer_unsubscribe_int() != 0) {
//     printf("Failed to unsubscribe timer\n");
//     return 1;
//   }

//   if (unsubscribe_kbc_interrupts() != 0) {
//     printf("Failed to unsubscribe keyboard\n");
//     return 1;
//   }

 

//   printf("video_test_move finished successfully\n");
//   return 0;
// }


int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  int ipc_status, r;
  message msg;
  
  uint16_t mode = 0x105;
  uint8_t timer_bit_no;
  
  // Current sprite position
  uint16_t curr_x = xi;
  uint16_t curr_y = yi;
  
  // Assume sprite dimensions (you may need to adjust based on your XPM)
  uint16_t sprite_width = 100;  // Adjust based on your sprite
  uint16_t sprite_height = 100; // Adjust based on your sprite

  // Subscribe to keyboard interrupts
  if (subscribe_kbc_interrupts() != 0) {
    printf("Failed to subscribe to keyboard interrupts\n");
    return 1;
  }

  if (timer_subscribe_int(&timer_bit_no) != 0) {
    printf("Failed to subscribe to timer interrupts\n");
    return 1;
  }

  if (timer_set_frequency(0, fr_rate) != 0) {
    printf("Failed to set timer frequency\n");
    return 1;
  }

  if (set_frame_buffer(mode) != 0) {
    printf("Failed to map frame buffer\n");
    vg_exit();
    return 1;
  }

  if (set_to_video_mode(mode) != 0) {
    printf("Failed to set video mode\n");
    return 1;
  }

  if (draw_xpm(curr_x, curr_y, xpm) != 0) {
    printf("Failed to draw xpm");
    return 1;
  };

  while (last_scancode != ESC_SCANCODE) {
    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(1)) {
            kbc_ih();
            
            // Handle arrow key movement if a new scancode is ready
            if (new_scancode_ready) {
              uint16_t new_x = curr_x;
              uint16_t new_y = curr_y;
              
              switch (last_scancode) {
                case ARROW_UP_SCANCODE:
                  if (curr_y >= speed) {
                    new_y = curr_y - speed;
                  }
                  break;
                case ARROW_DOWN_SCANCODE:
                  if (curr_y + sprite_height + speed <= mode_info.YResolution) {
                    new_y = curr_y + speed;
                  }
                  break;
                case ARROW_LEFT_SCANCODE:
                  if (curr_x >= speed) {
                    new_x = curr_x - speed;
                  }
                  break;
                case ARROW_RIGHT_SCANCODE:
                  if (curr_x + sprite_width + speed <= mode_info.XResolution) {
                    new_x = curr_x + speed;
                  }
                  break;
              }
              
              // Only redraw if position changed
              if (new_x != curr_x || new_y != curr_y) {
                // Clear old position with black
                if (vg_draw_rectangle(curr_x, curr_y, sprite_width, sprite_height, 0x000000) != 0) {
                  printf("Failed to clear old sprite position\n");
                  return 1;
                }
                
                // Update position
                curr_x = new_x;
                curr_y = new_y;
                
                // Draw sprite at new position
                if (draw_xpm(curr_x, curr_y, xpm) != 0) {
                  printf("Failed to draw xpm");
                  return 1;
                }
              }
              
              // Reset the flag
              new_scancode_ready = false;
            }
          }
          if (msg.m_notify.interrupts & BIT(0)) {
            timer_int_handler();
          }       
      }
    }
  }

   if (vg_exit() != 0) {
    printf("Failed to exit graphics mode\n");
    return 1;
  }

  if (timer_unsubscribe_int() != 0) {
    printf("Failed to unsubscribe timer\n");
    return 1;
  }

  if (unsubscribe_kbc_interrupts() != 0) {
    printf("Failed to unsubscribe keyboard\n");
    return 1;
  }

 

  printf("video_test_move finished successfully\n");
  return 0;
}

int(video_test_controller)() {
  /* This year you do not need to implement this */
  printf("%s(): under construction\n", __func__);

  return 1;
}
