/**
 * ============================================================================
 * Example 9: Tavily Search - Web Search with AI
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using Tavily API for web search
 *   - Getting AI-powered search results
 *   - Working with a different API (not Groq)
 *
 * WHAT YOU'LL LEARN:
 *   - Tavily API structure
 *   - Web search integration
 *   - Parsing search results
 *
 * COMPILE:
 *   g++ -std=c++17 -o 09_tavily_search 09_tavily_search.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./09_tavily_search
 *
 * NOTE: Requires TAVILY_API_KEY environment variable
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
    const char *api_key_cstr = std::getenv("TAVILY_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: TAVILY_API_KEY environment variable not set\n";
        std::cerr << "Get your free API key at: https://tavily.com\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Define search query */
    std::string search_query = "What are the latest developments in renewable energy 2024?";

    std::cout << "=== TAVILY WEB SEARCH ===\n";
    std::cout << "Query: " << search_query << "\n\n";

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build JSON request for Tavily */
    json request_body = {
        {"api_key", api_key},
        {"query", search_query},
        {"search_depth", "basic"},     /* "basic" or "advanced" */
        {"include_answer", true},       /* Get AI-generated answer */
        {"max_results", 5}              /* Number of results */
    };

    std::string json_payload = request_body.dump();

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* Step 6: Configure curl */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.tavily.com/search");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    /* Step 7: Perform search */
    std::cout << "Searching the web...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 8: Parse and display results */
        try {
            json response = json::parse(response_data);

            /* Display AI-generated answer */
            if (response.contains("answer")) {
                std::cout << "=== AI ANSWER ===\n";
                std::cout << response["answer"].get<std::string>() << "\n\n";
            }

            /* Display search results */
            if (response.contains("results") && response["results"].is_array()) {
                std::cout << "=== SEARCH RESULTS ===\n\n";

                int i = 1;
                for (const auto &result : response["results"]) {
                    std::cout << "Result " << i++ << ":\n";

                    if (result.contains("title")) {
                        std::cout << "  Title: " << result["title"] << "\n";
                    }
                    if (result.contains("url")) {
                        std::cout << "  URL: " << result["url"] << "\n";
                    }
                    if (result.contains("score")) {
                        std::cout << "  Relevance: " << result["score"] << "\n";
                    }
                    if (result.contains("content")) {
                        std::string content = result["content"];
                        /* Truncate long content */
                        if (content.length() > 200) {
                            content = content.substr(0, 200) + "...";
                        }
                        std::cout << "  Snippet: " << content << "\n";
                    }
                    std::cout << "\n";
                }
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

    /* Educational info */
    std::cout << "=== TAVILY API FEATURES ===\n\n";
    std::cout << "SEARCH DEPTHS:\n";
    std::cout << "  basic    - Fast, surface-level results\n";
    std::cout << "  advanced - Deeper search, more comprehensive\n\n";

    std::cout << "PARAMETERS:\n";
    std::cout << "  query           - Your search query\n";
    std::cout << "  include_answer  - Get AI-synthesized answer\n";
    std::cout << "  max_results     - Number of results (1-10)\n";
    std::cout << "  search_depth    - basic or advanced\n";
    std::cout << "  include_domains - Only search specific domains\n";
    std::cout << "  exclude_domains - Skip specific domains\n\n";

    std::cout << "USE CASES:\n";
    std::cout << "  1. Research assistants\n";
    std::cout << "  2. Fact-checking tools\n";
    std::cout << "  3. News aggregation\n";
    std::cout << "  4. RAG (Retrieval Augmented Generation)\n";

    return 0;
}

/*
 * C++ ITERATION OVER JSON ARRAY:
 *
 * for (const auto &item : response["array"]) {
 *     // item is each element
 * }
 *
 * This works because nlohmann/json implements iterators!
 *
 * TAVILY API NOTES:
 *
 * Tavily is designed specifically for AI applications:
 *   - Returns clean, structured content
 *   - Provides AI-synthesized answers
 *   - Optimized for RAG pipelines
 *
 * AUTHENTICATION:
 *   - API key in request body (not header)
 *   - Free tier available
 */
