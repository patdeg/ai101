/**
 * ============================================================================
 * Example 10: Tavily Extract - Web Content Extraction
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using Tavily Extract API to get clean content from URLs
 *   - Extracting readable text from web pages
 *   - Preparing web content for AI processing
 *
 * WHAT YOU'LL LEARN:
 *   - Web scraping vs API extraction
 *   - Content cleaning for AI
 *   - Working with multiple URLs
 *
 * COMPILE:
 *   g++ -std=c++17 -o 10_tavily_extract 10_tavily_extract.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./10_tavily_extract
 *
 * NOTE: Requires TAVILY_API_KEY environment variable
 *
 * ============================================================================
 */

#include <iostream>
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

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("TAVILY_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: TAVILY_API_KEY environment variable not set\n";
        std::cerr << "Get your free API key at: https://tavily.com\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Define URLs to extract */
    std::vector<std::string> urls = {
        "https://en.wikipedia.org/wiki/Artificial_intelligence"
    };

    std::cout << "=== TAVILY CONTENT EXTRACTOR ===\n";
    std::cout << "Extracting clean content from web pages\n\n";

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build JSON request for Tavily Extract */
    json request_body = {
        {"api_key", api_key},
        {"urls", urls}
    };

    std::string json_payload = request_body.dump();

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    /* Step 6: Configure curl */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.tavily.com/extract");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    /* Step 7: Perform extraction */
    std::cout << "Extracting content from " << urls.size() << " URL(s)...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 8: Parse and display results */
        try {
            json response = json::parse(response_data);

            if (response.contains("results") && response["results"].is_array()) {
                for (const auto &result : response["results"]) {
                    std::cout << "=== EXTRACTED CONTENT ===\n";

                    if (result.contains("url")) {
                        std::cout << "URL: " << result["url"] << "\n\n";
                    }

                    if (result.contains("raw_content")) {
                        std::string content = result["raw_content"];

                        /* Show first 1000 characters */
                        if (content.length() > 1000) {
                            std::cout << "Content Preview:\n"
                                      << content.substr(0, 1000) << "\n";
                            std::cout << "\n... [Content truncated, total: "
                                      << content.length() << " characters]\n";
                        } else {
                            std::cout << "Content:\n" << content << "\n";
                        }
                    }
                    std::cout << "\n";
                }
            }

            /* Show failed URLs if any */
            if (response.contains("failed_results") &&
                response["failed_results"].is_array() &&
                !response["failed_results"].empty()) {

                std::cout << "=== FAILED EXTRACTIONS ===\n";
                for (const auto &fail : response["failed_results"]) {
                    if (fail.contains("url")) {
                        std::cout << "URL: " << fail["url"] << "\n";
                    }
                    if (fail.contains("error")) {
                        std::cout << "Error: " << fail["error"] << "\n";
                    }
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
    std::cout << "\n=== TAVILY EXTRACT FEATURES ===\n\n";
    std::cout << "WHAT IT DOES:\n";
    std::cout << "  - Fetches web pages\n";
    std::cout << "  - Removes ads, navigation, scripts\n";
    std::cout << "  - Returns clean, readable text\n";
    std::cout << "  - Handles JavaScript-rendered content\n\n";

    std::cout << "USE CASES:\n";
    std::cout << "  1. RAG (Retrieval Augmented Generation)\n";
    std::cout << "  2. Document summarization\n";
    std::cout << "  3. Content analysis\n";
    std::cout << "  4. Research automation\n\n";

    std::cout << "LIMITS:\n";
    std::cout << "  - Multiple URLs per request supported\n";
    std::cout << "  - Rate limits apply (check your plan)\n";
    std::cout << "  - Some sites may block extraction\n";

    return 0;
}

/*
 * C++ VECTOR TO JSON:
 *
 * nlohmann/json automatically converts std::vector to JSON array:
 *   std::vector<std::string> urls = {"url1", "url2"};
 *   json j = {{"urls", urls}};
 *   // Results in: {"urls": ["url1", "url2"]}
 *
 * WEB EXTRACTION NOTES:
 *
 * WHY USE AN EXTRACTION API?
 *   - Web pages have lots of noise (ads, scripts, navigation)
 *   - JavaScript rendering is complex
 *   - Clean text is better for AI processing
 *
 * TAVILY EXTRACT vs SCRAPING:
 *   - No need to parse HTML yourself
 *   - Handles anti-bot measures
 *   - Returns structured, clean content
 *   - Legal considerations (respects robots.txt)
 */
