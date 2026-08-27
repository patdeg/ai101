////////////////////////////////////////////////////////////////////////////////
// Example 16: Multi-Speaker Podcast Generation
////////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to generate a podcast with two speakers using
// OpenRouter's Fish Audio multi-speaker TTS model in a SINGLE API call.
//
// What you'll learn:
// - Multi-speaker audio generation with distinct voices
// - NPR/NotebookLM "Deep Dive" podcast style
// - Speaker turns via <|speaker:N|> tags + a referenceId voice array
// - Script formatting with "Speaker: text" pattern
//
// Prerequisites:
// - OPENROUTER_API_KEY environment variable (free at https://openrouter.ai)
// - Two Fish Audio voice reference IDs (free account, browse voice library
//   at https://fish.audio, copy each voice's ID) set as FISH_VOICE_1 /
//   FISH_VOICE_2 environment variables
// - libcurl for HTTP requests
// - nlohmann/json for JSON handling
//
// Compile:
//   g++ -o 16_podcast 16_podcast.cpp -lcurl
//
// Usage:
//   export OPENROUTER_API_KEY="sk-or-your-api-key"
//   export FISH_VOICE_1="<voice-id-1>"
//   export FISH_VOICE_2="<voice-id-2>"
//   ./16_podcast
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
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
    // Step 1: Check for API key and voice reference IDs
    const char* api_key = std::getenv("OPENROUTER_API_KEY");
    if (!api_key) {
        std::cerr << "Error: OPENROUTER_API_KEY not set" << std::endl;
        std::cerr << "Get your key from: https://openrouter.ai" << std::endl;
        return 1;
    }

    const char* fish_voice_1 = std::getenv("FISH_VOICE_1");
    const char* fish_voice_2 = std::getenv("FISH_VOICE_2");
    if (!fish_voice_1 || !fish_voice_2) {
        std::cerr << "Error: FISH_VOICE_1 and FISH_VOICE_2 not set" << std::endl;
        std::cerr << "Grab two voice reference IDs from https://fish.audio "
                     "(free account, voice library)" << std::endl;
        return 1;
    }

    // Step 2: Define the podcast script (for display; the actual TTS request
    // uses <|speaker:0|>/<|speaker:1|> tags built in step 4 below)
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

    // Step 3: Two speakers, mapped to Fish Audio voice reference IDs from env
    std::cout << "========================================" << std::endl;
    std::cout << "Multi-Speaker Podcast Generation" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Speakers:" << std::endl;
    std::cout << "  Alex (speaker 0): voice " << fish_voice_1 << std::endl;
    std::cout << "  Sam  (speaker 1): voice " << fish_voice_2 << std::endl;
    std::cout << std::endl;
    std::cout << "Script length: " << podcast_script.length() << " characters" << std::endl;
    std::cout << std::endl;
    std::cout << "Generating podcast audio..." << std::endl;

    // Step 4: Build JSON request for OpenRouter's /audio/speech endpoint.
    // Fish Audio's multi-speaker format tags each turn with <|speaker:N|>
    // inline in the text, and referenceId[N] picks that speaker's voice.
    std::string tagged_script;
    {
        std::istringstream lines(podcast_script);
        std::string line;
        while (std::getline(lines, line)) {
            size_t colon = line.find(": ");
            if (colon == std::string::npos) continue;
            std::string speaker_name = line.substr(0, colon);
            std::string dialogue = line.substr(colon + 2);
            int idx = (speaker_name == "Alex") ? 0 : 1;
            tagged_script += "<|speaker:" + std::to_string(idx) + "|>" + dialogue + " ";
        }
    }

    json request_body = {
        {"model", "fish-audio/s2.1-pro-free:free"},
        {"input", tagged_script},
        {"providerOptions", {
            {"fishAudio", {
                {"referenceId", json::array({fish_voice_1, fish_voice_2})}
            }}
        }},
        {"response_format", "mp3"}
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

    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/audio/speech");
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

    // Step 9: Save audio file directly (OpenRouter's /audio/speech returns
    // raw audio bytes, same as example 15)
    std::string output_file = "columbus_podcast.mp3";
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
