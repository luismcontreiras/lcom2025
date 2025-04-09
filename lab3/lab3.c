#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>

#include "kbc.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int hook_id = 1; // Global variable for the interrupt hook ID

  // Step 1: Subscribe to KBC interrupts
  if (subscribe_kbc_interrupts() != OK) {
    printf("Failed to subscribe to KBC interrupts\n");
    return 1;
  }

  while (array_scancodes[0] != 0x81) {
    int ipc_status;
    message msg;
    // Wait for an interrupt
    if (driver_receive(ANY, &msg, &ipc_status) != OK) {
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(hook_id)) {
            // Call the interrupt handler
            kbc_ih();
          }
          break;
        default:
          break;
      }
    }
  }
  printf("%x|", array_scancodes[0]);
  printf("%x\n", array_scancodes[1]);

  // Step 2: Unsubscribe from KBC interrupts
  unsubscribe_kbc_interrupts();

  // Step 3: Print the total number of sys_inb() calls
  kbd_print_no_sysinb(sys_inb_count);

  return 0;
}
/*
  return 1;
}
*/



int(kbd_test_poll)() {
  uint8_t status, scancode;

  while (array_scancodes[0] != 0x81) {
      // Read the status register
      if (read_status_register(&status) != OK) {
          printf("Error reading status register\n");
          continue;
      }

      // Check if the output buffer is full and there are no errors
      if ((status & KBC_OBF) && !(status & KBC_AUX) && !(status & (KBC_PAR_ERROR | KBC_TIMEOUT_ERROR))) {
          // Read the scancode from the output buffer
          if (read_output_buffer(&scancode) != OK) {
              printf("Error reading output buffer\n");
              continue;
          }

          // Handle the scancode
          handle_scancode(scancode);

      }

      // Add a small delay to avoid excessive CPU usage
      tickdelay(micros_to_ticks(DELAY_US)); // e.g., DELAY_US = 20000
  }

  // Re-enable keyboard interrupts before exiting
  if (enable_keyboard_interrupts() != OK) {
      printf("Failed to re-enable keyboard interrupts\n");
  }

  // Print the number of sys_inb() calls
  kbd_print_no_sysinb(sys_inb_count);

  return 0;
}





int(kbd_test_timed_scan)(uint8_t n) {
    int kbd_hook_id = IRQ1;
    int timer_hook_id = IRQ_TIMER;

    // Subscribe to keyboard and timer interrupts
    if (subscribe_kbc_interrupts(&kbd_hook_id) != OK || subscribe_timer_interrupts(&timer_hook_id) != OK) {
        printf("Failed to subscribe interrupts\n");
        return 1;
    }

    // Main loop to handle interrupts
    while (array_scancodes[0] != 0x81) {
        int ipc_status;
        message msg;

        // Wait for an interrupt
        if (driver_receive(ANY, &msg, &ipc_status) != OK) {
            continue;
        }

        // Handle interrupts
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:
                    if (msg.m_notify.interrupts & BIT(kbd_hook_id)) {
                        kbc_ih(); // Call keyboard interrupt handler

                        // Reset idle time counter
                        idle_time = 0;

                        // Check if the Esc key was released (break code 0x81)
                        if (size == 1 && array_scancodes[0] == 0x81) {
                            goto exit_loop;
                        }
                    }
                    if (msg.m_notify.interrupts & BIT(timer_hook_id)) {
                        timer_ih(); // Call timer interrupt handler

                        // Check if idle time exceeds the limit
                        if (idle_time >= n) {
                            goto exit_loop;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

exit_loop:
    // Unsubscribe from interrupts
    if (unsubscribe_kbc_interrupts(&kbd_hook_id) != OK || unsubscribe_timer_interrupts(&timer_hook_id) != OK) {
        printf("Failed to unsubscribe interrupts\n");
    }

    // Print the number of sys_inb() calls
    kbd_print_no_sysinb(sys_inb_count);

    return 0;
}


