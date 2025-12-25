/**
 * ============================================================================
 * Example 3: Prompt Template - Dynamic Prompt Generation
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Building dynamic prompts from templates
 *   - String replacement in C (manual template system)
 *   - Separating prompt structure from content
 *
 * WHAT YOU'LL LEARN:
 *   - Template patterns for reusable prompts
 *   - String manipulation in C
 *   - How to build flexible AI interactions
 *
 * COMPILE:
 *   gcc -o 03_prompt_template 03_prompt_template.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./03_prompt_template
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

/* ============================================================================
 * Helper function: Simple string replacement
 * ============================================================================
 * Replaces all occurrences of 'search' with 'replace' in 'template'.
 * Returns a newly allocated string (caller must free).
 */
char *replace_placeholder(const char *template_str, const char *search, const char *replace) {
    /* Count occurrences to calculate new size */
    int count = 0;
    const char *tmp = template_str;
    size_t search_len = strlen(search);
    size_t replace_len = strlen(replace);

    while ((tmp = strstr(tmp, search)) != NULL) {
        count++;
        tmp += search_len;
    }

    /* Allocate result buffer */
    size_t result_len = strlen(template_str) + count * (replace_len - search_len) + 1;
    char *result = malloc(result_len);
    if (result == NULL) return NULL;

    /* Perform replacement */
    char *dest = result;
    const char *src = template_str;

    while (*src) {
        if (strncmp(src, search, search_len) == 0) {
            strcpy(dest, replace);
            dest += replace_len;
            src += search_len;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';

    return result;
}

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        return 1;
    }

    /* ========================================================================
     * Step 2: Define our prompt template
     * ========================================================================
     * Templates let us reuse the same prompt structure with different values.
     * We use {{placeholder}} syntax (like many template engines).
     */

    const char *template_str =
        "You are a {{ROLE}} expert.\n"
        "Explain {{TOPIC}} to someone who is {{LEVEL}}.\n"
        "Keep your explanation under {{MAX_WORDS}} words.\n"
        "Focus on practical examples.";

    /* Step 3: Define our template variables */
    const char *role = "computer science";
    const char *topic = "recursion";
    const char *level = "a beginner programmer";
    const char *max_words = "100";

    /* Step 4: Replace placeholders to build final prompt */
    char *prompt = replace_placeholder(template_str, "{{ROLE}}", role);
    char *prompt2 = replace_placeholder(prompt, "{{TOPIC}}", topic);
    free(prompt);
    char *prompt3 = replace_placeholder(prompt2, "{{LEVEL}}", level);
    free(prompt2);
    char *final_prompt = replace_placeholder(prompt3, "{{MAX_WORDS}}", max_words);
    free(prompt3);

    printf("=== GENERATED PROMPT ===\n");
    printf("%s\n", final_prompt);
    printf("========================\n\n");

    /* Step 5: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        free(final_prompt);
        curl_global_cleanup();
        return 1;
    }

    /* Step 6: Build JSON request */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();

    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddStringToObject(message, "content", final_prompt);
    cJSON_AddItemToArray(messages, message);

    cJSON_AddStringToObject(root, "model", "meta-llama/llama-4-scout-17b-16e-instruct");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "temperature", 0.7);
    cJSON_AddNumberToObject(root, "max_tokens", 200);

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 7: Set up headers and response buffer */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    /* Step 8: Configure and perform request */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    printf("Sending templated request...\n\n");
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    } else {
        /* Step 9: Parse and display response */
        cJSON *response_json = cJSON_Parse(response.data);
        if (response_json) {
            cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
            if (choices && cJSON_GetArraySize(choices) > 0) {
                cJSON *first = cJSON_GetArrayItem(choices, 0);
                cJSON *msg = cJSON_GetObjectItem(first, "message");
                cJSON *content = cJSON_GetObjectItem(msg, "content");
                if (content && cJSON_IsString(content)) {
                    printf("AI Response:\n%s\n", content->valuestring);
                }
            }
            cJSON_Delete(response_json);
        }
    }

    /* Step 10: Clean up */
    free(response.data);
    free(json_payload);
    free(final_prompt);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Show template examples */
    printf("\n=== MORE TEMPLATE IDEAS ===\n\n");
    printf("TRANSLATION TEMPLATE:\n");
    printf("  \"Translate the following {{SOURCE_LANG}} text to {{TARGET_LANG}}:\n");
    printf("   {{TEXT}}\"\n\n");

    printf("CODE REVIEW TEMPLATE:\n");
    printf("  \"Review this {{LANGUAGE}} code for {{REVIEW_TYPE}}:\n");
    printf("   {{CODE}}\"\n\n");

    printf("SUMMARIZATION TEMPLATE:\n");
    printf("  \"Summarize the following {{CONTENT_TYPE}} in {{FORMAT}}:\n");
    printf("   {{CONTENT}}\"\n");

    return 0;
}

/*
 * TEMPLATE PATTERNS:
 *
 * 1. Simple placeholders: {{VARIABLE}}
 *    Easy to spot, common convention
 *
 * 2. With defaults: {{VARIABLE:default_value}}
 *    Requires more complex parsing
 *
 * 3. Conditionals: {{#if CONDITION}}...{{/if}}
 *    For advanced template engines
 *
 * WHY TEMPLATES?
 *
 * 1. Reusability - Same prompt structure, different content
 * 2. Testability - Easy to test with known inputs
 * 3. Maintainability - Change prompt format in one place
 * 4. Separation - Content creators don't need to understand code
 *
 * C STRING FUNCTIONS USED:
 *
 * strlen(s)        - Get string length
 * strstr(s, sub)   - Find substring, returns pointer or NULL
 * strncmp(a, b, n) - Compare first n characters
 * strcpy(dst, src) - Copy string (careful: no bounds checking!)
 * malloc(size)     - Allocate memory
 * free(ptr)        - Release memory
 */
