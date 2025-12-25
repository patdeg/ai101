/**
 * ============================================================================
 * Example 10: Tavily Extract - Web Content Extraction
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using Tavily Extract API to get clean content from URLs
 *   - Extracting readable text from web pages
 *   - Preparing web content for AI processing
 *
 * WHAT YOU'LL LEARN:
 *   - Web scraping vs API extraction
 *   - Content cleaning for AI
 *   - Working with multiple URLs
 *
 * COMPILE:
 *   gcc -o 10_tavily_extract 10_tavily_extract.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./10_tavily_extract
 *
 * NOTE: Requires TAVILY_API_KEY environment variable
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
    const char *api_key = getenv("TAVILY_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: TAVILY_API_KEY environment variable not set\n");
        fprintf(stderr, "Get your free API key at: https://tavily.com\n");
        return 1;
    }

    /* Step 2: Define URLs to extract */
    const char *urls[] = {
        "https://en.wikipedia.org/wiki/Artificial_intelligence"
    };
    int num_urls = sizeof(urls) / sizeof(urls[0]);

    printf("=== TAVILY CONTENT EXTRACTOR ===\n");
    printf("Extracting clean content from web pages\n\n");

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build JSON request for Tavily Extract */
    cJSON *root = cJSON_CreateObject();
    cJSON *urls_array = cJSON_CreateArray();

    for (int i = 0; i < num_urls; i++) {
        cJSON_AddItemToArray(urls_array, cJSON_CreateString(urls[i]));
    }

    cJSON_AddStringToObject(root, "api_key", api_key);
    cJSON_AddItemToObject(root, "urls", urls_array);

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 5: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* Step 6: Configure curl */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.tavily.com/extract");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    /* Step 7: Perform extraction */
    printf("Extracting content from %d URL(s)...\n\n", num_urls);
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    } else {
        /* Step 8: Parse and display results */
        cJSON *response_json = cJSON_Parse(response.data);

        if (response_json) {
            cJSON *results = cJSON_GetObjectItem(response_json, "results");

            if (results && cJSON_IsArray(results)) {
                int result_count = cJSON_GetArraySize(results);

                for (int i = 0; i < result_count; i++) {
                    cJSON *result = cJSON_GetArrayItem(results, i);

                    cJSON *url = cJSON_GetObjectItem(result, "url");
                    cJSON *raw_content = cJSON_GetObjectItem(result, "raw_content");

                    printf("=== EXTRACTED CONTENT ===\n");
                    if (url) printf("URL: %s\n\n", url->valuestring);

                    if (raw_content && cJSON_IsString(raw_content)) {
                        /* Show first 1000 characters */
                        char preview[1001];
                        strncpy(preview, raw_content->valuestring, 1000);
                        preview[1000] = '\0';

                        printf("Content Preview:\n%s\n", preview);

                        if (strlen(raw_content->valuestring) > 1000) {
                            printf("\n... [Content truncated, total: %zu characters]\n",
                                   strlen(raw_content->valuestring));
                        }
                    }
                    printf("\n");
                }
            }

            /* Show failed URLs if any */
            cJSON *failed = cJSON_GetObjectItem(response_json, "failed_results");
            if (failed && cJSON_IsArray(failed) && cJSON_GetArraySize(failed) > 0) {
                printf("=== FAILED EXTRACTIONS ===\n");
                int failed_count = cJSON_GetArraySize(failed);
                for (int i = 0; i < failed_count; i++) {
                    cJSON *fail = cJSON_GetArrayItem(failed, i);
                    cJSON *fail_url = cJSON_GetObjectItem(fail, "url");
                    cJSON *error = cJSON_GetObjectItem(fail, "error");
                    if (fail_url) printf("URL: %s\n", fail_url->valuestring);
                    if (error) printf("Error: %s\n", error->valuestring);
                }
            }

            cJSON_Delete(response_json);
        } else {
            printf("Raw response: %s\n", response.data);
        }
    }

    /* Step 9: Clean up */
    free(response.data);
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    printf("\n=== TAVILY EXTRACT FEATURES ===\n\n");
    printf("WHAT IT DOES:\n");
    printf("  - Fetches web pages\n");
    printf("  - Removes ads, navigation, scripts\n");
    printf("  - Returns clean, readable text\n");
    printf("  - Handles JavaScript-rendered content\n\n");

    printf("USE CASES:\n");
    printf("  1. RAG (Retrieval Augmented Generation)\n");
    printf("  2. Document summarization\n");
    printf("  3. Content analysis\n");
    printf("  4. Research automation\n\n");

    printf("LIMITS:\n");
    printf("  - Multiple URLs per request supported\n");
    printf("  - Rate limits apply (check your plan)\n");
    printf("  - Some sites may block extraction\n");

    return 0;
}

/*
 * WEB EXTRACTION NOTES:
 *
 * WHY USE AN EXTRACTION API?
 *   - Web pages have lots of noise (ads, scripts, navigation)
 *   - JavaScript rendering is complex
 *   - Clean text is better for AI processing
 *
 * TAVILY EXTRACT vs SCRAPING:
 *   - No need to parse HTML yourself
 *   - Handles anti-bot measures
 *   - Returns structured, clean content
 *   - Legal considerations (respects robots.txt)
 *
 * RESPONSE STRUCTURE:
 *   {
 *     "results": [
 *       {"url": "...", "raw_content": "cleaned text..."}
 *     ],
 *     "failed_results": [
 *       {"url": "...", "error": "reason"}
 *     ]
 *   }
 */
