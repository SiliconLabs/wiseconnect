#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

char *temp;
void adline_inc_store_data(const char *filepath, const char *data)
{
  FILE *fp = fopen(filepath, "w");
  if (fp != NULL) {
    fputs(data, fp);
    fclose(fp);
  }
}

int main(int argc, char *argv[])
{
  int get_mode = atoi(argv[1]);
  if (get_mode == 0) {
    int nfd = open(argv[2], O_WRONLY);
    lseek(nfd, 0, SEEK_END);
    write(nfd, "\0", 1);
    lseek(nfd, atoi(argv[3]) - 1, SEEK_SET);
    write(nfd, "\0", 1);
    close(nfd);
  }
  if (get_mode == 1) {
    char fout[]  = "#define _fixed_pattern  0x5aa5\n#define  _offset 4032\n#define  _ivt_offset ";
    char *fout1  = argv[3];
    char fout2[] = "\n#define  _control_len ";
    char *fout3  = argv[4];
    char fout4[] =
      "\n#define  _bl_entry_control_reserved 0\n#define  _bl_entry_control_spi_32bitmode 0\n#define "
      "_bl_entry_control_release_ta_softreset 0\n#define _bl_entry_control_start_from_rom_pc 0\n#define "
      "_bl_entry_control_spi_8bitmode 0\n#define _bl_entry_control_last_entry 1\n#define _bl_entry_dst_addr 0";
    char *File_create_with;
    File_create_with = malloc(strlen(fout) + strlen(fout1) + strlen(fout2) + strlen(fout3)
                              + strlen(fout4)); /* make space for the new string (should check the return value ...) */
    strcpy(File_create_with, fout);             /* copy name into the new var */
    strcat(File_create_with, fout1);            /* add the extension */
    strcat(File_create_with, fout2);
    strcat(File_create_with, fout3);
    strcat(File_create_with, fout4);
    adline_inc_store_data(argv[2], File_create_with);
    free(File_create_with);
  }
  if (get_mode == 2) {
    char *File_create_with;
    char fout[]      = "00000000\n";
    int line_inc     = atoi(argv[3]);
    File_create_with = malloc(sizeof(fout) * line_inc);
    memset(File_create_with, 0, sizeof(File_create_with));
    if (File_create_with == NULL) {
      printf("cannot allocate memory");
    }
    int i;
    for (i = 0; i < line_inc; i++) {
      strcat(File_create_with, fout);
    }
    adline_inc_store_data(argv[2], File_create_with);
    free(File_create_with);
  }
  return 0;
}