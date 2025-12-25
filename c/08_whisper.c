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
 *   - HTTP multipart/form-data in libcurl
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
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Check if audio file exists */
    const char *audio_path = "../gettysburg.mp3";
    FILE *audio_file = fopen(audio_path, "rb");
    if (audio_file == NULL) {
        fprintf(stderr, "Error: Could not open audio file: %s\n", audio_path);
        fprintf(stderr, "Make sure gettysburg.mp3 exists in parent directory\n");
        return 1;
    }

    /* Get file size */
    fseek(audio_file, 0, SEEK_END);
    long file_size = ftell(audio_file);
    fseek(audio_file, 0, SEEK_SET);
    fclose(audio_file);

    printf("=== WHISPER AUDIO TRANSCRIPTION ===\n");
    printf("File: %s\n", audio_path);
    printf("Size: %ld bytes (%.2f MB)\n\n", file_size, file_size / (1024.0 * 1024.0));

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        curl_global_cleanup();
        return 1;
    }

    /* ========================================================================
     * Step 4: Build multipart form data
     * ========================================================================
     * Whisper API uses multipart/form-data for file uploads.
     * This is different from JSON - it's designed for file uploads.
     *
     * libcurl's curl_mime makes this easy!
     */

    curl_mime *form = curl_mime_init(curl);
    curl_mimepart *field;

    /* Add the audio file */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "file");
    curl_mime_filedata(field, audio_path);

    /* Add the model parameter */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "model");
    curl_mime_data(field, "whisper-large-v3-turbo", CURL_ZERO_TERMINATED);

    /* Optional: Add language hint (helps accuracy) */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "language");
    curl_mime_data(field, "en", CURL_ZERO_TERMINATED);

    /* Step 5: Set up headers */
    struct curl_slist *headers = NULL;
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 6: Configure curl */
    struct ResponseBuffer response = {0};
    response.data = malloc(1);
    response.size = 0;
    response.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/audio/transcriptions");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
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
    curl_mime_free(form);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational information */
    printf("\n=== WHISPER FEATURES ===\n\n");
    printf("SUPPORTED FORMATS:\n");
    printf("  mp3, mp4, mpeg, mpga, m4a, wav, webm\n\n");

    printf("PARAMETERS:\n");
    printf("  model    - whisper-large-v3-turbo (fastest)\n");
    printf("  language - ISO 639-1 code (en, es, fr, etc.)\n");
    printf("  prompt   - Optional context/vocabulary hints\n\n");

    printf("COST:\n");
    printf("  $0.04 per hour of audio\n\n");

    printf("TIPS:\n");
    printf("  1. Use language parameter for better accuracy\n");
    printf("  2. Clean audio = better transcription\n");
    printf("  3. Provide vocabulary hints for domain terms\n");
    printf("  4. Max file size: 25 MB\n");

    return 0;
}

/*
 * MULTIPART/FORM-DATA NOTES:
 *
 * Unlike JSON POST requests, file uploads use multipart/form-data.
 * This format allows mixing files and regular form fields.
 *
 * LIBCURL MIME API:
 *   curl_mime_init()      - Create a new form
 *   curl_mime_addpart()   - Add a part (field or file)
 *   curl_mime_name()      - Set field name
 *   curl_mime_filedata()  - Attach a file
 *   curl_mime_data()      - Set text data
 *   curl_mime_free()      - Clean up
 *
 * AUDIO TRANSCRIPTION USES:
 *   - Voice notes to text
 *   - Meeting transcription
 *   - Podcast transcription
 *   - Accessibility features
 *   - Voice commands
 */
