/**
 * ============================================================================
 * Example 6: Image Safety Check - Vision Content Moderation
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using LlamaGuard with vision capabilities
 *   - Checking images for unsafe content
 *   - Combining base64 encoding with safety models
 *
 * WHY IMAGE SAFETY MATTERS:
 *   - User-uploaded images may contain inappropriate content
 *   - AI-generated images should be verified before display
 *   - Legal requirements for content moderation
 *
 * COMPILE:
 *   gcc -o 06_image_safety_check 06_image_safety_check.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./06_image_safety_check
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

/* Base64 encoding */
static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = 4 * ((input_length + 2) / 3);
    char *encoded = malloc(*output_length + 1);
    if (encoded == NULL) return NULL;

    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        encoded[j++] = base64_table[(triple >> 18) & 0x3F];
        encoded[j++] = base64_table[(triple >> 12) & 0x3F];
        encoded[j++] = base64_table[(triple >> 6) & 0x3F];
        encoded[j++] = base64_table[triple & 0x3F];
    }

    int mod = input_length % 3;
    if (mod > 0) {
        encoded[*output_length - 1] = '=';
        if (mod == 1) encoded[*output_length - 2] = '=';
    }
    encoded[*output_length] = '\0';
    return encoded;
}

/* Read file into memory */
unsigned char *read_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return NULL;
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char *buffer = malloc(*size);
    if (buffer && fread(buffer, 1, *size, file) != *size) {
        free(buffer);
        buffer = NULL;
    }
    fclose(file);
    return buffer;
}

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Read and encode image */
    const char *image_path = "../test_image.jpg";
    printf("=== IMAGE SAFETY CHECKER ===\n");
    printf("Using LlamaGuard Vision to check image safety\n\n");
    printf("Checking image: %s\n", image_path);

    size_t image_size;
    unsigned char *image_data = read_file(image_path, &image_size);
    if (image_data == NULL) {
        fprintf(stderr, "Error: Could not read image file\n");
        fprintf(stderr, "Make sure test_image.jpg exists in parent directory\n");
        return 1;
    }

    size_t encoded_length;
    char *image_base64 = base64_encode(image_data, image_size, &encoded_length);
    free(image_data);

    if (image_base64 == NULL) {
        fprintf(stderr, "Error: Failed to encode image\n");
        return 1;
    }

    printf("Image size: %zu bytes\n", image_size);
    printf("Encoded length: %zu characters\n\n", encoded_length);

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        free(image_base64);
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build JSON request with image for LlamaGuard */
    size_t data_url_len = strlen("data:image/jpeg;base64,") + encoded_length + 1;
    char *data_url = malloc(data_url_len);
    snprintf(data_url, data_url_len, "data:image/jpeg;base64,%s", image_base64);
    free(image_base64);

    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();
    cJSON *message = cJSON_CreateObject();
    cJSON *content = cJSON_CreateArray();

    /* Image content for safety check */
    cJSON *image_part = cJSON_CreateObject();
    cJSON_AddStringToObject(image_part, "type", "image_url");
    cJSON *image_url_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(image_url_obj, "url", data_url);
    cJSON_AddItemToObject(image_part, "image_url", image_url_obj);
    cJSON_AddItemToArray(content, image_part);

    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddItemToObject(message, "content", content);
    cJSON_AddItemToArray(messages, message);

    /* Use LlamaGuard model */
    cJSON_AddStringToObject(root, "model", "meta-llama/llama-guard-4-12b");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "max_tokens", 100);

    char *json_payload = cJSON_PrintUnformatted(root);
    free(data_url);

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

    printf("Analyzing image for safety...\n\n");
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    } else {
        /* Step 7: Parse and display result */
        cJSON *response_json = cJSON_Parse(response.data);
        if (response_json) {
            cJSON *choices = cJSON_GetObjectItem(response_json, "choices");
            if (choices && cJSON_GetArraySize(choices) > 0) {
                cJSON *first = cJSON_GetArrayItem(choices, 0);
                cJSON *msg = cJSON_GetObjectItem(first, "message");
                cJSON *msg_content = cJSON_GetObjectItem(msg, "content");

                if (msg_content && cJSON_IsString(msg_content)) {
                    const char *result = msg_content->valuestring;

                    if (strncmp(result, "safe", 4) == 0) {
                        printf("Result: SAFE\n");
                        printf("The image does not contain unsafe content.\n");
                    } else {
                        printf("Result: UNSAFE\n");
                        printf("Raw response: %s\n", result);
                    }
                }
            }
            cJSON_Delete(response_json);
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
    printf("\n=== IMAGE SAFETY CATEGORIES ===\n\n");
    printf("LlamaGuard checks images for:\n");
    printf("- Violence and gore\n");
    printf("- Adult/sexual content\n");
    printf("- Hate symbols\n");
    printf("- Dangerous activities\n");
    printf("- Child safety issues\n\n");

    printf("=== WHEN TO USE IMAGE SAFETY ===\n\n");
    printf("1. USER UPLOADS: Check all user-submitted images\n");
    printf("2. AI GENERATION: Verify AI-generated images before display\n");
    printf("3. CONTENT FEEDS: Filter images in social features\n");
    printf("4. AVATARS: Validate profile pictures\n");

    return 0;
}
