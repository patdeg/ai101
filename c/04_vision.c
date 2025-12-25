/**
 * ============================================================================
 * Example 4: Vision - Image Analysis with AI
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Sending images to AI vision models
 *   - Base64 encoding images in C
 *   - Multimodal (text + image) API requests
 *
 * WHAT YOU'LL LEARN:
 *   - How vision models process images
 *   - Base64 encoding for binary data
 *   - File I/O in C
 *   - Building complex nested JSON
 *
 * COMPILE:
 *   gcc -o 04_vision 04_vision.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./04_vision
 *
 * NOTE: Requires test_image.jpg in the parent directory
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
 * Base64 encoding table and function
 * ============================================================================
 * Base64 converts binary data to ASCII text using 64 safe characters.
 * This is necessary because JSON can't contain raw binary data.
 */
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

    /* Add padding */
    int mod = input_length % 3;
    if (mod > 0) {
        encoded[*output_length - 1] = '=';
        if (mod == 1) {
            encoded[*output_length - 2] = '=';
        }
    }

    encoded[*output_length] = '\0';
    return encoded;
}

/* Read entire file into memory */
unsigned char *read_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file: %s\n", filename);
        return NULL;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* Allocate buffer and read */
    unsigned char *buffer = malloc(*size);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, *size, file) != *size) {
        free(buffer);
        fclose(file);
        return NULL;
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

    /* Step 2: Read and encode image file */
    const char *image_path = "../test_image.jpg";
    printf("Reading image: %s\n", image_path);

    size_t image_size;
    unsigned char *image_data = read_file(image_path, &image_size);
    if (image_data == NULL) {
        fprintf(stderr, "Error: Could not read image file\n");
        fprintf(stderr, "Make sure test_image.jpg exists in the parent directory\n");
        return 1;
    }

    printf("Image size: %zu bytes\n", image_size);

    /* Step 3: Base64 encode the image */
    size_t encoded_length;
    char *image_base64 = base64_encode(image_data, image_size, &encoded_length);
    free(image_data);  /* Don't need raw image anymore */

    if (image_base64 == NULL) {
        fprintf(stderr, "Error: Failed to encode image\n");
        return 1;
    }

    printf("Base64 length: %zu characters\n\n", encoded_length);

    /* Step 4: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        free(image_base64);
        curl_global_cleanup();
        return 1;
    }

    /* ========================================================================
     * Step 5: Build JSON request with image
     * ========================================================================
     * Vision models accept images as base64-encoded data URLs.
     * Format: "data:image/jpeg;base64,<base64_data>"
     */

    /* Build the data URL */
    size_t data_url_len = strlen("data:image/jpeg;base64,") + encoded_length + 1;
    char *data_url = malloc(data_url_len);
    snprintf(data_url, data_url_len, "data:image/jpeg;base64,%s", image_base64);
    free(image_base64);

    /* Build JSON structure */
    cJSON *root = cJSON_CreateObject();
    cJSON *messages = cJSON_CreateArray();
    cJSON *message = cJSON_CreateObject();
    cJSON *content = cJSON_CreateArray();

    /* Text part: the question about the image */
    cJSON *text_part = cJSON_CreateObject();
    cJSON_AddStringToObject(text_part, "type", "text");
    cJSON_AddStringToObject(text_part, "text", "What do you see in this image? Describe it in detail.");
    cJSON_AddItemToArray(content, text_part);

    /* Image part: the base64-encoded image */
    cJSON *image_part = cJSON_CreateObject();
    cJSON_AddStringToObject(image_part, "type", "image_url");
    cJSON *image_url_obj = cJSON_CreateObject();
    cJSON_AddStringToObject(image_url_obj, "url", data_url);
    cJSON_AddItemToObject(image_part, "image_url", image_url_obj);
    cJSON_AddItemToArray(content, image_part);

    /* Assemble message */
    cJSON_AddStringToObject(message, "role", "user");
    cJSON_AddItemToObject(message, "content", content);
    cJSON_AddItemToArray(messages, message);

    /* Assemble root */
    cJSON_AddStringToObject(root, "model", "meta-llama/llama-4-scout-17b-16e-instruct");
    cJSON_AddItemToObject(root, "messages", messages);
    cJSON_AddNumberToObject(root, "max_tokens", 500);

    char *json_payload = cJSON_PrintUnformatted(root);
    free(data_url);

    /* Step 6: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 7: Initialize response buffer */
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

    printf("Analyzing image with AI vision...\n\n");
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
                cJSON *msg_content = cJSON_GetObjectItem(msg, "content");
                if (msg_content && cJSON_IsString(msg_content)) {
                    printf("AI Vision Analysis:\n");
                    printf("==================\n");
                    printf("%s\n", msg_content->valuestring);
                }
            }

            /* Display token usage */
            cJSON *usage = cJSON_GetObjectItem(response_json, "usage");
            if (usage) {
                printf("\nToken Usage:\n");
                printf("  Prompt: %d\n", cJSON_GetObjectItem(usage, "prompt_tokens")->valueint);
                printf("  Response: %d\n", cJSON_GetObjectItem(usage, "completion_tokens")->valueint);
            }

            cJSON_Delete(response_json);
        }
    }

    /* Step 10: Clean up */
    free(response.data);
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

/*
 * VISION API NOTES:
 *
 * 1. IMAGE FORMATS:
 *    - JPEG, PNG, GIF, WebP are supported
 *    - Use appropriate MIME type in data URL
 *
 * 2. IMAGE SIZE:
 *    - Large images are automatically resized
 *    - More pixels = more tokens = higher cost
 *    - Consider resizing before sending
 *
 * 3. DATA URL FORMAT:
 *    data:<mime_type>;base64,<base64_data>
 *    Example: data:image/jpeg;base64,/9j/4AAQ...
 *
 * 4. ALTERNATIVE: URL
 *    Instead of base64, you can provide a URL:
 *    {"type": "image_url", "image_url": {"url": "https://..."}}
 *
 * BASE64 ENCODING:
 *    - Converts binary to ASCII text
 *    - Uses 64 characters: A-Z, a-z, 0-9, +, /
 *    - Padding with = at the end
 *    - Increases size by ~33%
 */
