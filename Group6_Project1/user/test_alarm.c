#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Global counter to track how many times the alarm handler fires
int alarm_count = 0;

// This is the alarm handler function.
// The kernel will force the CPU to jump here after every N ticks.
// After it finishes, it MUST call alarm_return() to restore
// the original registers and resume normal execution.
void alarm_handler(void) {
  alarm_count++;
  printf("  >>> [ALARM] Handler fired! (count = %d)\n", alarm_count);
  alarm_return();
}

int main(void) {
  printf("=== Testing alarm() System Call ===\n\n");
  printf("[MAIN] PID = %d\n", getpid());

  // Register the alarm: fire the handler every 10 timer ticks
  printf("[MAIN] Setting alarm: interval = 10 ticks, handler = alarm_handler\n\n");
  alarm(10, alarm_handler);

  // Spin in a busy loop to burn CPU ticks.
  // The timer interrupt will fire periodically, and after 10 ticks
  // the kernel will redirect execution to alarm_handler().
  printf("[MAIN] Entering busy loop... waiting for alarms to fire.\n");

  int i;
  // We loop for a large number to give enough time for multiple alarms
  for (i = 0; i < 500000000; i++) {
    if (alarm_count >= 3) {
      break;
    }
  }

  printf("\n[MAIN] Exited loop. Alarm handler was triggered %d time(s).\n", alarm_count);

  // Disable the alarm by setting interval to 0
  printf("[MAIN] Disabling alarm (interval = 0).\n");
  alarm(0, 0);

  if (alarm_count >= 3) {
    printf("\n=== SUCCESS: Alarm signal is working correctly! ===\n");
  } else {
    printf("\n=== WARNING: Alarm fired fewer times than expected. ===\n");
  }

  exit(0);
}
