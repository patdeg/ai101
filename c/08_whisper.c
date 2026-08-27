/**
 * ============================================================================
 * Example 8: Whisper - Audio Transcription
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Transcribing audio files using Whisper
 *   - Multipart form data uploads in C
 *   - Working with audio files
 *
 * WHAT YOU'LL LEARN:
 *   - Whisper API for speech-to-text
 *   - Base64-encoding a binary file into a JSON request
 *   - Audio format considerations
 *
 * COMPILE:
 *   gcc -o 08_whisper 08_whisper.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./08_whisper
 *
 * NOTE: Requires gettysburg.mp3 in the parent directory
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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

/* Base64 encoding (same approach as 04_vision.c) */
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

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("OPENROUTER_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: OPENROUTER_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Read the whole audio file into memory */
    const char *audio_path = "../gettysburg.mp3";
    FILE *audio_file = fopen(audio_path, "rb");
    if (audio_file == NULL) {
        fprintf(stderr, "Error: Could not open audio file: %s\n", audio_path);
        fprintf(stderr, "Make sure gettysburg.mp3 exists in parent directory\n");
        return 1;
    }

    fseek(audio_file, 0, SEEK_END);
    long file_size = ftell(audio_file);
    fseek(audio_file, 0, SEEK_SET);

    unsigned char *audio_data = malloc((size_t)file_size);
    if (audio_data == NULL || fread(audio_data, 1, (size_t)file_size, audio_file) != (size_t)file_size) {
        fprintf(stderr, "Error: Could not read audio file\n");
        fclose(audio_file);
        free(audio_data);
        return 1;
    }
    fclose(audio_file);

    printf("=== WHISPER AUDIO TRANSCRIPTION ===\n");
    printf("File: %s\n", audio_path);
    printf("Size: %ld bytes (%.2f MB)\n\n", file_size, file_size / (1024.0 * 1024.0));

    /* Step 3: Base64-encode the audio (OpenRouter's transcription endpoint
     * takes a JSON body with the audio embedded as base64, not a multipart
     * file upload). */
    size_t encoded_length;
    char *audio_base64 = base64_encode(audio_data, (size_t)file_size, &encoded_length);
    free(audio_data);
    if (audio_base64 == NULL) {
        fprintf(stderr, "Error: Failed to encode audio\n");
        return 1;
    }

    /* Step 4: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        free(audio_base64);
        curl_global_cleanup();
        return 1;
    }

    /* Step 5: Build the JSON request body */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", "openai/whisper-1");
    cJSON *input_audio = cJSON_CreateObject();
    cJSON_AddStringToObject(input_audio, "data", audio_base64);
    cJSON_AddStringToObject(input_audio, "format", "mp3");
    cJSON_AddItemToObject(root, "input_audio", input_audio);
    free(audio_base64);

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 6: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 7: Configure curl */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/audio/transcriptions");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    /* Step 7: Perform the request */
    printf("Transcribing audio (this may take a moment)...\n\n");
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
    } else {
        /* Step 8: Parse and display result */
        cJSON *response_json = cJSON_Parse(response.data);

        if (response_json) {
            /* Check for error */
            cJSON *error = cJSON_GetObjectItem(response_json, "error");
            if (error) {
                cJSON *message = cJSON_GetObjectItem(error, "message");
                if (message) {
                    fprintf(stderr, "API Error: %s\n", message->valuestring);
                }
            } else {
                /* Success - get transcription */
                cJSON *text = cJSON_GetObjectItem(response_json, "text");
                if (text && cJSON_IsString(text)) {
                    printf("=== TRANSCRIPTION ===\n\n");
                    printf("%s\n", text->valuestring);
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

    /* Educational information */
    printf("\n=== WHISPER FEATURES ===\n\n");
    printf("SUPPORTED FORMATS:\n");
    printf("  mp3, mp4, mpeg, mpga, m4a, wav, webm\n\n");

    printf("PARAMETERS:\n");
    printf("  model      - openai/whisper-1\n");
    printf("  input_audio.format - matches the audio's encoding (mp3, wav, ...)\n\n");

    printf("COST:\n");
    printf("  ~$0.006 per minute of audio (openai/whisper-1 on OpenRouter)\n\n");

    printf("TIPS:\n");
    printf("  1. Clean audio = better transcription\n");
    printf("  2. Base64 grows the payload ~33%% vs. the raw file\n");
    printf("  3. Max file size: 25 MB\n");

    return 0;
}

/*
 * BASE64 JSON UPLOAD NOTES:
 *
 * OpenRouter's /audio/transcriptions endpoint takes a plain JSON body with
 * the audio embedded as a base64 string, rather than a multipart/form-data
 * file upload — the same base64-in-JSON pattern already used for images in
 * 04_vision.c and 06_image_safety_check.c.
 *
 * AUDIO TRANSCRIPTION USES:
 *   - Voice notes to text
 *   - Meeting transcription
 *   - Podcast transcription
 *   - Accessibility features
 *   - Voice commands
 */
