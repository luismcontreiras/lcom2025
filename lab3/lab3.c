#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>


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
  /* To be completed by the students */
  //printf("%s is not yet implemented!\n", __func__);

  // Step 1: Subscribe to KBC interrupts
  if (subscribe_kbc_interrupts() != OK) {
      printf("Failed to subscribe to KBC interrupts\n");
      return 1;
  }

  // Main loop to handle interrupts
  while (1) {
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

                      // Process the assembled scancode
                      if (size > 0) {
                          bool is_make_code = !(bytes[size - 1] & 0x80);
                          kbd_print_scancode(is_make_code, size, bytes);

                          // Check for ESC break code (0x81)
                          if (bytes[size - 1] == 0x81) {
                              goto exit_loop;
                          }
                          size = 0; // Reset size for the next scancode
                      }
                  }
                  break;
              default:
                  break;
          }
      }
  }

  exit_loop:
    // Step 2: Unsubscribe from KBC interrupts
    unsubscribe_kbc_interrupts();
  return 0;
}
/*
  return 1;
}
*/
int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
