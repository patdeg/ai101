/**
 * ============================================================================
 * Example 14: Reasoning - Step-by-Step Thinking with Prompt Caching
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using AI for complex reasoning tasks
 *   - Chain-of-thought prompting
 *   - Prompt caching for cost savings
 *
 * WHAT YOU'LL LEARN:
 *   - How to get AI to "think" through problems
 *   - Structured reasoning prompts
 *   - Caching benefits for repeated queries
 *
 * COMPILE:
 *   gcc -o 14_reasoning 14_reasoning.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./14_reasoning
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

/* Response buffer */
struct ResponseBuffer {
    char *data;
    size_t size;
};

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

    /* Step 3: Define a problem requiring reasoning */
    const char *reasoning_problem =
        "A farmer has 17 sheep. All but 9 run away. How many sheep does the farmer have left?\n\n"
        "Think through this step-by-step before giving your final answer.";

    printf("=== AI REASONING ===\n");
    printf("Using chain-of-thought prompting for complex problems\n\n");
    printf("Problem: %s\n\n", reasoning_problem);

    /* Step 4: Build request with reasoning system prompt */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();

    /* System prompt optimized for reasoning (can be cached) */
    cJSON *system_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(system_msg, "role", "system");
    cJSON_AddStringToObject(system_msg, "content",
        "You are a logical reasoning assistant. When solving problems:\n"
        "1. Read the problem carefully - note any tricky wording\n"
        "2. Identify what is being asked\n"
        "3. Work through the logic step by step\n"
        "4. Check your reasoning for errors\n"
        "5. Provide a clear final answer\n\n"
        "Take your time and think carefully before answering.");
    cJSON_AddItemToArray(messages, system_msg);

    cJSON *user_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(user_msg, "role", "user");
    cJSON_AddStringToObject(user_msg, "content", reasoning_problem);
    cJSON_AddItemToArray(messages, user_msg);

    /* Use model good for reasoning */
    cJSON_AddStringToObject(root, "model", "openai/gpt-oss-20b");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "max_tokens", 500);
    cJSON_AddNumberToObject(root, "temperature", 0.3);  /* Lower = more focused */

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 5: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 6: Configure and perform request */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    printf("AI is thinking...\n\n");
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    } else {
        /* Step 7: Parse and display results */
        cJSON *response_json = cJSON_Parse(response.data);

        if (response_json) {
            cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
            if (choices && cJSON_GetArraySize(choices) > 0) {
                cJSON *first = cJSON_GetArrayItem(choices, 0);
                cJSON *msg = cJSON_GetObjectItem(first, "message");
                cJSON *content = cJSON_GetObjectItem(msg, "content");

                if (content && cJSON_IsString(content)) {
                    printf("=== AI REASONING CHAIN ===\n\n");
                    printf("%s\n", content->valuestring);
                }
            }

            /* Token usage - check for caching */
            cJSON *usage = cJSON_GetObjectItem(response_json, "usage");
            if (usage) {
                printf("\n=== TOKEN USAGE ===\n");
                cJSON *prompt = cJSON_GetObjectItem(usage, "prompt_tokens");
                cJSON *completion = cJSON_GetObjectItem(usage, "completion_tokens");
                cJSON *cached = cJSON_GetObjectItem(usage, "prompt_cache_hit_tokens");

                if (prompt) printf("Prompt tokens: %d\n", prompt->valueint);
                if (completion) printf("Completion tokens: %d\n", completion->valueint);
                if (cached && cached->valueint > 0) {
                    printf("Cached tokens: %d (50%% discount!)\n", cached->valueint);
                }
            }

            cJSON_Delete(response_json);
        } else {
            printf("Raw response: %s\n", response.data);
        }
    }

    /* Step 8: Clean up */
    free(response.data);
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    printf("\n=== REASONING TECHNIQUES ===\n\n");
    printf("CHAIN-OF-THOUGHT PROMPTING:\n");
    printf("  Ask the AI to \"think step by step\"\n");
    printf("  Helps with math, logic, and complex problems\n\n");

    printf("PROMPT CACHING:\n");
    printf("  - System prompts can be cached\n");
    printf("  - Cached tokens cost 50%% less\n");
    printf("  - Great for repeated similar queries\n\n");

    printf("TIPS FOR BETTER REASONING:\n");
    printf("  1. Use lower temperature (0.1-0.3)\n");
    printf("  2. Ask to \"check your work\"\n");
    printf("  3. Break complex problems into parts\n");
    printf("  4. Use explicit step-by-step instructions\n");

    return 0;
}

/*
 * REASONING NOTES:
 *
 * CHAIN-OF-THOUGHT:
 *   Just adding "think step by step" can dramatically
 *   improve AI performance on reasoning tasks.
 *
 * PROMPT CACHING:
 *   - System prompts repeated across requests can be cached
 *   - Reduces latency and cost
 *   - Especially useful for:
 *     - Long system prompts
 *     - High-volume applications
 *     - Similar repeated queries
 *
 * WHEN REASONING HELPS:
 *   - Math word problems
 *   - Logic puzzles
 *   - Multi-step calculations
 *   - Tricky questions with wordplay
 */
