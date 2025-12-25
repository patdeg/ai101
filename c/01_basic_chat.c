/**
 * ============================================================================
 * Example 1: Basic Chat - Your First AI API Call in C
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Making an HTTPS request to an AI API using libcurl
 *   - Building and parsing JSON with cJSON library
 *   - Working with environment variables in C
 *   - Proper memory management (malloc/free)
 *
 * WHAT YOU'LL LEARN:
 *   - libcurl basics: easy interface for HTTP requests
 *   - cJSON basics: creating and parsing JSON
 *   - String handling in C
 *   - Error checking patterns
 *
 * PREREQUISITES:
 *   - libcurl installed (see README.md)
 *   - cJSON in lib/ folder or installed system-wide
 *   - DEMETERICS_API_KEY environment variable set
 *
 * COMPILE:
 *   gcc -o 01_basic_chat 01_basic_chat.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./01_basic_chat
 *
 * ============================================================================
 */

#include <stdio.h>      /* printf, fprintf */
#include <stdlib.h>     /* malloc, free, getenv, exit */
#include <string.h>     /* strlen, strcpy */
#include <curl/curl.h>  /* libcurl for HTTP requests */
#include "cJSON.h"      /* cJSON for JSON handling */

/* ============================================================================
 * Step 1: Define a struct to hold the HTTP response
 * ============================================================================
 * In C, we need to manually manage memory for dynamic data.
 * This struct holds a growing string as data arrives from the server.
 */
struct ResponseBuffer {
    char *data;      /* Pointer to the response string */
    size_t size;     /* Current size of the string */
};

/* ============================================================================
 * Step 2: Callback function for libcurl to write response data
 * ============================================================================
 * libcurl calls this function each time it receives data from the server.
 * We append the received data to our ResponseBuffer.
 *
 * Parameters:
 *   contents - Pointer to the received data
 *   size     - Size of each element (always 1 for characters)
 *   nmemb    - Number of elements received
 *   userp    - Pointer to our ResponseBuffer (passed via CURLOPT_WRITEDATA)
 *
 * Returns:
 *   Number of bytes processed (must equal size * nmemb to continue)
 */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct ResponseBuffer *buf = (struct ResponseBuffer *)userp;

    /* Reallocate buffer to fit new data plus existing data plus null terminator */
    char *ptr = realloc(buf->data, buf->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Out of memory!\n");
        return 0;  /* Returning 0 tells curl to abort */
    }

    buf->data = ptr;
    memcpy(&(buf->data[buf->size]), contents, realsize);
    buf->size += realsize;
    buf->data[buf->size] = '\0';  /* Null-terminate the string */

    return realsize;
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main(void) {
    /* Step 3: Get API key from environment variable */
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY environment variable not set\n");
        fprintf(stderr, "Run: export DEMETERICS_API_KEY='your_key_here'\n");
        return 1;
    }

    /* Step 4: Initialize libcurl globally (must be called once) */
    curl_global_init(CURL_GLOBAL_ALL);

    /* Step 5: Create a curl "easy" handle for our request */
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        curl_global_cleanup();
        return 1;
    }

    /* Step 6: Build the JSON request body using cJSON */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();
    cJSON *message = cJSON_CreateObject();

    /* Add message content */
    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddStringToObject(message, "content", "What is the capital of Switzerland?");
    cJSON_AddItemToArray(messages, message);

    /* Add fields to root object */
    cJSON_AddStringToObject(root, "model", "meta-llama/llama-4-scout-17b-16e-instruct");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "temperature", 0.7);
    cJSON_AddNumberToObject(root, "max_tokens", 100);

    /* Convert cJSON object to string */
    char *json_payload = cJSON_PrintUnformatted(root);
    if (json_payload == NULL) {
        fprintf(stderr, "Error: Failed to create JSON\n");
        cJSON_Delete(root);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 7: Set up the HTTP headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* Build Authorization header with API key */
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 8: Initialize our response buffer */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);  /* Start with 1 byte (will grow as needed) */
    response.size = 0;
    response.data[0] = '\0';

    /* Step 9: Configure curl options */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

    /* Step 10: Perform the HTTP request */
    printf("Sending request to AI API...\n\n");
    CURLcode res = curl_easy_perform(curl);

    /* Step 11: Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl request failed: %s\n", curl_easy_strerror(res));
        free(response.data);
        free(json_payload);
        cJSON_Delete(root);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 12: Parse the JSON response */
    cJSON *response_json = cJSON_Parse(response.data);
    if (response_json == NULL) {
        fprintf(stderr, "Error: Failed to parse response JSON\n");
        printf("Raw response: %s\n", response.data);
        free(response.data);
        free(json_payload);
        cJSON_Delete(root);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 13: Display the full response (pretty-printed) */
    printf("Full Response:\n");
    char *pretty_response = cJSON_Print(response_json);
    printf("%s\n", pretty_response);
    free(pretty_response);

    /* Step 14: Extract and display the AI's answer */
    cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
    if (choices && cJSON_IsArray(choices) && cJSON_GetArraySize(choices) > 0) {
        cJSON *first_choice = cJSON_GetArrayItem(choices, 0);
        cJSON *msg = cJSON_GetObjectItem(first_choice, "message");
        cJSON *content = cJSON_GetObjectItem(msg, "content");
        if (content && cJSON_IsString(content)) {
            printf("\nAI Answer:\n%s\n", content->valuestring);
        }
    }

    /* Step 15: Display token usage */
    cJSON *usage = cJSON_GetObjectItem(response_json, "usage");
    if (usage) {
        cJSON *prompt_tokens = cJSON_GetObjectItem(usage, "prompt_tokens");
        cJSON *completion_tokens = cJSON_GetObjectItem(usage, "completion_tokens");
        cJSON *total_tokens = cJSON_GetObjectItem(usage, "total_tokens");

        printf("\nToken Usage:\n");
        if (prompt_tokens) printf("  Prompt: %d\n", prompt_tokens->valueint);
        if (completion_tokens) printf("  Response: %d\n", completion_tokens->valueint);
        if (total_tokens) printf("  Total: %d\n", total_tokens->valueint);
    }

    /* Step 16: Clean up all allocated resources */
    cJSON_Delete(response_json);  /* Free parsed response */
    cJSON_Delete(root);           /* Free request JSON */
    free(json_payload);           /* Free JSON string */
    free(response.data);          /* Free response buffer */
    curl_slist_free_all(headers); /* Free header list */
    curl_easy_cleanup(curl);      /* Free curl handle */
    curl_global_cleanup();        /* Clean up libcurl */

    return 0;
}

