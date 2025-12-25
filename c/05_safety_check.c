/**
 * ============================================================================
 * Example 5: Safety Check - Content Moderation with LlamaGuard
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using AI safety models to check content
 *   - LlamaGuard for text content moderation
 *   - Building responsible AI applications
 *
 * WHY SAFETY MATTERS:
 *   - AI should refuse harmful requests
 *   - Content moderation protects users
 *   - Responsible AI is good AI
 *
 * COMPILE:
 *   gcc -o 05_safety_check 05_safety_check.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./05_safety_check
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

/* Function to check if text is safe using LlamaGuard */
int check_safety(const char *api_key, const char *text_to_check, int *is_safe, char *category, size_t category_size) {
    CURL *curl = curl_easy_init();
    if (curl == NULL) return -1;

    /* Build JSON request for LlamaGuard */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();
    cJSON *message = cJSON_CreateObject();

    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddStringToObject(message, "content", text_to_check);
    cJSON_AddItemToArray(messages, message);

    cJSON_AddStringToObject(root, "model", "meta-llama/llama-guard-4-12b");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "max_tokens", 100);

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Initialize response buffer */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    /* Configure curl */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    int result = -1;

    if (res == CURLE_OK) {
        cJSON *response_json = cJSON_Parse(response.data);
        if (response_json) {
            cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
            if (choices && cJSON_GetArraySize(choices) > 0) {
                cJSON *first = cJSON_GetArrayItem(choices, 0);
                cJSON *msg = cJSON_GetObjectItem(first, "message");
                cJSON *content = cJSON_GetObjectItem(msg, "content");

                if (content && cJSON_IsString(content)) {
                    const char *response_text = content->valuestring;

                    /* LlamaGuard returns "safe" or "unsafe\n<category>" */
                    if (strncmp(response_text, "safe", 4) == 0) {
                        *is_safe = 1;
                        strncpy(category, "none", category_size);
                    } else {
                        *is_safe = 0;
                        /* Extract category if present */
                        const char *newline = strchr(response_text, '\n');
                        if (newline && *(newline + 1)) {
                            strncpy(category, newline + 1, category_size - 1);
                            category[category_size - 1] = '\0';
                        } else {
                            strncpy(category, "unknown", category_size);
                        }
                    }
                    result = 0;
                }
            }
            cJSON_Delete(response_json);
        }
    }

    /* Clean up */
    free(response.data);
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return result;
}

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Initialize curl globally */
    curl_global_init(CURL_GLOBAL_ALL);

    printf("=== AI CONTENT SAFETY CHECKER ===\n");
    printf("Using LlamaGuard to check content safety\n\n");

    /* Step 3: Test with various inputs */
    const char *test_inputs[] = {
        "What is the capital of France?",
        "How do I bake chocolate chip cookies?",
        "Explain quantum computing in simple terms",
        "Tell me a joke about programmers"
    };

    int num_tests = sizeof(test_inputs) / sizeof(test_inputs[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("Checking: \"%s\"\n", test_inputs[i]);

        int is_safe = 0;
        char category[256] = {0};

        int result = check_safety(api_key, test_inputs[i], &is_safe, category, sizeof(category));

        if (result == 0) {
            if (is_safe) {
                printf("  Result: SAFE\n");
            } else {
                printf("  Result: UNSAFE\n");
                printf("  Category: %s\n", category);
            }
        } else {
            printf("  Error: Could not check safety\n");
        }
        printf("\n");
    }

    /* Step 4: Clean up */
    curl_global_cleanup();

    /* Print educational information */
    printf("=== LLAMAGUARD SAFETY CATEGORIES ===\n\n");
    printf("S1: Violent Crimes\n");
    printf("S2: Non-Violent Crimes\n");
    printf("S3: Sex Crimes\n");
    printf("S4: Child Exploitation\n");
    printf("S5: Defamation\n");
    printf("S6: Specialized Advice (medical, legal, financial)\n");
    printf("S7: Privacy\n");
    printf("S8: Intellectual Property\n");
    printf("S9: Indiscriminate Weapons\n");
    printf("S10: Hate\n");
    printf("S11: Self-Harm\n");
    printf("S12: Sexual Content\n");
    printf("S13: Elections\n");
    printf("S14: Code Interpreter Abuse\n\n");

    printf("=== WHY SAFETY MATTERS ===\n\n");
    printf("1. PROTECT USERS:\n");
    printf("   - Prevent exposure to harmful content\n");
    printf("   - Keep minors safe\n\n");
    printf("2. LEGAL COMPLIANCE:\n");
    printf("   - Many jurisdictions require content moderation\n");
    printf("   - Avoid liability issues\n\n");
    printf("3. RESPONSIBLE AI:\n");
    printf("   - AI should help, not harm\n");
    printf("   - Build trust with users\n\n");
    printf("4. BEST PRACTICE:\n");
    printf("   - Check BOTH user input AND AI output\n");
    printf("   - Defense in depth!\n");

    return 0;
}

/*
 * SAFETY CHECK PATTERN:
 *
 * 1. CHECK USER INPUT
 *    Before sending to main AI, check if input is safe
 *    if (!is_safe(user_input)) {
 *        return "I can't help with that request.";
 *    }
 *
 * 2. CHECK AI OUTPUT
 *    Before showing to user, check if output is safe
 *    if (!is_safe(ai_response)) {
 *        return "I apologize, I generated inappropriate content.";
 *    }
 *
 * 3. DEFENSE IN DEPTH
 *    Multiple layers of protection:
 *    - Input validation
 *    - Prompt engineering (system prompts with rules)
 *    - Output filtering
 *    - Rate limiting
 *    - Logging for review
 *
 * LLAMAGUARD RESPONSE FORMAT:
 *    Safe content: "safe"
 *    Unsafe content: "unsafe\nS1" (category code after newline)
 */
