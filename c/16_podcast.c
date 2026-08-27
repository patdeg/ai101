////////////////////////////////////////////////////////////////////////////////
// Example 16: Multi-Speaker Podcast Generation
////////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to generate a two-speaker podcast using
// OpenRouter's /audio/speech endpoint (Fish Audio S2.1 Pro, free tier) in a
// SINGLE API call.
//
// What you'll learn:
// - Multi-speaker audio generation with distinct voices
// - NPR/NotebookLM "Deep Dive" podcast style
// - Fish Audio's <|speaker:N|> inline tagging + providerOptions.fishAudio
// - Script formatting with "Speaker: text" pattern
//
// Prerequisites:
// - OPENROUTER_API_KEY environment variable
// - FISH_VOICE_1 / FISH_VOICE_2 environment variables: two Fish Audio voice
//   reference IDs. Get a free account at https://fish.audio, browse the
//   voice library, and copy the ID of any voice you like.
// - libcurl for HTTP requests
// - cJSON for JSON building
//
// Compile:
//   gcc -o 16_podcast 16_podcast.c -lcurl -lcjson
//
// Usage:
//   export OPENROUTER_API_KEY="sk-or-your-api-key"
//   export FISH_VOICE_1="<voice-id-for-Alex>"
//   export FISH_VOICE_2="<voice-id-for-Sam>"
//   ./16_podcast
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

// Response buffer structure
struct ResponseBuffer {
    char *data;
    size_t size;
};

// Callback for curl response
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct ResponseBuffer *buf = (struct ResponseBuffer *)userp;

    char *ptr = realloc(buf->data, buf->size + realsize + 1);
    if (!ptr) {
        fprintf(stderr, "Error: Out of memory\n");
        return 0;
    }

    buf->data = ptr;
    memcpy(&(buf->data[buf->size]), contents, realsize);
    buf->size += realsize;
    buf->data[buf->size] = '\0';

    return realsize;
}

int main(void) {
    // Step 1: Check for API keys
    const char *api_key = getenv("OPENROUTER_API_KEY");
    if (!api_key) {
        fprintf(stderr, "Error: OPENROUTER_API_KEY not set\n");
        fprintf(stderr, "Get your key from: https://openrouter.ai\n");
        return 1;
    }

    const char *voice1 = getenv("FISH_VOICE_1");
    const char *voice2 = getenv("FISH_VOICE_2");
    if (!voice1 || !voice2) {
        fprintf(stderr, "Error: FISH_VOICE_1 and FISH_VOICE_2 not set\n");
        fprintf(stderr, "Get free voice IDs from: https://fish.audio\n");
        return 1;
    }

    // Step 2: Define the podcast script. Fish Audio's multi-speaker format
    // marks each turn with <|speaker:0|> / <|speaker:1|> inline tags instead
    // of "Name: text" -- the index picks a voice from the referenceId array
    // we build below (0 -> Alex/FISH_VOICE_1, 1 -> Sam/FISH_VOICE_2).
    const char *podcast_script =
        "<|speaker:0|>Hey, welcome back to The Deep Dive! I am Alex. "
        "<|speaker:1|>And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it. "
        "<|speaker:0|>Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue. "
        "<|speaker:1|>Right! But here is the thing. That rhyme does not tell you much, does it? "
        "<|speaker:0|>Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia. "
        "<|speaker:1|>To India, specifically. Which is why he called the native people Indians. "
        "<|speaker:0|>Exactly! A massive geography fail that stuck around for five hundred years. "
        "<|speaker:1|>So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain. "
        "<|speaker:0|>Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no. "
        "<|speaker:1|>But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it. "
        "<|speaker:0|>And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is. "
        "<|speaker:1|>Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad. "
        "<|speaker:0|>Because it was! If America was not there, he and his crew would have starved in the middle of the ocean. "
        "<|speaker:1|>So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea. "
        "<|speaker:0|>And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas. "
        "<|speaker:1|>An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries. "
        "<|speaker:0|>So this whole discovery narrative is complicated, to say the least. "
        "<|speaker:1|>Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything. "
        "<|speaker:0|>That is the deep dive for today. Thanks for listening, everyone! "
        "<|speaker:1|>See you next time!";

    printf("========================================\n");
    printf("Multi-Speaker Podcast Generation\n");
    printf("========================================\n\n");
    printf("Speakers:\n");
    printf("  speaker:0 (Alex) -> voice %s\n", voice1);
    printf("  speaker:1 (Sam)  -> voice %s\n\n", voice2);
    printf("Script length: %zu characters\n\n", strlen(podcast_script));
    printf("Generating podcast audio...\n");

    // Step 3: Build JSON request (OpenRouter /audio/speech, Fish Audio format)
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "model", "fish-audio/s2.1-pro-free:free");
    cJSON_AddStringToObject(root, "input", podcast_script);
    cJSON_AddStringToObject(root, "response_format", "mp3");

    // providerOptions.fishAudio.referenceId: array of voice IDs, indexed by
    // the <|speaker:N|> tags in the input text.
    cJSON *reference_ids = cJSON_CreateArray();
    cJSON_AddItemToArray(reference_ids, cJSON_CreateString(voice1));
    cJSON_AddItemToArray(reference_ids, cJSON_CreateString(voice2));

    cJSON *fish_audio_opts = cJSON_CreateObject();
    cJSON_AddItemToObject(fish_audio_opts, "referenceId", reference_ids);

    cJSON *provider_options = cJSON_CreateObject();
    cJSON_AddItemToObject(provider_options, "fishAudio", fish_audio_opts);

    cJSON_AddItemToObject(root, "providerOptions", provider_options);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    // Step 4: Build authorization header
    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);

    // Step 5: Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize curl\n");
        free(json_str);
        return 1;
    }

    struct ResponseBuffer response = {NULL, 0};
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);

    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/audio/speech");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Step 6: Make request
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    free(json_str);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: curl request failed: %s\n", curl_easy_strerror(res));
        free(response.data);
        return 1;
    }

    // Step 7: Check HTTP status
    if (http_code != 200) {
        fprintf(stderr, "API Error: HTTP %ld\n", http_code);
        if (response.data) {
            fprintf(stderr, "%s\n", response.data);
            free(response.data);
        }
        return 1;
    }

    // Step 8: Save audio file directly (OpenRouter returns raw audio bytes)
    const char *output_file = "columbus_podcast.mp3";
    FILE *fp = fopen(output_file, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open output file\n");
        free(response.data);
        return 1;
    }

    fwrite(response.data, 1, response.size, fp);
    fclose(fp);

    printf("\nSuccess!\n\n");
    printf("Output:\n");
    printf("  File: %s\n", output_file);
    printf("  Size: %zu bytes (%zu KB)\n\n", response.size, response.size / 1024);
    printf("To play:\n");
    printf("  mpv %s\n", output_file);

    free(response.data);
    return 0;
}
