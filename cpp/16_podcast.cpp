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
// - nlohmann/json for JSON handling
//
// Compile:
//   g++ -o 16_podcast 16_podcast.cpp -lcurl
//
// Usage:
//   export DEMETERICS_API_KEY="dmt_your-api-key"
//   ./16_podcast
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

// Curl write callback
static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int main() {
    // Step 1: Check for API key
    const char* api_key = std::getenv("DEMETERICS_API_KEY");
    if (!api_key) {
        std::cerr << "Error: DEMETERICS_API_KEY not set" << std::endl;
        std::cerr << "Get your key from: https://demeterics.com" << std::endl;
        return 1;
    }

    // Step 2: Define the podcast script
    // Format: "SpeakerName: dialogue text"
    std::string podcast_script = R"(Alex: Hey, welcome back to The Deep Dive! I am Alex.
Sam: And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it.
Alex: Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue.
Sam: Right! But here is the thing. That rhyme does not tell you much, does it?
Alex: Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia.
Sam: To India, specifically. Which is why he called the native people Indians.
Alex: Exactly! A massive geography fail that stuck around for five hundred years.
Sam: So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain.
Alex: Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no.
Sam: But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it.
Alex: And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is.
Sam: Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad.
Alex: Because it was! If America was not there, he and his crew would have starved in the middle of the ocean.
Sam: So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea.
Alex: And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas.
Sam: An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries.
Alex: So this whole discovery narrative is complicated, to say the least.
Sam: Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything.
Alex: That is the deep dive for today. Thanks for listening, everyone!
Sam: See you next time!)";

    // Step 3: Configure voices for each speaker
    // Available voices (30 total):
    //   Puck (upbeat), Kore (firm), Charon (informative), Zephyr (bright),
    //   Fenrir (excitable), Leda (youthful), Aoede (breezy), Sulafat (warm),
    //   Achird (friendly), and 21 others.
    struct Speaker {
        std::string name;
        std::string voice;
    };

    std::vector<Speaker> speakers = {
        {"Alex", "Charon"},  // Informative, clear - explains concepts
        {"Sam", "Puck"}      // Upbeat - energetic, enthusiastic
    };

    std::cout << "========================================" << std::endl;
    std::cout << "Multi-Speaker Podcast Generation" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Speakers:" << std::endl;
    for (const auto& s : speakers) {
        std::cout << "  " << s.name << ": " << s.voice << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Script length: " << podcast_script.length() << " characters" << std::endl;
    std::cout << std::endl;
    std::cout << "Generating podcast audio..." << std::endl;

    // Step 4: Build JSON request (Demeterics format)
    json speaker_array = json::array();
    for (const auto& s : speakers) {
        speaker_array.push_back({
            {"name", s.name},
            {"voice", s.voice}
        });
    }

    json request_body = {
        {"provider", "gemini"},
        {"input", podcast_script},
        {"speakers", speaker_array}
    };

    std::string json_str = request_body.dump();

    // Step 5: Build authorization header
    std::string auth_header = "Authorization: Bearer " + std::string(api_key);

    // Step 6: Initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Error: Failed to initialize curl" << std::endl;
        return 1;
    }

    std::string response_data;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/tts/v1/generate");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Step 7: Make request
    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res != CURLE_OK) {
        std::cerr << "Error: curl request failed: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    // Step 8: Check HTTP status
    if (http_code != 200) {
        std::cerr << "API Error: HTTP " << http_code << std::endl;
        std::cerr << response_data << std::endl;
        return 1;
    }

    // Step 9: Save audio file directly (Demeterics returns audio bytes)
    std::string output_file = "columbus_podcast.wav";
    std::ofstream outfile(output_file, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: Cannot open output file" << std::endl;
        return 1;
    }

    outfile.write(response_data.data(), response_data.size());
    outfile.close();

    std::cout << std::endl;
    std::cout << "Success!" << std::endl;
    std::cout << std::endl;
    std::cout << "Output:" << std::endl;
    std::cout << "  File: " << output_file << std::endl;
    std::cout << "  Size: " << response_data.size() << " bytes (" << response_data.size() / 1024 << " KB)" << std::endl;
    std::cout << std::endl;
    std::cout << "To play:" << std::endl;
    std::cout << "  mpv " << output_file << std::endl;

    return 0;
}
