/**
 * ============================================================================
 * Example 15: Text-to-Speech - Voice Synthesis with OpenAI TTS
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Converting text to spoken audio
 *   - Using OpenAI's TTS API
 *   - Working with audio output in C++
 *
 * WHAT YOU'LL LEARN:
 *   - Text-to-speech API usage
 *   - Available voices and their characteristics
 *   - Saving audio files with modern C++
 *
 * COMPILE:
 *   g++ -std=c++17 -o 15_text_to_speech 15_text_to_speech.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./15_text_to_speech
 *
 * OUTPUT: Creates speech_output.mp3 in current directory
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

/* Write callback that saves to file */
static size_t write_to_file(void *contents, size_t size, size_t nmemb, void *userp) {
    std::ofstream *file = static_cast<std::ofstream*>(userp);
    file->write(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("DEMETERICS_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: DEMETERICS_API_KEY not set\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 3: Define text to speak and voice to use */
    std::string text_to_speak =
        "Hello! I am an AI assistant speaking to you. "
        "This demonstrates text-to-speech synthesis using modern AI technology. "
        "Pretty cool, right?";
    std::string voice = "nova";  /* One of 11 available voices */

    std::cout << "=== TEXT-TO-SPEECH ===\n";
    std::cout << "Using OpenAI gpt-4o-mini-tts\n\n";
    std::cout << "Text: " << text_to_speak << "\n\n";
    std::cout << "Voice: " << voice << "\n\n";

    /* Step 4: Build JSON request for TTS */
    json request_body = {
        {"model", "gpt-4o-mini-tts"},
        {"input", text_to_speak},
        {"voice", voice},
        {"response_format", "mp3"}
    };

    std::string json_payload = request_body.dump();

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 6: Open output file */
    std::string output_file = "speech_output.mp3";
    std::ofstream file(output_file, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not create output file\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 7: Configure curl */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/openai/v1/audio/speech");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

    std::cout << "Generating speech...\n";
    CURLcode res = curl_easy_perform(curl);
    file.close();

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
        std::remove(output_file.c_str());  /* Delete failed output */
    } else {
        /* Check file size to verify success */
        std::ifstream check(output_file, std::ios::binary | std::ios::ate);
        if (check) {
            std::streamsize size = check.tellg();
            check.close();

            if (size > 0) {
                std::cout << "\nSuccess! Audio saved to: " << output_file
                          << " (" << size << " bytes)\n";
                std::cout << "Play it with: mpv " << output_file << "\n";
                std::cout << "         or: ffplay " << output_file << "\n";
            } else {
                std::cout << "Error: Generated file is empty\n";
            }
        }
    }

    /* Step 8: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    std::cout << "\n=== AVAILABLE VOICES ===\n\n";
    std::cout << "Voice       Description\n";
    std::cout << "------      -----------\n";
    std::cout << "alloy       Neutral, balanced\n";
    std::cout << "ash         Confident, professional\n";
    std::cout << "ballad      Warm, storytelling\n";
    std::cout << "coral       Friendly, upbeat\n";
    std::cout << "echo        Smooth, clear\n";
    std::cout << "fable       Expressive, British-accented\n";
    std::cout << "nova        Youthful, energetic\n";
    std::cout << "onyx        Deep, authoritative\n";
    std::cout << "sage        Wise, measured\n";
    std::cout << "shimmer     Gentle, soothing\n";
    std::cout << "verse       Dramatic, theatrical\n\n";

    std::cout << "=== OUTPUT FORMATS ===\n\n";
    std::cout << "mp3   - Most compatible (default)\n";
    std::cout << "opus  - Best quality/size ratio\n";
    std::cout << "aac   - Apple/iOS compatible\n";
    std::cout << "flac  - Lossless quality\n";
    std::cout << "wav   - Uncompressed\n";
    std::cout << "pcm   - Raw audio data\n\n";

    std::cout << "=== USE CASES ===\n\n";
    std::cout << "1. Accessibility - Screen readers\n";
    std::cout << "2. Voice assistants\n";
    std::cout << "3. Podcasts/audiobooks\n";
    std::cout << "4. Language learning\n";
    std::cout << "5. Notification systems\n";

    return 0;
}

/*
 * C++ FILE I/O NOTES:
 *
 * std::ofstream with std::ios::binary
 *   Opens file for binary writing. Essential for audio!
 *
 * std::ifstream with std::ios::ate
 *   Opens at end so tellg() gives file size.
 *
 * std::streamsize
 *   Type for file positions/sizes.
 *
 * TEXT-TO-SPEECH NOTES:
 *
 * PRICING:
 *   - Input: $0.60 per 1M characters
 *   - Output: $12 per 1M tokens (audio duration-based)
 *
 * VOICE SELECTION:
 *   - Try different voices for different contexts
 *   - Some voices work better for certain languages
 *   - Consider your audience when choosing
 */
