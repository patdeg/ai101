/**
 * ============================================================================
 * Example 2: System Prompt - Controlling AI Behavior
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using system prompts to set AI personality and behavior
 *   - The three message roles: system, user, assistant
 *   - How system messages influence AI responses
 *
 * WHAT YOU'LL LEARN:
 *   - System prompt patterns for different use cases
 *   - Message ordering and its importance
 *   - Creating specialized AI assistants
 *
 * COMPILE:
 *   g++ -std=c++17 -o 02_system_prompt 02_system_prompt.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./02_system_prompt
 *
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

/* Callback for curl to write response data */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
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

    /* ========================================================================
     * Step 3: Build the JSON request with a SYSTEM PROMPT
     * ========================================================================
     * The system prompt sets the AI's personality and rules.
     * It's like giving instructions to an employee before they start work.
     *
     * MESSAGE ROLES:
     *   - "system": Instructions for the AI (invisible to the "user")
     *   - "user": Messages from the human (you)
     *   - "assistant": Messages from the AI (in conversation history)
     */

    /* With nlohmann/json, building complex JSON is beautiful! */
    json request_body = {
        {"model", "meta-llama/llama-4-scout-17b-16e-instruct"},
        {"messages", {
            /* SYSTEM MESSAGE: Sets AI behavior - this AI will act as a pirate! */
            {
                {"role", "system"},
                {"content", "You are a friendly pirate who speaks in pirate dialect. "
                           "You always include nautical references and say 'Arrr!' occasionally. "
                           "Keep responses brief but entertaining."}
            },
            /* USER MESSAGE: The actual question */
            {
                {"role", "user"},
                {"content", "What is the capital of France?"}
            }
        }},
        {"temperature", 0.8},  /* Higher = more creative */
        {"max_tokens", 150}
    };

    std::string json_payload = request_body.dump();

    /* Step 4: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 5: Configure and perform request */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    std::cout << "Asking a pirate about France...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 6: Parse and display response */
    try {
        json response = json::parse(response_data);

        if (response.contains("choices") && !response["choices"].empty()) {
            std::string answer = response["choices"][0]["message"]["content"];
            std::cout << "Pirate AI says:\n" << answer << "\n";
        }
    } catch (const json::exception &e) {
        std::cerr << "Error parsing response: " << e.what() << "\n";
    }

    /* Step 7: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Print examples of system prompts */
    std::cout << "\n";
    std::cout << "=== SYSTEM PROMPT EXAMPLES ===\n\n";

    std::cout << "1. EXPERT PERSONA:\n";
    std::cout << R"(   "You are a senior software engineer with 20 years experience.
    Give detailed technical explanations with code examples.")" << "\n\n";

    std::cout << "2. TEACHER:\n";
    std::cout << R"(   "You are a patient teacher for 10-year-old students.
    Use simple words and fun analogies.")" << "\n\n";

    std::cout << "3. JSON ONLY:\n";
    std::cout << R"(   "Respond only with valid JSON. No other text.
    Use the format: {\"answer\": \"...\"}")" << "\n\n";

    std::cout << "4. SAFETY:\n";
    std::cout << R"(   "You are a helpful assistant. Never provide harmful information.
    If asked about dangerous topics, politely decline.")" << "\n";

    return 0;
}

/*
 * SYSTEM PROMPT BEST PRACTICES:
 *
 * 1. Be specific about the persona
 *    Bad:  "Be helpful"
 *    Good: "You are a patient math tutor for high school students"
 *
 * 2. Define output format if needed
 *    "Always respond in JSON format"
 *    "Keep responses under 100 words"
 *
 * 3. Set boundaries
 *    "Only discuss topics related to cooking"
 *    "Do not provide medical advice"
 *
 * 4. Include examples
 *    "When greeting, say 'Howdy partner!' like a cowboy"
 *
 * 5. Order matters
 *    System message should always come FIRST
 *    Then conversation history (alternating user/assistant)
 *    Then the current user message
 *
 * C++ RAW STRING LITERALS:
 *   R"(...)" lets you write strings with quotes and newlines
 *   without escaping. Perfect for documentation!
 *
 *   R"(Hello "World")" is the same as "Hello \"World\""
 */
