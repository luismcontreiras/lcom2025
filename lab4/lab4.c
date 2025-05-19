// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include <stdio.h>
#include "i8254.h"
#include "kbc.h"
#include "mouse.h"

// Any header files included below this line should have been created by you


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  //lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  //lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status, r;
  message msg;
  uint8_t mouse_mask;
  
  // Add these extern declarations to access variables from mouse.c
  extern uint8_t byte_index;
  extern struct packet pp;
  
  if (mouse_subscribe_int(&mouse_mask) != 0) return 1;
  if (mouse_write(ENABLE_DATA_REPORTING) != 0) {
      mouse_unsubscribe_int();
      return 1;
  }
  
  uint32_t packets_processed = 0;
  
  while (packets_processed < cnt) {
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
          printf("driver_receive failed with: %d", r);
          continue;
      }

      if (is_ipc_notify(ipc_status)) {
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE:
                  if (msg.m_notify.interrupts & mouse_mask) {
                      mouse_ih();
                      // Synchronize bytes into a packet
                      mouse_sync_bytes();
                      
                      // If we've collected all 3 bytes of a packet
                      if (byte_index == 3) {
                          byte_index = 0; // Reset for next packet
                          
                          // Convert bytes to a packet structure
                          mouse_bytes_to_packet();
                          
                          // Display the packet
                          mouse_print_packet(&pp);
                          
                          // Count this packet
                          packets_processed++;
                      }
                  }
                  break;
              default:
                  break;
          }
      }
  }
  
  // Desativar o report de dados do rato
  if (mouse_write(DISABLE_DATA_REPORTING) != 0) return 1;

  // Desativar as interrupções
  if (mouse_unsubscribe_int() != 0) return 1;
  
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    int ipc_status, r;
    message msg;
    uint8_t mouse_mask, timer_mask;
    int timer_counter = 0;
    int hz = sys_hz();

    // Externs for mouse packet handling
    extern uint8_t byte_index;
    extern struct packet pp;

    // Subscribe mouse interrupts
    if (mouse_subscribe_int(&mouse_mask) != 0) return 1;
    // Subscribe timer interrupts
    if (timer_subscribe_int(&timer_mask) != 0) {
        mouse_unsubscribe_int();
        return 1;
    }
    // Enable mouse data reporting
    if (mouse_write(ENABLE_DATA_REPORTING) != 0) {
        mouse_unsubscribe_int();
        timer_unsubscribe_int();
        return 1;
    }

    while (timer_counter < idle_time * hz) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & timer_mask) {
                        timer_int_handler(); // Call the timer interrupt handler
                        timer_counter++;
                    }
                    if (msg.m_notify.interrupts & mouse_mask) {
                        mouse_ih();
                        mouse_sync_bytes();
                        if (byte_index == 3) {
                            byte_index = 0;
                            mouse_bytes_to_packet();
                            mouse_print_packet(&pp);
                            timer_counter = 0; // Reset timer on mouse activity
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    // Disable mouse data reporting
    mouse_write(DISABLE_DATA_REPORTING);
    // Unsubscribe interrupts
    mouse_unsubscribe_int();
    timer_unsubscribe_int();

    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    int ipc_status, r;
    message msg;
    uint8_t mouse_mask;
    extern uint8_t byte_index;
    extern struct packet pp;

    enum state_t {
        INIT,           // Waiting for left button press
        DRAW_UP,        // Drawing first (up-right) line
        VERTEX,         // Waiting for right button press (vertex)
        DRAW_DOWN,      // Drawing second (down-right) line
        DONE            // Gesture completed
    } state = INIT;

    int16_t x_draw = 0, y_draw = 0; // Accumulators for each line

    if (mouse_subscribe_int(&mouse_mask) != 0) return 1;
    if (mouse_write(ENABLE_DATA_REPORTING) != 0) {
        mouse_unsubscribe_int();
        return 1;
    }

    while (state != DONE) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & mouse_mask) {
                        mouse_ih();
                        mouse_sync_bytes();
                        if (byte_index == 3) {
                            byte_index = 0;
                            mouse_bytes_to_packet();
                            mouse_print_packet(&pp);

                            int16_t dx = pp.delta_x;
                            int16_t dy = pp.delta_y;

                            switch (state) {
                                case INIT:
                                    if (pp.lb && !pp.rb && !pp.mb) {
                                        // Start of first line (ignore this packet's movement)
                                        x_draw = 0; y_draw = 0;
                                        state = DRAW_UP;
                                    }
                                    break;
                                case DRAW_UP:
                                    if (!pp.lb && !pp.rb && !pp.mb) {
                                        // End of first line
                                        if (x_draw >= x_len && abs(y_draw) >= x_draw && y_draw > 0) {
                                            state = VERTEX;
                                        } else {
                                            state = INIT; // Not enough movement or wrong slope
                                        }
                                    } else if (pp.lb && !pp.rb && !pp.mb) {
                                        // Only left button pressed
                                        // Accept only up-right movement (with tolerance)
                                        if (dx < -tolerance || dy < -tolerance) {
                                            state = INIT;
                                        } else {
                                            x_draw += dx;
                                            y_draw += dy;
                                        }
                                    } else {
                                        state = INIT; // Any other button event resets
                                    }
                                    break;
                                case VERTEX:
                                    if (pp.rb && !pp.lb && !pp.mb) {
                                        // Start of second line (ignore this packet's movement)
                                        x_draw = 0; y_draw = 0;
                                        state = DRAW_DOWN;
                                    } else if (!pp.lb && !pp.rb && !pp.mb) {
                                        // Only allow small movements (residual) between lines
                                        if (abs(dx) > tolerance || abs(dy) > tolerance) {
                                            state = INIT;
                                        }
                                    } else {
                                        state = INIT;
                                    }
                                    break;
                                case DRAW_DOWN:
                                    if (!pp.rb && !pp.lb && !pp.mb) {
                                        // End of second line
                                        if (x_draw >= x_len && abs(y_draw) >= x_draw && y_draw < 0) {
                                            state = DONE;
                                        } else {
                                            state = INIT;
                                        }
                                    } else if (pp.rb && !pp.lb && !pp.mb) {
                                        // Only right button pressed
                                        // Accept only down-right movement (with tolerance)
                                        if (dx < -tolerance || dy > tolerance) {
                                            state = INIT;
                                        } else {
                                            x_draw += dx;
                                            y_draw += dy;
                                        }
                                    } else {
                                        state = INIT;
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    mouse_write(DISABLE_DATA_REPORTING);
    mouse_unsubscribe_int();

    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