/*
 * C CONCEPTS EXPLAINED:
 *
 * #include <header.h>
 *   Includes a header file. <> for system headers, "" for local files.
 *
 * struct StructName { ... };
 *   Defines a custom data type that groups related variables together.
 *
 * size_t
 *   Unsigned integer type for sizes. Can't be negative.
 *   Always use size_t for array indices and memory sizes.
 *
 * void *
 *   "Void pointer" - a pointer that can point to any type.
 *   Must be cast to the correct type before dereferencing.
 *
 * malloc(size)
 *   Allocates 'size' bytes of memory on the heap.
 *   Returns NULL if allocation fails - always check!
 *
 * realloc(ptr, new_size)
 *   Resizes previously allocated memory.
 *   May move the data to a new location.
 *
 * free(ptr)
 *   Releases memory back to the system.
 *   Always free() what you malloc()!
 *
 * getenv("VAR_NAME")
 *   Gets an environment variable. Returns NULL if not set.
 *   Like $VAR_NAME in bash.
 *
 * snprintf(buf, size, format, ...)
 *   Safe string formatting (won't overflow buffer).
 *   Returns number of characters that would be written.
 *
 * fprintf(stderr, ...)
 *   Print to standard error stream (for error messages).
 *   stdout is for normal output, stderr is for errors.
 *
 * return 0 / return 1
 *   0 = success, non-zero = error (convention in C)
 *
 * MEMORY MANAGEMENT RULES:
 *   1. Every malloc() needs a matching free()
 *   2. Check malloc() return value for NULL
 *   3. Don't use memory after free()
 *   4. Don't free() the same pointer twice
 *   5. Initialize pointers to NULL if not immediately assigned
 *
 * LIBCURL PATTERN:
 *   1. curl_global_init() - once at program start
 *   2. curl_easy_init() - create a handle
 *   3. curl_easy_setopt() - configure options
 *   4. curl_easy_perform() - make the request
 *   5. curl_easy_cleanup() - free the handle
 *   6. curl_global_cleanup() - once at program end
 *
 * cJSON PATTERN:
 *   Creating: cJSON_CreateObject(), cJSON_AddStringToObject(), etc.
 *   Reading: cJSON_GetObjectItem(), cJSON_IsString(), ->valuestring
 *   Freeing: cJSON_Delete() on the root object (frees all children)
 */
