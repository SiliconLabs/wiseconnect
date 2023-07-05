#include "timer_platform.h"
#include "cmsis_os2.h"

bool has_timer_expired(Timer *timer)
{
  long int elapsed_time = (timer->end_time - osKernelGetTickCount());
  return (elapsed_time < 0);
}

void countdown_ms(Timer *timer, uint32_t timeout_ms)
{
  timer->timeout  = timeout_ms;
  timer->end_time = osKernelGetTickCount() + timeout_ms;
}

void countdown_sec(Timer *timer, uint32_t timeout_sec)
{
  timer->timeout  = (timeout_sec * 1000);
  timer->end_time = osKernelGetTickCount() + (timeout_sec * 1000);
}

uint32_t left_ms(Timer *timer)
{
  long int remaining_time = timer->end_time - osKernelGetTickCount();
  return (remaining_time < 0) ? 0 : remaining_time;
}

void init_timer(Timer *timer)
{
  timer->end_time = 0;
}

void delay(unsigned milliseconds)
{
  osDelay(milliseconds);
}
