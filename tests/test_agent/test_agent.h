#pragma once

#include "threaded_commands.h"

static inline console_threaded_command_t *get_thread_object(osThreadId_t id)
{
  for (int a = 0; a < MAX_THREADED_COMMANDS; ++a) {
    if (id == threaded_commands[a].thread_id) {
      return &threaded_commands[a];
    }
  }
  return NULL;
}
