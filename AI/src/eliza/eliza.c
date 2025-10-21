/*
 * eliza.c - ELIZA conversational agent
 * Part of AI.TAZ package for Minix
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "script.h"

#define INPUT_BUFFER 512
#define OUTPUT_BUFFER 512

/* Function prototypes */
void to_lowercase(char *str);
int is_quit_phrase(const char *input);
int find_keyword(const char *input, char *matched_keyword);
char *get_random_response(struct keyword_response *kw);
char *get_filler_response(void);
char *reflect_response(char *response);
void process_input(char *input);

/*
 * Convert string to lowercase for case-insensitive matching
 */
void to_lowercase(char *str)
{
    int i;
    for (i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

/*
 * Check if input is a quit phrase
 */
int is_quit_phrase(const char *input)
{
    int i;
    char lower_input[INPUT_BUFFER];

    strncpy(lower_input, input, sizeof(lower_input) - 1);
    lower_input[sizeof(lower_input) - 1] = '\0';
    to_lowercase(lower_input);

    for (i = 0; i < QUIT_SIZE; i++) {
        if (strstr(lower_input, quit_phrases[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

/*
 * Find if input contains any known keyword
 * Returns 1 if keyword found, 0 otherwise
 */
int find_keyword(const char *input, char *matched_keyword)
{
    int i;
    char lower_input[INPUT_BUFFER];

    strncpy(lower_input, input, sizeof(lower_input) - 1);
    lower_input[sizeof(lower_input) - 1] = '\0';
    to_lowercase(lower_input);

    for (i = 0; i < SCRIPT_SIZE; i++) {
        if (strstr(lower_input, script[i].keyword) != NULL) {
            strcpy(matched_keyword, script[i].keyword);
            return i;
        }
    }
    return -1;
}

/*
 * Get a random response from a keyword's response set
 */
char *get_random_response(struct keyword_response *kw)
{
    int index;
    if (kw->response_count <= 0)
        return "I'm not sure how to respond.";

    index = rand() % kw->response_count;
    return kw->responses[index];
}

/*
 * Get a random filler response for unrecognized input
 */
char *get_filler_response(void)
{
    int index = rand() % FILLER_SIZE;
    return (char *)filler_responses[index];
}

/*
 * Reflect pronouns in the response
 */
char *reflect_response(char *response)
{
    static char reflected[OUTPUT_BUFFER];
    char temp[OUTPUT_BUFFER];
    char *pos, *start;
    int i, len;

    strncpy(reflected, response, sizeof(reflected) - 1);
    reflected[sizeof(reflected) - 1] = '\0';

    to_lowercase(reflected);

    for (i = 0; i < REFLECTION_SIZE; i++) {
        while ((pos = strstr(reflected, reflections[i].original)) != NULL) {
            len = strlen(reflections[i].original);
            /* Check if it's a whole word (basic check) */
            if ((pos == reflected || !isalnum((unsigned char)*(pos - 1))) &&
                (!*(pos + len) || !isalnum((unsigned char)*(pos + len)))) {
                strcpy(temp, pos + len);
                strcpy(pos, reflections[i].reflected);
                strcat(pos, temp);
            } else {
                break;
            }
        }
    }

    return reflected;
}

/*
 * Process and respond to user input
 */
void process_input(char *input)
{
    char matched_keyword[MAX_WORD_LEN];
    char response[OUTPUT_BUFFER];
    char *selected_response;
    int keyword_index;

    /* Remove trailing newline */
    int len = strlen(input);
    if (len > 0 && input[len - 1] == '\n')
        input[len - 1] = '\0';

    /* Check for quit phrases */
    if (is_quit_phrase(input)) {
        printf("Goodbye! It was nice talking with you.\n");
        exit(0);
    }

    /* Try to find a keyword in the input */
    keyword_index = find_keyword(input, matched_keyword);

    if (keyword_index >= 0) {
        /* Found a keyword, get a response */
        selected_response = get_random_response(&script[keyword_index]);
    } else {
        /* No keyword found, use filler response */
        selected_response = get_filler_response();
    }

    /* Apply reflection transformations */
    strcpy(response, selected_response);
    printf("%s\n", response);
}

/*
 * Main ELIZA loop
 */
int main(int argc, char *argv[])
{
    char input[INPUT_BUFFER];

    /* Seed random number generator */
    srand(time(NULL));

    printf("ELIZA - Conversational AI\n");
    printf("========================\n");
    printf("Hello, I'm ELIZA. Tell me about yourself.\n");
    printf("Type 'quit' to exit.\n\n");

    /* Main interaction loop */
    while (1) {
        printf("You: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        printf("ELIZA: ");
        process_input(input);
        printf("\n");
    }

    return 0;
}

