#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "login.h"
#include "helloworld.h"
#include "provisioning.h"
#include "my_fs.h"

//Create an array of my_fs_file_t
#define MAX_FILES 3

my_fs_file_t my_fs_files[MAX_FILES];

void initialize_fs_files()
{
  // Calculate the lengths of the HTML contents
  size_t html_length1 = sizeof(helloworld_content);
  size_t html_length2 = sizeof(login_content);
  size_t html_length3 = sizeof(provisioning_content);

  // Initialize the elements of the my_fs_files array
  my_fs_files[0].pathname      = "/test1";
  my_fs_files[0].buffer        = (char *)helloworld_content;
  my_fs_files[0].buffer_length = html_length1;
  my_fs_files[0].offset        = 0;

  my_fs_files[1].pathname      = "/test2";
  my_fs_files[1].buffer        = (char *)login_content;
  my_fs_files[1].buffer_length = html_length2;
  my_fs_files[1].offset        = 0;

  my_fs_files[2].pathname      = "/test3";
  my_fs_files[2].buffer        = (char *)provisioning_content;
  my_fs_files[2].buffer_length = html_length3;
  my_fs_files[2].offset        = 0;
}

int fs_open(const char *pathname)
{
  //take array of my_fs_files return array index if pathname is present else return -1
  for (int i = 0; i < MAX_FILES; i++) {
    if (strcmp(my_fs_files[i].pathname, pathname) == 0) {
      return i;
    }
  }
  return -1;
}

int fs_close(int fd)
{
  //Reset current_offset of my_fs_file_t present at index fd
  my_fs_files[fd].offset = 0;
  return 0;
}

int fs_read(int fd, void *data, uint32_t count)
{
  // Calculate the remaining bytes in the file
  uint32_t remaining_bytes = my_fs_files[fd].buffer_length - my_fs_files[fd].offset;

  // If count is greater than the remaining bytes, adjust it
  if (count > remaining_bytes) {
    count = remaining_bytes;
  }

  // Copy buffer data of size count present at index fd into data
  memcpy(data, my_fs_files[fd].buffer + my_fs_files[fd].offset, count);

  // Move the offset
  my_fs_files[fd].offset += count;

  // Return the number of bytes read
  return count;
}

int fs_stat(int fd, uint32_t *file_size)
{
  // Return the length of the buffer at index fd
  *file_size = my_fs_files[fd].buffer_length;
  return 0;
}
