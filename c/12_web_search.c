/**
 * ============================================================================
 * Example 12: Web Search - Built-in Web Search with Compound Model
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using groq/compound-mini for built-in web search
 *   - Getting real-time information from the web
 *   - No separate search API needed
 *
 * WHAT YOU'LL LEARN:
 *   - Compound model capabilities
 *   - Web-grounded AI responses
 *   - Comparing to Tavily approach
 *
 * COMPILE:
 *   gcc -o 12_web_search 12_web_search.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./12_web_search
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

    /* Step 3: Define a query that needs current information */
    const char *user_query = "What are the latest tech news today?";

    printf("=== COMPOUND MODEL WEB SEARCH ===\n");
    printf("Using groq/compound-mini for built-in web search\n\n");
    printf("Query: %s\n\n", user_query);

    /* Step 4: Build request with compound model */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddStringToObject(message, "content", user_query);
    cJSON_AddItemToArray(messages, message);

    /* Use compound-mini which has built-in web search */
    cJSON_AddStringToObject(root, "model", "compound-mini");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "max_tokens", 500);

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

    printf("Searching the web...\n\n");
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
                    printf("=== WEB-GROUNDED RESPONSE ===\n\n");
                    printf("%s\n", content->valuestring);
                }
            }

            /* Token usage */
            cJSON *usage = cJSON_GetObjectItem(response_json, "usage");
            if (usage) {
                printf("\n=== TOKEN USAGE ===\n");
                cJSON *prompt = cJSON_GetObjectItem(usage, "prompt_tokens");
                cJSON *completion = cJSON_GetObjectItem(usage, "completion_tokens");
                if (prompt) printf("Prompt: %d\n", prompt->valueint);
                if (completion) printf("Response: %d\n", completion->valueint);
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
    printf("\n=== COMPOUND MODEL FEATURES ===\n\n");
    printf("WHAT IS COMPOUND-MINI?\n");
    printf("  A specialized model with built-in web search.\n");
    printf("  Automatically searches when queries need current info.\n\n");

    printf("COMPARISON:\n");
    printf("  Tavily + Llama:  Separate API calls, more control\n");
    printf("  Compound-mini:   Single call, automatic search\n\n");

    printf("WHEN TO USE:\n");
    printf("  - Quick web-grounded answers\n");
    printf("  - Current events and news\n");
    printf("  - Simple research questions\n\n");

    printf("WHEN TO USE TAVILY INSTEAD:\n");
    printf("  - Need full control over search\n");
    printf("  - Domain-specific filtering\n");
    printf("  - Complex RAG pipelines\n");

    return 0;
}

/*
 * COMPOUND MODEL NOTES:
 *
 * groq/compound-mini:
 *   - Has built-in web search capability
 *   - Automatically decides when to search
 *   - Returns web-grounded responses
 *
 * BENEFITS:
 *   - Simpler API integration
 *   - One endpoint for everything
 *   - No need to manage search separately
 *
 * LIMITATIONS:
 *   - Less control over search parameters
 *   - Can't filter domains easily
 *   - May not search when you expect it to
 */
