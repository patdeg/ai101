/**
 * ============================================================================
 * Example 13: Code Execution - Python Sandbox with AI
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using AI models that can execute Python code
 *   - Sandboxed code execution
 *   - Getting computed results from AI
 *
 * WHAT YOU'LL LEARN:
 *   - Code interpreter capabilities
 *   - Safe code execution patterns
 *   - When to use code execution
 *
 * COMPILE:
 *   g++ -std=c++17 -o 13_code_execution 13_code_execution.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./13_code_execution
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

    /* Step 3: Define a problem that benefits from code execution */
    std::string user_query =
        "Calculate the first 10 Fibonacci numbers and find their sum. "
        "Show me the Python code you use and the result.";

    std::cout << "=== AI CODE EXECUTION ===\n";
    std::cout << "Using openai/gpt-oss-20b for code execution\n\n";
    std::cout << "Query: " << user_query << "\n\n";

    /* Step 4: Build request */
    json request_body = {
        {"model", "openai/gpt-oss-20b"},  /* Model with code execution */
        {"messages", {
            {
                {"role", "system"},
                {"content", "You are a helpful assistant that solves problems by writing "
                           "and executing Python code. Always show your code and explain the results."}
            },
            {
                {"role", "user"},
                {"content", user_query}
            }
        }},
        {"max_tokens", 1000}
    };

    std::string json_payload = request_body.dump();

    /* Step 5: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 6: Configure and perform request */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    std::cout << "Executing code...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 7: Parse and display results */
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                std::string answer = response["choices"][0]["message"]["content"];
                std::cout << "=== AI RESPONSE WITH CODE ===\n\n";
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
    std::cout << "\n=== CODE EXECUTION FEATURES ===\n\n";
    std::cout << "WHAT CAN IT DO?\n";
    std::cout << "  - Run Python code in sandbox\n";
    std::cout << "  - Complex calculations\n";
    std::cout << "  - Data analysis\n";
    std::cout << "  - Generate charts (returned as base64)\n\n";

    std::cout << "SAFETY:\n";
    std::cout << "  - Sandboxed environment\n";
    std::cout << "  - No network access\n";
    std::cout << "  - Limited file system\n";
    std::cout << "  - Time and memory limits\n\n";

    std::cout << "USE CASES:\n";
    std::cout << "  - Math homework help\n";
    std::cout << "  - Data analysis tasks\n";
    std::cout << "  - Algorithm verification\n";
    std::cout << "  - Chart generation\n";

    return 0;
}

/*
 * CODE EXECUTION NOTES:
 *
 * MODELS WITH CODE EXECUTION:
 *   - openai/gpt-oss-20b: Has code interpreter
 *   - Other models: Can write code but not execute
 *
 * SANDBOXING:
 *   Code runs in isolated environment:
 *   - No access to real file system
 *   - No network requests
 *   - Limited CPU time
 *   - Limited memory
 *
 * WHEN TO USE:
 *   - Precise calculations needed
 *   - Data processing required
 *   - Visualization generation
 *   - Algorithm testing
 */
