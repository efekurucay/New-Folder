/*
 * script.h - ELIZA script data structures and responses
 * Part of AI.TAZ package for Minix
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#define MAX_KEYWORDS 50
#define MAX_RESPONSES 10
#define MAX_REFLECTIONS 20
#define MAX_WORD_LEN 32
#define MAX_RESPONSE_LEN 256

/* Structure for a keyword and its associated responses */
struct keyword_response {
    char keyword[MAX_WORD_LEN];
    char *responses[MAX_RESPONSES];
    int response_count;
};

/* Structure for reflection word pairs (I -> you, me -> you, etc.) */
struct reflection {
    char original[MAX_WORD_LEN];
    char reflected[MAX_WORD_LEN];
};

/* ELIZA script array */
static struct keyword_response script[] = {
    {
        "hello",
        {
            "Hello! How are you feeling today?",
            "Hi there! What brings you here?",
            "Greetings! Tell me, how can I help?",
            NULL
        },
        3
    },
    {
        "help",
        {
            "I'm here to listen. What troubles you?",
            "Tell me more about what you need help with.",
            "I'll do my best to assist you.",
            NULL
        },
        3
    },
    {
        "mother",
        {
            "Tell me more about your mother.",
            "How does your mother make you feel?",
            "What does your mother mean to you?",
            NULL
        },
        3
    },
    {
        "father",
        {
            "Let's talk about your father.",
            "How does your father make you feel?",
            "What is your relationship with your father?",
            NULL
        },
        3
    },
    {
        "love",
        {
            "That's a powerful emotion. Tell me more.",
            "Love is an important feeling. What about it?",
            "How does love affect your life?",
            NULL
        },
        3
    },
    {
        "hate",
        {
            "Hate is strong. Why do you feel this way?",
            "Tell me what provokes such strong feelings.",
            "What is it about this that angers you?",
            NULL
        },
        3
    },
    {
        "dream",
        {
            "Dreams can be very revealing. Tell me more.",
            "What do your dreams mean to you?",
            "Have you had this dream before?",
            NULL
        },
        3
    },
    {
        "computer",
        {
            "Computers are interesting. Why do you mention them?",
            "Do you have feelings about computers?",
            "What do computers mean to you?",
            NULL
        },
        3
    },
    {
        "think",
        {
            "Do you often think about such things?",
            "What else do you think about?",
            "I'm interested in your thoughts.",
            NULL
        },
        3
    },
    {
        "feel",
        {
            "Tell me more about these feelings.",
            "How long have you felt this way?",
            "What causes these feelings?",
            NULL
        },
        3
    }
};

#define SCRIPT_SIZE (sizeof(script) / sizeof(struct keyword_response))

/* Reflection word pairs for response transformation */
static struct reflection reflections[] = {
    { "me", "you" },
    { "i", "you" },
    { "am", "are" },
    { "my", "your" },
    { "you", "me" },
    { "i'm", "you're" },
    { "i've", "you've" },
    { "i'll", "you'll" },
    { "myself", "yourself" },
    { "yourself", "myself" }
};

#define REFLECTION_SIZE (sizeof(reflections) / sizeof(struct reflection))

/* Non-committal responses for unrecognized input */
static const char *filler_responses[] = {
    "I see. Tell me more.",
    "That's interesting. Go on.",
    "Hmm, I'm listening.",
    "Please continue.",
    "Yes? And then?",
    "I see what you mean.",
    "How does that make you feel?",
    "Why do you think that?"
};

#define FILLER_SIZE (sizeof(filler_responses) / sizeof(const char *))

/* Quit phrases to end the session */
static const char *quit_phrases[] = {
    "bye",
    "goodbye",
    "exit",
    "quit",
    "bye!",
    "see you",
    "goodbye!"
};

#define QUIT_SIZE (sizeof(quit_phrases) / sizeof(const char *))

#endif /* SCRIPT_H */
