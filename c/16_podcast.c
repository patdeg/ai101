////////////////////////////////////////////////////////////////////////////////
// Example 16: Multi-Speaker Podcast Generation
////////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to generate a podcast with multiple speakers
// using Demeterics multi-speaker TTS API in a SINGLE API call.
//
// What you'll learn:
// - Multi-speaker audio generation with distinct voices
// - NPR/NotebookLM "Deep Dive" podcast style
// - Voice selection from 30 available options
// - Script formatting with "Speaker: text" pattern
//
// Prerequisites:
// - DEMETERICS_API_KEY environment variable
// - libcurl for HTTP requests
// - cJSON for JSON building
//
// Compile:
//   gcc -o 16_podcast 16_podcast.c -lcurl -lcjson
//
// Usage:
//   export DEMETERICS_API_KEY="dmt_your-api-key"
//   ./16_podcast
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

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
    // Step 1: Check for API key
    const char *api_key = getenv("DEMETERICS_API_KEY");
    if (!api_key) {
        fprintf(stderr, "Error: DEMETERICS_API_KEY not set\n");
        fprintf(stderr, "Get your key from: https://demeterics.com\n");
        return 1;
    }

    // Step 2: Define the podcast script
    const char *podcast_script =
        "Alex: Hey, welcome back to The Deep Dive! I am Alex.\n"
        "Sam: And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it.\n"
        "Alex: Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue.\n"
        "Sam: Right! But here is the thing. That rhyme does not tell you much, does it?\n"
        "Alex: Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia.\n"
        "Sam: To India, specifically. Which is why he called the native people Indians.\n"
        "Alex: Exactly! A massive geography fail that stuck around for five hundred years.\n"
        "Sam: So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain.\n"
        "Alex: Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no.\n"
        "Sam: But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it.\n"
        "Alex: And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is.\n"
        "Sam: Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad.\n"
        "Alex: Because it was! If America was not there, he and his crew would have starved in the middle of the ocean.\n"
        "Sam: So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea.\n"
        "Alex: And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas.\n"
        "Sam: An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries.\n"
        "Alex: So this whole discovery narrative is complicated, to say the least.\n"
        "Sam: Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything.\n"
        "Alex: That is the deep dive for today. Thanks for listening, everyone!\n"
        "Sam: See you next time!";

    printf("========================================\n");
    printf("Multi-Speaker Podcast Generation\n");
    printf("========================================\n\n");
    printf("Speakers:\n");
    printf("  Alex: Charon (informative, clear)\n");
    printf("  Sam: Puck (upbeat, enthusiastic)\n\n");
    printf("Script length: %zu characters\n\n", strlen(podcast_script));
    printf("Generating podcast audio...\n");

    // Step 3: Build JSON request (Demeterics format)
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "provider", "gemini");
    cJSON_AddStringToObject(root, "input", podcast_script);

    // Speakers array
    cJSON *speakers = cJSON_CreateArray();

    cJSON *alex = cJSON_CreateObject();
    cJSON_AddStringToObject(alex, "name", "Alex");
    cJSON_AddStringToObject(alex, "voice", "Charon");
    cJSON_AddItemToArray(speakers, alex);

    cJSON *sam = cJSON_CreateObject();
    cJSON_AddStringToObject(sam, "name", "Sam");
    cJSON_AddStringToObject(sam, "voice", "Puck");
    cJSON_AddItemToArray(speakers, sam);

    cJSON_AddItemToObject(root, "speakers", speakers);

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

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/tts/v1/generate");
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

    // Step 8: Save audio file directly (Demeterics returns audio bytes)
    const char *output_file = "columbus_podcast.wav";
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
