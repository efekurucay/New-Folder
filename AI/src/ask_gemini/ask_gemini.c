/*
 * ask_gemini.c - Gemini AI gateway client
 * Part of AI.TAZ package for Minix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "net.h"

#define QUERY_BUFFER 512
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT 8080

/* Function prototypes */
void usage(const char *program);
void parse_response(const char *response);
char *extract_json_value(const char *response, const char *key);

/*
 * Print usage information
 */
void usage(const char *program)
{
    fprintf(stderr, "Usage: %s [OPTIONS]\n", program);
    fprintf(stderr, "  -h HOST    Connect to HOST (default: %s)\n", DEFAULT_HOST);
    fprintf(stderr, "  -p PORT    Connect to PORT (default: %d)\n", DEFAULT_PORT);
    fprintf(stderr, "  -q QUERY   Ask a question\n");
    fprintf(stderr, "  -?         Show this help\n");
}

/*
 * Simple JSON value extractor
 * Looks for "key": "value" and returns the value
 */
char *extract_json_value(const char *response, const char *key)
{
    static char value[MAX_RESPONSE_LEN];
    char search_pattern[256];
    const char *start, *end;
    int len;

    snprintf(search_pattern, sizeof(search_pattern), "\"%s\": \"", key);

    start = strstr(response, search_pattern);
    if (start == NULL)
        return NULL;

    start += strlen(search_pattern);

    end = strchr(start, '"');
    if (end == NULL)
        return NULL;

    len = end - start;
    if (len >= MAX_RESPONSE_LEN)
        len = MAX_RESPONSE_LEN - 1;

    strncpy(value, start, len);
    value[len] = '\0';

    return value;
}

/*
 * Parse and display server response
 */
void parse_response(const char *response)
{
    const char *body_start;
    char *answer;

    /* Skip HTTP headers */
    body_start = strstr(response, "\r\n\r\n");
    if (body_start == NULL) {
        body_start = strstr(response, "\n\n");
        if (body_start == NULL) {
            printf("%s\n", response);
            return;
        }
        body_start += 2;
    } else {
        body_start += 4;
    }

    /* Try to extract answer from JSON */
    answer = extract_json_value(body_start, "answer");
    if (answer != NULL) {
        printf("Answer: %s\n", answer);
    } else {
        /* Print raw body if no JSON found */
        printf("%s\n", body_start);
    }
}

/*
 * Main program
 */
int main(int argc, char *argv[])
{
    char host[MAX_HOST_LEN];
    int port;
    char query[QUERY_BUFFER];
    struct connection conn;
    char request[MAX_REQUEST_LEN];
    char response[MAX_RESPONSE_LEN];
    int i;

    /* Set defaults */
    strcpy(host, DEFAULT_HOST);
    port = DEFAULT_PORT;
    memset(query, 0, sizeof(query));

    /* Parse command-line arguments */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'h':
                if (i + 1 < argc) {
                    strncpy(host, argv[++i], MAX_HOST_LEN - 1);
                    host[MAX_HOST_LEN - 1] = '\0';
                }
                break;
            case 'p':
                if (i + 1 < argc) {
                    port = atoi(argv[++i]);
                }
                break;
            case 'q':
                if (i + 1 < argc) {
                    strncpy(query, argv[++i], QUERY_BUFFER - 1);
                    query[QUERY_BUFFER - 1] = '\0';
                }
                break;
            case '?':
                usage(argv[0]);
                return 0;
            default:
                fprintf(stderr, "Unknown option: %s\n", argv[i]);
                usage(argv[0]);
                return 1;
            }
        }
    }

    /* If no query provided, read from stdin */
    if (strlen(query) == 0) {
        printf("Enter your question: ");
        fflush(stdout);
        if (fgets(query, sizeof(query), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            return 1;
        }
        /* Remove trailing newline */
        int len = strlen(query);
        if (len > 0 && query[len - 1] == '\n')
            query[len - 1] = '\0';
    }

    /* Validate query */
    if (strlen(query) == 0) {
        fprintf(stderr, "No query provided\n");
        return 1;
    }

    /* Build request */
    if (build_request(query, request, sizeof(request)) < 0) {
        fprintf(stderr, "Failed to build request\n");
        return 1;
    }

    /* Connect to server */
    printf("Connecting to %s:%d...\n", host, port);
    if (connect_to_server(host, port, &conn) < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return 1;
    }

    /* Send request */
    printf("Sending query...\n");
    if (send_request(&conn, request) < 0) {
        fprintf(stderr, "Failed to send request\n");
        close_connection(&conn);
        return 1;
    }

    /* Receive response */
    printf("Waiting for response...\n");
    if (receive_response(&conn, response, sizeof(response)) < 0) {
        fprintf(stderr, "Failed to receive response\n");
        close_connection(&conn);
        return 1;
    }

    /* Close connection */
    close_connection(&conn);

    /* Parse and display response */
    parse_response(response);

    return 0;
}

