/**
 * ============================================================================
 * Example 1: Basic Chat - Your First AI API Call in C++
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Making an HTTPS request to an AI API using libcurl
 *   - Building and parsing JSON with nlohmann/json (feels like Python!)
 *   - Modern C++ patterns: RAII, std::string, exceptions
 *   - Working with environment variables
 *
 * WHAT YOU'LL LEARN:
 *   - libcurl integration in C++
 *   - nlohmann/json intuitive syntax
 *   - C++ string handling (no manual memory management!)
 *   - Error handling with exceptions
 *
 * PREREQUISITES:
 *   - libcurl installed (see README.md)
 *   - nlohmann/json.hpp in lib/ folder or installed system-wide
 *   - DEMETERICS_API_KEY environment variable set
 *
 * COMPILE:
 *   g++ -std=c++17 -o 01_basic_chat 01_basic_chat.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./01_basic_chat
 *
 * ============================================================================
 */

#include <iostream>     /* std::cout, std::cerr */
#include <string>       /* std::string */
#include <cstdlib>      /* getenv */
#include <curl/curl.h>  /* libcurl for HTTP requests */
#include "json.hpp"     /* nlohmann/json for JSON handling */

/* Use the nlohmann namespace for convenience */
using json = nlohmann::json;

/* ============================================================================
 * Step 1: Callback function for libcurl to write response data
 * ============================================================================
 * libcurl calls this function each time it receives data from the server.
 * We append the received data to a std::string (passed as userdata).
 *
 * Unlike C, we use std::string which manages its own memory!
 */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    std::string *response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), realsize);
    return realsize;
}

/* ============================================================================
 * MAIN FUNCTION
 * ============================================================================
 */
int main() {
    /* Step 2: Get API key from environment variable */
    const char *api_key_cstr = std::getenv("DEMETERICS_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: DEMETERICS_API_KEY environment variable not set\n";
        std::cerr << "Run: export DEMETERICS_API_KEY='your_key_here'\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 3: Initialize libcurl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    if (curl == nullptr) {
        std::cerr << "Error: Failed to initialize curl\n";
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build the JSON request body using nlohmann/json */
    /* This syntax is incredibly intuitive - almost like Python! */
    json request_body = {
        {"model", "meta-llama/llama-4-scout-17b-16e-instruct"},
        {"messages", {
            {{"role", "user"}, {"content", "What is the capital of Switzerland?"}}
        }},
        {"temperature", 0.7},
        {"max_tokens", 100}
    };

    /* Convert JSON to string for HTTP request */
    std::string json_payload = request_body.dump();

    /* Step 5: Set up HTTP headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 6: Initialize response string (no malloc needed in C++!) */
    std::string response_data;

    /* Step 7: Configure curl options */
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    /* Step 8: Perform the HTTP request */
    std::cout << "Sending request to AI API...\n\n";
    CURLcode res = curl_easy_perform(curl);

    /* Step 9: Check for curl errors */
    if (res != CURLE_OK) {
        std::cerr << "Error: curl request failed: " << curl_easy_strerror(res) << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 10: Parse the JSON response */
    json response;
    try {
        response = json::parse(response_data);
    } catch (const json::parse_error &e) {
        std::cerr << "Error: Failed to parse response JSON: " << e.what() << "\n";
        std::cerr << "Raw response: " << response_data << "\n";
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    /* Step 11: Display the full response (pretty-printed) */
    std::cout << "Full Response:\n";
    std::cout << response.dump(2) << "\n";  /* 2 = indent spaces */

    /* Step 12: Extract and display the AI's answer */
    /* nlohmann/json makes this beautifully simple! */
    if (response.contains("choices") && !response["choices"].empty()) {
        std::string answer = response["choices"][0]["message"]["content"];
        std::cout << "\nAI Answer:\n" << answer << "\n";
    }

    /* Step 13: Display token usage */
    if (response.contains("usage")) {
        auto usage = response["usage"];
        std::cout << "\nToken Usage:\n";
        std::cout << "  Prompt: " << usage["prompt_tokens"] << "\n";
        std::cout << "  Response: " << usage["completion_tokens"] << "\n";
        std::cout << "  Total: " << usage["total_tokens"] << "\n";
    }

    /* Step 14: Clean up curl resources */
    /* Note: std::string and json clean themselves up automatically! (RAII) */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

/*
 * C++ CONCEPTS EXPLAINED:
 *
 * #include <iostream>
 *   C++ I/O library. Use std::cout for output, std::cin for input.
 *
 * std::string
 *   C++ string class. Manages its own memory - no malloc/free needed!
 *   Much safer than C-style char* strings.
 *
 * std::getenv("VAR")
 *   Same as C's getenv(), returns char* or nullptr.
 *
 * nullptr
 *   C++ null pointer (safer than NULL or 0).
 *
 * static_cast<Type*>(ptr)
 *   Safe type conversion between related pointer types.
 *   Checked at compile time.
 *
 * using json = nlohmann::json;
 *   Creates an alias for the nlohmann::json type.
 *   Like "typedef" but more powerful.
 *
 * json::parse(string)
 *   Parses a JSON string into a json object.
 *   Throws json::parse_error on failure.
 *
 * json.dump(indent)
 *   Converts json object to string.
 *   indent = pretty-print indentation (2 spaces).
 *
 * json["key"]
 *   Access JSON field by key (like Python dict).
 *   Throws exception if key doesn't exist.
 *
 * json.contains("key")
 *   Check if key exists without throwing.
 *
 * try { ... } catch (const Exception &e) { ... }
 *   Exception handling. Catches errors thrown by functions.
 *   e.what() returns error message.
 *
 * auto
 *   Compiler figures out the type automatically.
 *   auto x = 5;  // x is int
 *   auto s = "hello";  // s is const char*
 *
 * RAII (Resource Acquisition Is Initialization):
 *   Objects clean up after themselves when they go out of scope.
 *   std::string, std::vector, json all do this.
 *   No need for manual free() like in C!
 *
 * NLOHMANN/JSON SYNTAX:
 *   Creating JSON (feels like Python!):
 *     json j = {{"key", "value"}, {"number", 42}};
 *     json arr = {1, 2, 3, 4, 5};
 *     json nested = {{"user", {{"name", "Alice"}, {"age", 30}}}};
 *
 *   Reading JSON:
 *     std::string name = j["user"]["name"];
 *     int age = j["user"]["age"];
 *     for (auto& item : j["array"]) { ... }
 *
 * C++ vs C - KEY DIFFERENCES:
 *   1. std::string instead of char* (automatic memory)
 *   2. Exceptions instead of return codes (optional)
 *   3. RAII - objects clean up automatically
 *   4. << operator for output (std::cout << "hello")
 *   5. nullptr instead of NULL
 *   6. References (&) instead of just pointers (*)
 */
