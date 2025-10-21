/*
 * net.c - Minix network utilities for Gemini gateway communication
 * Part of AI.TAZ package for Minix
 * Uses ioctl-based socket operations compatible with Minix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include "net.h"

#define BUFFER_SIZE 512
#define READ_TIMEOUT 30

/*
 * Connect to a remote server
 * Returns 0 on success, -1 on failure
 */
int connect_to_server(const char *host, int port, struct connection *conn)
{
    struct hostent *hp;
    struct sockaddr_in addr;
    int sock;

    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    /* Get host information */
    hp = gethostbyname(host);
    if (hp == NULL) {
        fprintf(stderr, "gethostbyname: host %s not found\n", host);
        close(sock);
        return -1;
    }

    /* Setup address structure */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);

    /* Connect to server */
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    /* Store connection info */
    conn->socket_fd = sock;
    strncpy(conn->host, host, MAX_HOST_LEN - 1);
    conn->host[MAX_HOST_LEN - 1] = '\0';
    conn->port = port;

    return 0;
}

/*
 * Send HTTP request to server
 * Returns bytes sent on success, -1 on failure
 */
int send_request(struct connection *conn, const char *request)
{
    int bytes_sent;
    int total_sent = 0;
    int request_len = strlen(request);

    while (total_sent < request_len) {
        bytes_sent = write(conn->socket_fd,
                          request + total_sent,
                          request_len - total_sent);

        if (bytes_sent < 0) {
            perror("write");
            return -1;
        }

        if (bytes_sent == 0)
            break;

        total_sent += bytes_sent;
    }

    return total_sent;
}

/*
 * Receive response from server
 * Returns bytes received on success, -1 on failure
 */
int receive_response(struct connection *conn, char *buffer, int buffer_size)
{
    int total_read = 0;
    int bytes_read;
    int remaining = buffer_size - 1;

    memset(buffer, 0, buffer_size);

    while (remaining > 0) {
        bytes_read = read(conn->socket_fd, buffer + total_read, remaining);

        if (bytes_read < 0) {
            perror("read");
            return -1;
        }

        if (bytes_read == 0)
            break;

        total_read += bytes_read;
        remaining = buffer_size - total_read - 1;
    }

    buffer[total_read] = '\0';
    return total_read;
}

/*
 * Close the connection
 */
void close_connection(struct connection *conn)
{
    if (conn->socket_fd >= 0) {
        close(conn->socket_fd);
        conn->socket_fd = -1;
    }
}

/*
 * Build HTTP request for Gemini gateway
 * Returns request length on success, -1 on failure
 */
int build_request(const char *query, char *request_buffer, int buffer_size)
{
    int written;

    if (query == NULL || request_buffer == NULL || buffer_size <= 0)
        return -1;

    /* Build HTTP POST request */
    written = snprintf(request_buffer, buffer_size,
        "POST /api/ask HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "{\"query\": \"%s\"}",
        (int)(strlen(query) + 11),
        query);

    if (written < 0 || written >= buffer_size) {
        fprintf(stderr, "Request too large\n");
        return -1;
    }

    return written;
}

