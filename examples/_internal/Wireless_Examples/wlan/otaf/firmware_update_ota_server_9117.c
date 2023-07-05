/*******************************************************************************
* @file  firmware_update_ota_server.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#define RPS_HEADER 0x01
#define RPS_DATA   0x00
#include "stdio.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>

#define RSI_RPS_HEADER_LENGTH 64

#define RSI_MAX_CLIENT_CONNECTIONS 10
#define RSI_DEBUG                  1

int processRequest(int fd, char *file_ptr);
signed int sock_id, fd;
void handler(int signo)
{
  close(fd);
  close(sock_id);
  exit(1);
}

int main(int argc, char **argv)
{
  signed int dst_len;
  struct sockaddr_in sock;
  struct sockaddr_in dst_sock;
  int window_size = 2920;
  FILE *fp        = NULL;

  char sendip[50];

  //! Checking all proper Command line Arguements
  if (argc < 3) {
    printf("Usage ./a.out <local port> <RPS file path>\n");
    exit(0);
  }

  //! Creating of the TCP Socket
  if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("Socket create fail\n");
    return;
  }

  //! filling the structure to connect with the client
  sock.sin_family      = AF_INET;
  sock.sin_port        = htons(atoi(argv[1]));
  sock.sin_addr.s_addr = htonl(0);

  //! try to bind to the specified socket
  if (bind(sock_id, (struct sockaddr *)&sock, sizeof(sock)) == -1) {
    printf("Socket bind fail\n");
    return;
  }

  signal(SIGINT, handler);
  setsockopt(sock_id, SOL_SOCKET, SO_RCVBUF, (char *)&window_size, sizeof(window_size));

  if (listen(sock_id, RSI_MAX_CLIENT_CONNECTIONS) != 0) {
    printf("Socket Listen fail\n");
    return;
  }

  printf("Listen passed\n");

  dst_len = sizeof(struct sockaddr_in);
  while (1) {
    //! Accept client request
    printf("waiting for client to connect\n");
    fd = accept(sock_id, (struct sockaddr *)&dst_sock, (socklen_t *)&dst_len);

    strcpy(sendip, (const char *)inet_ntop(AF_INET, (void *)&dst_sock.sin_addr, sendip, sizeof(sendip)));
    printf("Connect Req from %s accepted\n", sendip);

    if (fork() == 0) {
      close(sock_id);
      processRequest(fd, argv[2]);
      exit(0);
    }
    close(fd);
  }
}

int processRequest(int fd, char *file_ptr)
{
  int ctr            = 0;
  signed int ret_len = 0, tx_len = 0;
  int errno;
  unsigned char data[3];
  unsigned char *ptr = data;
  unsigned char cmd_type;
  char data1[1500];
  int length        = 0;
  FILE *fp          = NULL;
  uint32_t chunk_no = 0, temp_len = 0, temp_cnt = 0;

  fp = fopen(file_ptr, "r");
  if (fp == NULL) {
    printf("unabled to open rps file\n");
    return;
  }

  while (1) {

again:

#if RSI_DEBUG
    printf("waiting for recv on  %d \n", fd);
#endif
    ret_len = recv(fd, ptr, 100, 0);
#if RSI_DEBUG
    printf("recv length == 0x%x\n", ret_len);
#endif
    if (ret_len < 0 && errno == EINTR) {
      printf("Recv error\n");
      goto again;
    } else if (ret_len < 0) {
      printf("error while receiving\n");
      return (0);
    }

    if (ret_len > 0) {
      cmd_type = data[0];

      if (cmd_type == RPS_HEADER) {
        length   = fread((data1 + 3), 1, RSI_RPS_HEADER_LENGTH, fp);
        data1[0] = RPS_HEADER;
        data1[1] = (length & 0xff);
        data1[2] = ((length >> 8) & 0xff);
        fseek(fp, 0, SEEK_SET);

#if RSI_DEBUG
        printf("length of first chunk==%d\n", length);
#endif
      } else if (cmd_type == RPS_DATA) {

        chunk_no = *(uint16_t *)&data[1];

        temp_cnt = (ftell(fp)) / (800);
#if RSI_DEBUG
        printf("\nChunk_no == %d \n", chunk_no);
#endif
        if (temp_cnt != (chunk_no - 2)) {
          fseek(fp, 0, SEEK_SET);
          fseek(fp, (800 * (chunk_no - 2)), SEEK_CUR);
        }

        length   = fread((data1 + 3), 1, 800, fp);
        data1[0] = RPS_DATA;
        data1[1] = (length & 0x00ff);
        data1[2] = ((length >> 8) & 0x00ff);
        if (feof(fp)) {
          fclose(fp);
          printf("reach end of file\n");
          tx_len = send(fd, data1, length + 3, 0);
          return;
        }
      }
    }

#if RSI_DEBUG
    printf("size of data1==%d\n", length);
#endif

    tx_len = send(fd, data1, length + 3, 0);
#if RSI_DEBUG
    printf("send returns %d\n", tx_len);
#endif

    if (!tx_len) {
      printf("error while sending\n");
      return (0);
    }

#if RSI_DEBUG
    printf("Pkt sent no:%d\n", ++ctr);
#endif
  }
}
