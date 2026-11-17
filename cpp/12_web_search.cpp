/**
 * ============================================================================
 * Example 12: Web Search - Built-in Web Search via OpenRouter :online
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using a model:online slug for built-in web search
 *   - Getting real-time information from the web
 *   - No separate search API needed
 *
 * WHAT YOU'LL LEARN:
 *   - The :online model-slug suffix
 *   - Web-grounded AI responses
 *   - Comparing to Tavily approach
 *
 * COMPILE:
 *   g++ -std=c++17 -o 12_web_search 12_web_search.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./12_web_search
 *
 * ============================================================================
 */

#include <iostream>
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

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("OPENROUTER_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: OPENROUTER_API_KEY not set\n";
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

    /* Step 3: Define a query that needs current information */
    std::string user_query = "What are the latest tech news today?";

    std::cout << "=== WEB-AUGMENTED CHAT (OpenRouter :online) ===\n";
    std::cout << "Appending :online to any model slug adds built-in web search\n\n";
    std::cout << "Query: " << user_query << "\n\n";

    /* Step 4: Build request. Appending ":online" to a model slug activates
     * OpenRouter's web-search plugin (default engine: Exa) for this call -
     * no separate search API needed, same idea as groq/compound-mini before. */
    json request_body = {
        {"model", "google/gemini-3.8-flash:online"},
        {"messages", {
            {{"role", "user"}, {"content", user_query}}
        }},
        {"max_tokens", 1200}
    };

    std::string json_payload = request_body.dump();

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 6: Configure and perform request */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    std::cout << "Searching the web...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 7: Parse and display results */
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                std::string answer = response["choices"][0]["message"]["content"];
                std::cout << "=== WEB-GROUNDED RESPONSE ===\n\n";
                std::cout << answer << "\n";
            }

            /* Token usage */
            if (response.contains("usage")) {
                std::cout << "\n=== TOKEN USAGE ===\n";
                std::cout << "Prompt: " << response["usage"]["prompt_tokens"] << "\n";
                std::cout << "Response: " << response["usage"]["completion_tokens"] << "\n";
            }

        } catch (const json::exception &e) {
            std::cerr << "Parse error: " << e.what() << "\n";
            std::cout << "Raw response: " << response_data << "\n";
        }
    }

    /* Step 8: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    std::cout << "\n=== :online SUFFIX FEATURES ===\n\n";
    std::cout << "WHAT DOES :online DO?\n";
    std::cout << "  Appended to any OpenRouter model slug, it runs a web search\n";
    std::cout << "  (default engine Exa, ~$0.005/request) and injects the results\n";
    std::cout << "  as context before the model answers.\n\n";

    std::cout << "COMPARISON:\n";
    std::cout << "  Tavily + chat model:  Separate API calls, more control\n";
    std::cout << "  model:online:         Single call, automatic search\n\n";

    std::cout << "WHEN TO USE:\n";
    std::cout << "  - Quick web-grounded answers\n";
    std::cout << "  - Current events and news\n";
    std::cout << "  - Simple research questions\n\n";

    std::cout << "WHEN TO USE TAVILY INSTEAD:\n";
    std::cout << "  - Need full control over search\n";
    std::cout << "  - Domain-specific filtering\n";
    std::cout << "  - Complex RAG pipelines\n";

    return 0;
}

/*
 * :online SUFFIX NOTES:
 *
 * <model>:online:
 *   - Has built-in web search capability
 *   - Automatically searches, then grounds the response in results
 *   - Returns web-grounded responses
 *
 * BENEFITS:
 *   - Simpler API integration
 *   - One endpoint for everything
 *   - No need to manage search separately
 *
 * LIMITATIONS:
 *   - Less control over search parameters
 *   - Can't filter domains easily
 *   - May not search when you expect it to
 */
