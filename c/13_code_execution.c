/**
 * ============================================================================
 * Example 13: Code Execution - Python Sandbox with AI
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using AI models that can execute Python code
 *   - Sandboxed code execution
 *   - Getting computed results from AI
 *
 * WHAT YOU'LL LEARN:
 *   - Code interpreter capabilities
 *   - Safe code execution patterns
 *   - When to use code execution
 *
 * COMPILE:
 *   gcc -o 13_code_execution 13_code_execution.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./13_code_execution
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

    /* Step 3: Define a problem that benefits from code execution */
    const char *user_query =
        "Calculate the first 10 Fibonacci numbers and find their sum. "
        "Show me the Python code you use and the result.";

    printf("=== AI CODE EXECUTION ===\n");
    printf("Using openai/gpt-oss-20b for code execution\n\n");
    printf("Query: %s\n\n", user_query);

    /* Step 4: Build request */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();

    /* System prompt to encourage code execution */
    cJSON *system_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(system_msg, "role", "system");
    cJSON_AddStringToObject(system_msg, "content",
        "You are a helpful assistant that solves problems by writing and executing Python code. "
        "Always show your code and explain the results.");
    cJSON_AddItemToArray(messages, system_msg);

    cJSON *user_msg = cJSON_CreateObject();
    cJSON_AddStringToObject(user_msg, "role", "user");
    cJSON_AddStringToObject(user_msg, "content", user_query);
    cJSON_AddItemToArray(messages, user_msg);

    /* Use model with code execution capability */
    cJSON_AddStringToObject(root, "model", "openai/gpt-oss-20b");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "max_tokens", 1000);

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

    printf("Executing code...\n\n");
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
                    printf("=== AI RESPONSE WITH CODE ===\n\n");
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
    printf("\n=== CODE EXECUTION FEATURES ===\n\n");
    printf("WHAT CAN IT DO?\n");
    printf("  - Run Python code in sandbox\n");
    printf("  - Complex calculations\n");
    printf("  - Data analysis\n");
    printf("  - Generate charts (returned as base64)\n\n");

    printf("SAFETY:\n");
    printf("  - Sandboxed environment\n");
    printf("  - No network access\n");
    printf("  - Limited file system\n");
    printf("  - Time and memory limits\n\n");

    printf("USE CASES:\n");
    printf("  - Math homework help\n");
    printf("  - Data analysis tasks\n");
    printf("  - Algorithm verification\n");
    printf("  - Chart generation\n");

    return 0;
}

/*
 * CODE EXECUTION NOTES:
 *
 * MODELS WITH CODE EXECUTION:
 *   - openai/gpt-oss-20b: Has code interpreter
 *   - Other models: Can write code but not execute
 *
 * SANDBOXING:
 *   Code runs in isolated environment:
 *   - No access to real file system
 *   - No network requests
 *   - Limited CPU time
 *   - Limited memory
 *
 * WHEN TO USE:
 *   - Precise calculations needed
 *   - Data processing required
 *   - Visualization generation
 *   - Algorithm testing
 */
