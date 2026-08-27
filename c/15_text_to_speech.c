/**
 * ============================================================================
 * Example 15: Text-to-Speech - Voice Synthesis with Fish Audio (OpenRouter)
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Converting text to spoken audio
 *   - Using OpenRouter's /audio/speech endpoint (Fish Audio S2.1 Pro, free)
 *   - Working with audio output in C
 *
 * WHAT YOU'LL LEARN:
 *   - Text-to-speech API usage
 *   - Available voices and their characteristics
 *   - Saving audio files
 *
 * COMPILE:
 *   gcc -o 15_text_to_speech 15_text_to_speech.c lib/cJSON.c -lcurl -I lib/
 *
 * RUN:
 *   ./15_text_to_speech
 *
 * OUTPUT: Creates speech_output.mp3 in current directory
 *
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

/* Write callback that saves to file */
static size_t write_to_file(void *contents, size_t size, size_t nmemb, void *userp) {
    FILE *file = (FILE *)userp;
    return fwrite(contents, size, nmemb, file);
}

int main(void) {
    /* Step 1: Get API key */
    const char *api_key = getenv("OPENROUTER_API_KEY");
    if (api_key == NULL) {
        fprintf(stderr, "Error: OPENROUTER_API_KEY not set\n");
        return 1;
    }

    /* Step 2: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == NULL) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 3: Define text to speak */
    const char *text_to_speak = "Hello! I am an AI assistant speaking to you. "
                                 "This demonstrates text-to-speech synthesis using modern AI technology. "
                                 "Pretty cool, right?";

    printf("=== TEXT-TO-SPEECH ===\n");
    printf("Using fish-audio/s2.1-pro-free:free (via OpenRouter)\n\n");
    printf("Text: %s\n\n", text_to_speak);

    /* Step 4: Build JSON request for TTS.
     * Fish Audio picks a default voice when no referenceId is supplied --
     * for a specific voice, grab a reference ID from https://fish.audio and
     * see Example 16 for how to pass it via providerOptions.fishAudio. */
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", "fish-audio/s2.1-pro-free:free");
    cJSON_AddStringToObject(root, "input", text_to_speak);
    cJSON_AddStringToObject(root, "response_format", "mp3");

    char *json_payload = cJSON_PrintUnformatted(root);

    /* Step 5: Set up headers */
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth_header);

    /* Step 6: Open output file */
    const char *output_file = "speech_output.mp3";
    FILE *fp = fopen(output_file, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not create output file\n");
        free(json_payload);
        cJSON_Delete(root);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 7: Configure curl */
    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/audio/speech");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    printf("Generating speech...\n");
    CURLcode res = curl_easy_perform(curl);
    fclose(fp);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: %s\n", curl_easy_strerror(res));
        remove(output_file);  /* Delete failed output */
    } else {
        /* Check file size to verify success */
        FILE *check = fopen(output_file, "rb");
        if (check) {
            fseek(check, 0, SEEK_END);
            long size = ftell(check);
            fclose(check);

            if (size > 0) {
                printf("\nSuccess! Audio saved to: %s (%ld bytes)\n", output_file, size);
                printf("Play it with: mpv %s\n", output_file);
                printf("         or: ffplay %s\n", output_file);
            } else {
                printf("Error: Generated file is empty\n");
            }
        }
    }

    /* Step 8: Clean up */
    free(json_payload);
    cJSON_Delete(root);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    printf("\n=== CHOOSING A VOICE ===\n\n");
    printf("No referenceId  -> Fish Audio picks a default voice\n");
    printf("With referenceId -> pick any voice from https://fish.audio\n");
    printf("                    (free account, browse the voice library,\n");
    printf("                     copy its ID -- see Example 16 for the\n");
    printf("                     providerOptions.fishAudio.referenceId shape)\n\n");

    printf("=== OUTPUT FORMATS ===\n\n");
    printf("mp3   - Most compatible (default)\n");
    printf("opus  - Best quality/size ratio\n");
    printf("aac   - Apple/iOS compatible\n");
    printf("flac  - Lossless quality\n");
    printf("wav   - Uncompressed\n");
    printf("pcm   - Raw audio data\n\n");

    printf("=== USE CASES ===\n\n");
    printf("1. Accessibility - Screen readers\n");
    printf("2. Voice assistants\n");
    printf("3. Podcasts/audiobooks\n");
    printf("4. Language learning\n");
    printf("5. Notification systems\n");

    return 0;
}

/*
 * TEXT-TO-SPEECH NOTES:
 *
 * PRICING:
 *   - fish-audio/s2.1-pro-free:free costs nothing (rate-limited, best-effort
 *     availability -- meant for testing/prototyping, not production)
 *
 * VOICE SELECTION:
 *   - Try different voices for different contexts
 *   - Some voices work better for certain languages
 *   - Consider your audience when choosing
 *
 * AUDIO FORMATS:
 *   - mp3: Universal compatibility
 *   - opus: Best for streaming/web
 *   - flac: Best for archival quality
 *
 * C FILE WRITING:
 *   - Open with "wb" (write binary) mode
 *   - Use fwrite() for binary data
 *   - Always check return values
 *   - Close file when done
 */
