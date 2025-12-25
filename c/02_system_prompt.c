/**
 * ============================================================================
 * Example 2: System Prompt - Controlling AI Behavior
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using system prompts to set AI personality and behavior
 *   - The three message roles: system, user, assistant
 *   - How system messages influence AI responses
 *
 * WHAT YOU'LL LEARN:
 *   - System prompt patterns for different use cases
 *   - Message ordering and its importance
 *   - Creating specialized AI assistants
 *
 * COMPILE:
 *   gcc -o 02_system_prompt 02_system_prompt.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./02_system_prompt
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

/* Response buffer structure */
struct ResponseBuffer {
    char *data;
    size_t size;
};

/* Callback to accumulate response data */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct ResponseBuffer *buf = (struct ResponseBuffer *)userp;
    char *ptr = realloc(buf->data, buf->size + realsize + 1);
    if (ptr == NULL) return 0;
    buf->data = ptr;
    memcpy(&(buf->data[buf->size]), contents, realsize);
    buf->size += realsize;
    buf->data[buf->size] = '\0';
    return realsize;
}

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        curl_global_cleanup();
        return 1;
    }

    /* ========================================================================
     * Step 3: Build the JSON request with a SYSTEM PROMPT
     * ========================================================================
     * The system prompt sets the AI's personality and rules.
     * It's like giving instructions to an employee before they start work.
     *
     * MESSAGE ROLES:
     *   - "system": Instructions for the AI (invisible to the "user")
     *   - "user": Messages from the human (you)
     *   - "assistant": Messages from the AI (in conversation history)
     */

    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();

    /* SYSTEM MESSAGE: Sets AI behavior */
    /* This AI will act as a pirate! */
    cJSON *system_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(system_msg, "role", "system");
    cJSON_AddStringToObject(system_msg, "content",
        "You are a friendly pirate who speaks in pirate dialect. "
        "You always include nautical references and say 'Arrr!' occasionally. "
        "Keep responses brief but entertaining.");
    cJSON_AddItemToArray(messages, system_msg);

    /* USER MESSAGE: The actual question */
    cJSON *user_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(user_msg, "role", "user");
    cJSON_AddStringToObject(user_msg, "content", "What is the capital of France?");
    cJSON_AddItemToArray(messages, user_msg);

    /* Add fields to root */
    cJSON_AddStringToObject(root, "model", "meta-llama/llama-4-scout-17b-16e-instruct");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "temperature", 0.8);  /* Higher = more creative */
    cJSON_AddNumberToObject(root, "max_tokens", 150);

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 4: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 5: Initialize response buffer */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    /* Step 6: Configure and perform request */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    printf("Asking a pirate about France...\n\n");
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
        free(response.data);
        free(json_payload);
        cJSON_Delete(root);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 7: Parse and display response */
    cJSON *response_json = cJSON_Parse(response.data);
    if (response_json) {
        cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
        if (choices && cJSON_GetArraySize(choices) > 0) {
            cJSON *first = cJSON_GetArrayItem(choices, 0);
            cJSON *msg = cJSON_GetObjectItem(first, "message");
            cJSON *content = cJSON_GetObjectItem(msg, "content");
            if (content && cJSON_IsString(content)) {
                printf("Pirate AI says:\n%s\n", content->valuestring);
            }
        }
        cJSON_Delete(response_json);
    }

    /* Step 8: Clean up */
    free(response.data);
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    printf("\n");
    printf("=== SYSTEM PROMPT EXAMPLES ===\n\n");
    printf("1. EXPERT PERSONA:\n");
    printf("   \"You are a senior software engineer with 20 years experience.\n");
    printf("    Give detailed technical explanations with code examples.\"\n\n");
    printf("2. TEACHER:\n");
    printf("   \"You are a patient teacher for 10-year-old students.\n");
    printf("    Use simple words and fun analogies.\"\n\n");
    printf("3. JSON ONLY:\n");
    printf("   \"Respond only with valid JSON. No other text.\n");
    printf("    Use the format: {\\\"answer\\\": \\\"...\\\"}\"\n\n");
    printf("4. SAFETY:\n");
    printf("   \"You are a helpful assistant. Never provide harmful information.\n");
    printf("    If asked about dangerous topics, politely decline.\"\n");

    return 0;
}

/*
 * SYSTEM PROMPT BEST PRACTICES:
 *
 * 1. Be specific about the persona
 *    Bad:  "Be helpful"
 *    Good: "You are a patient math tutor for high school students"
 *
 * 2. Define output format if needed
 *    "Always respond in JSON format"
 *    "Keep responses under 100 words"
 *
 * 3. Set boundaries
 *    "Only discuss topics related to cooking"
 *    "Do not provide medical advice"
 *
 * 4. Include examples
 *    "When greeting, say 'Howdy partner!' like a cowboy"
 *
 * 5. Order matters
 *    System message should always come FIRST
 *    Then conversation history (alternating user/assistant)
 *    Then the current user message
 *
 * MESSAGE ORDER:
 *   [system] → [user] → [assistant] → [user] → [assistant] → [user]
 *              └─ history ─────────────────────────────────┘
 */
