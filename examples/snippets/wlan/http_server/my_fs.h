//Create a structure for file system having buffer_address, buffer_length, current_offset
#include <stdio.h>
#include <stdint.h>

typedef struct {
  char *pathname;
  char *buffer;
  uint32_t buffer_length;
  uint32_t offset;
} my_fs_file_t;

int fs_open(const char *pathname);
int fs_close(int fd);
int fs_read(int fd, void *data, uint32_t count);
int fs_stat(int fd, uint32_t *file_size);
void initialize_fs_files();
