/**
 * ============================================================================
 * Example 8: Whisper - Audio Transcription
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Transcribing audio files using Whisper
 *   - Base64-encoding a binary file into a JSON request in C++
 *   - Working with audio files
 *
 * WHAT YOU'LL LEARN:
 *   - Whisper API for speech-to-text
 *   - OpenRouter's /audio/transcriptions endpoint (JSON body, not multipart)
 *   - Audio format considerations
 *
 * PREREQUISITES:
 *   - OPENROUTER_API_KEY environment variable set (free at https://openrouter.ai)
 *
 * COMPILE:
 *   g++ -std=c++17 -o 08_whisper 08_whisper.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./08_whisper
 *
 * NOTE: Requires gettysburg.mp3 in the parent directory
 *
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

/* Base64 encoding function (same pattern as the vision examples) */
std::string base64_encode(const std::vector<unsigned char> &data) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string result;
    result.reserve(4 * ((data.size() + 2) / 3));

    size_t i = 0;
    while (i < data.size()) {
        uint32_t octet_a = i < data.size() ? data[i++] : 0;
        uint32_t octet_b = i < data.size() ? data[i++] : 0;
        uint32_t octet_c = i < data.size() ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        result += table[(triple >> 18) & 0x3F];
        result += table[(triple >> 12) & 0x3F];
        result += table[(triple >> 6) & 0x3F];
        result += table[triple & 0x3F];
    }

    size_t mod = data.size() % 3;
    if (mod > 0) {
        result[result.size() - 1] = '=';
        if (mod == 1) result[result.size() - 2] = '=';
    }

    return result;
}

/* Read entire file into a vector */
std::vector<unsigned char> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("OPENROUTER_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: OPENROUTER_API_KEY not set\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Read and base64-encode the audio file */
    std::string audio_path = "../gettysburg.mp3";

    std::vector<unsigned char> audio_data;
    try {
        audio_data = read_file(audio_path);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Make sure gettysburg.mp3 exists in parent directory\n";
        return 1;
    }

    std::cout << "=== WHISPER AUDIO TRANSCRIPTION ===\n";
    std::cout << "File: " << audio_path << "\n";
    std::cout << "Size: " << audio_data.size() << " bytes ("
              << (audio_data.size() / (1024.0 * 1024.0)) << " MB)\n\n";

    std::string audio_base64 = base64_encode(audio_data);

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* ========================================================================
     * Step 4: Build the JSON request body
     * ========================================================================
     * OpenRouter's transcription endpoint takes base64-encoded audio inside a
     * JSON body (the same "encode binary as base64, embed in JSON" pattern
     * used for images in example 04) rather than a multipart file upload.
     */
    json request_body = {
        {"model", "openai/whisper-1"},
        {"input_audio", {
            {"data", audio_base64},
            {"format", "mp3"}
        }}
    };

    std::string json_payload = request_body.dump();

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 6: Configure curl */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/audio/transcriptions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    /* Step 7: Perform the request */
    std::cout << "Transcribing audio (this may take a moment)...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 8: Parse and display result */
        try {
            json response = json::parse(response_data);

            if (response.contains("error")) {
                std::cerr << "API Error: " << response["error"]["message"] << "\n";
            } else if (response.contains("text")) {
                std::cout << "=== TRANSCRIPTION ===\n\n";
                std::cout << response["text"].get<std::string>() << "\n";
            }
        } catch (const json::exception &e) {
            std::cerr << "Parse error: " << e.what() << "\n";
            std::cout << "Raw response: " << response_data << "\n";
        }
    }

    /* Step 9: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational information */
    std::cout << "\n=== WHISPER FEATURES ===\n\n";
    std::cout << "SUPPORTED FORMATS:\n";
    std::cout << "  mp3, mp4, mpeg, mpga, m4a, wav, webm\n\n";

    std::cout << "PARAMETERS:\n";
    std::cout << "  model - openai/whisper-1\n";
    std::cout << "  input_audio.format - matches the file you send (mp3, wav, ...)\n\n";

    std::cout << "COST:\n";
    std::cout << "  $0.006 per minute of audio (not free - the only STT model\n";
    std::cout << "  on OpenRouter, but cheap: a 3-minute clip costs about $0.02)\n\n";

    std::cout << "TIPS:\n";
    std::cout << "  1. Clean audio = better transcription\n";
    std::cout << "  2. Max file size: 25 MB\n";
    std::cout << "  3. Base64 inflates size ~33% - stay well under that limit\n";

    return 0;
}

/*
 * BASE64 JSON AUDIO UPLOAD NOTES:
 *
 * Unlike a traditional multipart/form-data file upload, OpenRouter's audio
 * endpoints take a plain JSON body with the file's bytes base64-encoded
 * inline - the same approach the vision examples use for images. One HTTP
 * client code path (build JSON, POST it, parse JSON back) covers chat,
 * vision, and audio.
 *
 * C++ FILE SIZE CHECK:
 *   std::ios::ate - Open at end (for tellg())
 *   tellg() - Get current position (= file size at end)
 *
 * AUDIO TRANSCRIPTION USES:
 *   - Voice notes to text
 *   - Meeting transcription
 *   - Podcast transcription
 *   - Accessibility features
 *   - Voice commands
 */
