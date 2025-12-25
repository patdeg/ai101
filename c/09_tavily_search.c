/**
 * ============================================================================
 * Example 9: Tavily Search - Web Search with AI
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using Tavily API for web search
 *   - Getting AI-powered search results
 *   - Working with a different API (not Groq)
 *
 * WHAT YOU'LL LEARN:
 *   - Tavily API structure
 *   - Web search integration
 *   - Parsing search results
 *
 * COMPILE:
 *   gcc -o 09_tavily_search 09_tavily_search.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./09_tavily_search
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

    /* Step 2: Define search query */
    const char *search_query = "What are the latest developments in renewable energy 2024?";

    printf("=== TAVILY WEB SEARCH ===\n");
    printf("Query: %s\n\n", search_query);

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build JSON request for Tavily */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "api_key", api_key);
    cJSON_AddStringToObject(root, "query", search_query);
    cJSON_AddStringToObject(root, "search_depth", "basic");  /* "basic" or "advanced" */
    cJSON_AddBoolToObject(root, "include_answer", 1);        /* Get AI-generated answer */
    cJSON_AddNumberToObject(root, "max_results", 5);         /* Number of results */

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 5: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* Step 6: Configure curl */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.tavily.com/search");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    /* Step 7: Perform search */
    printf("Searching the web...\n\n");
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    } else {
        /* Step 8: Parse and display results */
        cJSON *response_json = cJSON_Parse(response.data);

        if (response_json) {
            /* Display AI-generated answer */
            cJSON *answer = cJSON_GetObjectItem(response_json, "answer");
            if (answer && cJSON_IsString(answer)) {
                printf("=== AI ANSWER ===\n");
                printf("%s\n\n", answer->valuestring);
            }

            /* Display search results */
            cJSON *results = cJSON_GetObjectItem(response_json, "results");
            if (results && cJSON_IsArray(results)) {
                printf("=== SEARCH RESULTS ===\n\n");

                int result_count = cJSON_GetArraySize(results);
                for (int i = 0; i < result_count; i++) {
                    cJSON *result = cJSON_GetArrayItem(results, i);

                    cJSON *title = cJSON_GetObjectItem(result, "title");
                    cJSON *url = cJSON_GetObjectItem(result, "url");
                    cJSON *content = cJSON_GetObjectItem(result, "content");
                    cJSON *score = cJSON_GetObjectItem(result, "score");

                    printf("Result %d:\n", i + 1);
                    if (title) printf("  Title: %s\n", title->valuestring);
                    if (url) printf("  URL: %s\n", url->valuestring);
                    if (score) printf("  Relevance: %.2f\n", score->valuedouble);
                    if (content) {
                        /* Truncate long content */
                        char snippet[201];
                        strncpy(snippet, content->valuestring, 200);
                        snippet[200] = '\0';
                        printf("  Snippet: %s...\n", snippet);
                    }
                    printf("\n");
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
    printf("=== TAVILY API FEATURES ===\n\n");
    printf("SEARCH DEPTHS:\n");
    printf("  basic    - Fast, surface-level results\n");
    printf("  advanced - Deeper search, more comprehensive\n\n");

    printf("PARAMETERS:\n");
    printf("  query           - Your search query\n");
    printf("  include_answer  - Get AI-synthesized answer\n");
    printf("  max_results     - Number of results (1-10)\n");
    printf("  search_depth    - basic or advanced\n");
    printf("  include_domains - Only search specific domains\n");
    printf("  exclude_domains - Skip specific domains\n\n");

    printf("USE CASES:\n");
    printf("  1. Research assistants\n");
    printf("  2. Fact-checking tools\n");
    printf("  3. News aggregation\n");
    printf("  4. RAG (Retrieval Augmented Generation)\n");

    return 0;
}

/*
 * TAVILY API NOTES:
 *
 * Tavily is designed specifically for AI applications:
 *   - Returns clean, structured content
 *   - Provides AI-synthesized answers
 *   - Optimized for RAG pipelines
 *
 * AUTHENTICATION:
 *   - API key in request body (not header)
 *   - Free tier available
 *
 * RESPONSE STRUCTURE:
 *   {
 *     "answer": "AI-generated summary...",
 *     "results": [
 *       {"title": "...", "url": "...", "content": "...", "score": 0.95}
 *     ],
 *     "query": "original query"
 *   }
 */
