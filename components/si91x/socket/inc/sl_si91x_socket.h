#pragma once
#include "sl_si91x_socket_types.h"

int sl_si91x_socket_async(int family, int type, int protocol, receive_data_callback callback);

int sl_si91x_socket(int family, int type, int protocol);

int sl_si91x_bind(int socket, const struct sockaddr *addr, socklen_t addr_len);

int sl_si91x_listen(int socket, int max_number_of_clients);

int sl_si91x_accept(int socket, const struct sockaddr *addr, socklen_t addr_len);

int sl_si91x_accept_async(int socket, accept_callback callback);

int sl_si91x_connect(int socket, const struct sockaddr *addr, socklen_t addr_len);

int sl_si91x_send(int socket, const uint8_t *buffer, size_t buffer_length, int32_t flags);

int sl_si91x_send_async(int socket,
                        const uint8_t *buffer,
                        size_t buffer_length,
                        int32_t flags,
                        data_transfer_complete_handler callback);

int sl_si91x_sendto(int socket,
                    uint8_t *buffer,
                    size_t buffer_length,
                    int32_t flags,
                    const struct sockaddr *addr,
                    socklen_t addr_len);

int sl_si91x_sendto_async(int socket,
                          uint8_t *buffer,
                          size_t buffer_length,
                          int32_t flags,
                          const struct sockaddr *to_addr,
                          socklen_t to_addr_len,
                          data_transfer_complete_handler callback);

int sl_si91x_recv(int socket, uint8_t *buffer, size_t bufferLength, int32_t flags);

int sl_si91x_recvfrom(int socket,
                      uint8_t *buffer,
                      size_t buffersize,
                      int32_t flags,
                      struct sockaddr *fromAddr,
                      socklen_t *fromAddrLen);

/**
 *
 * @note: In case of socket being a server socket, the "how" parameter would be ignored and socket shall always be closed based on port number.
 */
int sl_si91x_shutdown(int socket, int how);

int sl_si91x_select(int nfds,
                    fd_set *readfds,
                    fd_set *writefds,
                    fd_set *exceptfds,
                    struct timeval *timeout,
                    select_callback callback);
