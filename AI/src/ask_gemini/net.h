/*
 * net.h - Minix network utilities for Gemini gateway communication
 * Part of AI.TAZ package for Minix
 */

#ifndef NET_H
#define NET_H

#define MAX_HOST_LEN 256
#define MAX_REQUEST_LEN 2048
#define MAX_RESPONSE_LEN 4096
#define MAX_PORT_STR 6

/* Connection context structure */
struct connection {
    int socket_fd;
    char host[MAX_HOST_LEN];
    int port;
};

/* Function prototypes */
int connect_to_server(const char *host, int port, struct connection *conn);
int send_request(struct connection *conn, const char *request);
int receive_response(struct connection *conn, char *buffer, int buffer_size);
void close_connection(struct connection *conn);
int build_request(const char *query, char *request_buffer, int buffer_size);

#endif /* NET_H */

