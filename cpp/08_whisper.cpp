/**
 * ============================================================================
 * Example 8: Whisper - Audio Transcription
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Transcribing audio files using Whisper
 *   - Multipart form data uploads in C++
 *   - Working with audio files
 *
 * WHAT YOU'LL LEARN:
 *   - Whisper API for speech-to-text
 *   - HTTP multipart/form-data with libcurl
 *   - Audio format considerations
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
#include <cstdlib>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

/* Get file size */
long get_file_size(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) return -1;
    return file.tellg();
}

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("DEMETERICS_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: DEMETERICS_API_KEY not set\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Check if audio file exists */
    std::string audio_path = "../gettysburg.mp3";
    long file_size = get_file_size(audio_path);

    if (file_size < 0) {
        std::cerr << "Error: Could not open audio file: " << audio_path << "\n";
        std::cerr << "Make sure gettysburg.mp3 exists in parent directory\n";
        return 1;
    }

    std::cout << "=== WHISPER AUDIO TRANSCRIPTION ===\n";
    std::cout << "File: " << audio_path << "\n";
    std::cout << "Size: " << file_size << " bytes ("
              << (file_size / (1024.0 * 1024.0)) << " MB)\n\n";

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
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
    curl_mime_filedata(field, audio_path.c_str());

    /* Add the model parameter */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "model");
    curl_mime_data(field, "whisper-large-v3-turbo", CURL_ZERO_TERMINATED);

    /* Optional: Add language hint (helps accuracy) */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "language");
    curl_mime_data(field, "en", CURL_ZERO_TERMINATED);

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 6: Configure curl */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/audio/transcriptions");
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
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
    curl_mime_free(form);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational information */
    std::cout << "\n=== WHISPER FEATURES ===\n\n";
    std::cout << "SUPPORTED FORMATS:\n";
    std::cout << "  mp3, mp4, mpeg, mpga, m4a, wav, webm\n\n";

    std::cout << "PARAMETERS:\n";
    std::cout << "  model    - whisper-large-v3-turbo (fastest)\n";
    std::cout << "  language - ISO 639-1 code (en, es, fr, etc.)\n";
    std::cout << "  prompt   - Optional context/vocabulary hints\n\n";

    std::cout << "COST:\n";
    std::cout << "  $0.04 per hour of audio\n\n";

    std::cout << "TIPS:\n";
    std::cout << "  1. Use language parameter for better accuracy\n";
    std::cout << "  2. Clean audio = better transcription\n";
    std::cout << "  3. Provide vocabulary hints for domain terms\n";
    std::cout << "  4. Max file size: 25 MB\n";

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
