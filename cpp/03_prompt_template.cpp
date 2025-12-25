/**
 * ============================================================================
 * Example 3: Prompt Template - Dynamic Prompt Generation
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Building dynamic prompts from templates
 *   - Using std::regex for string replacement (modern C++)
 *   - Separating prompt structure from content
 *
 * WHAT YOU'LL LEARN:
 *   - Template patterns for reusable prompts
 *   - std::regex for pattern matching
 *   - How to build flexible AI interactions
 *
 * COMPILE:
 *   g++ -std=c++17 -o 03_prompt_template 03_prompt_template.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./03_prompt_template
 *
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <map>
#include <regex>
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

/* ============================================================================
 * Helper function: Replace template placeholders
 * ============================================================================
 * Takes a template string and a map of variables, returns the filled template.
 * Uses {{PLACEHOLDER}} syntax.
 */
std::string render_template(const std::string &template_str,
                            const std::map<std::string, std::string> &vars) {
    std::string result = template_str;

    for (const auto &[key, value] : vars) {
        std::string placeholder = "{{" + key + "}}";
        size_t pos;
        while ((pos = result.find(placeholder)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
        }
    }

    return result;
}

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("DEMETERICS_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: DEMETERICS_API_KEY not set\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* ========================================================================
     * Step 2: Define our prompt template
     * ========================================================================
     * Templates let us reuse the same prompt structure with different values.
     * We use {{PLACEHOLDER}} syntax (like many template engines).
     *
     * C++ raw strings R"(...)" are perfect for multi-line templates!
     */

    std::string template_str = R"(You are a {{ROLE}} expert.
Explain {{TOPIC}} to someone who is {{LEVEL}}.
Keep your explanation under {{MAX_WORDS}} words.
Focus on practical examples.)";

    /* Step 3: Define our template variables using a map */
    std::map<std::string, std::string> variables = {
        {"ROLE", "computer science"},
        {"TOPIC", "recursion"},
        {"LEVEL", "a beginner programmer"},
        {"MAX_WORDS", "100"}
    };

    /* Step 4: Render the template */
    std::string final_prompt = render_template(template_str, variables);

    std::cout << "=== GENERATED PROMPT ===\n";
    std::cout << final_prompt << "\n";
    std::cout << "========================\n\n";

    /* Step 5: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 6: Build JSON request */
    json request_body = {
        {"model", "meta-llama/llama-4-scout-17b-16e-instruct"},
        {"messages", {
            {{"role", "user"}, {"content", final_prompt}}
        }},
        {"temperature", 0.7},
        {"max_tokens", 200}
    };

    std::string json_payload = request_body.dump();

    /* Step 7: Set up headers and response */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    std::cout << "Sending templated request...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 8: Parse and display response */
        try {
            json response = json::parse(response_data);
            if (response.contains("choices") && !response["choices"].empty()) {
                std::string answer = response["choices"][0]["message"]["content"];
                std::cout << "AI Response:\n" << answer << "\n";
            }
        } catch (const json::exception &e) {
            std::cerr << "Error parsing response: " << e.what() << "\n";
        }
    }

    /* Step 9: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Show template examples */
    std::cout << "\n=== MORE TEMPLATE IDEAS ===\n\n";

    std::cout << "TRANSLATION TEMPLATE:\n";
    std::cout << R"(  "Translate the following {{SOURCE_LANG}} text to {{TARGET_LANG}}:
   {{TEXT}}")" << "\n\n";

    std::cout << "CODE REVIEW TEMPLATE:\n";
    std::cout << R"(  "Review this {{LANGUAGE}} code for {{REVIEW_TYPE}}:
   {{CODE}}")" << "\n\n";

    std::cout << "SUMMARIZATION TEMPLATE:\n";
    std::cout << R"(  "Summarize the following {{CONTENT_TYPE}} in {{FORMAT}}:
   {{CONTENT}}")" << "\n";

    return 0;
}

/*
 * C++ CONCEPTS USED:
 *
 * std::map<Key, Value>
 *   Ordered key-value container (like dict in Python)
 *   Access: map["key"] or map.at("key")
 *
 * Range-based for with structured bindings:
 *   for (const auto &[key, value] : map) { ... }
 *   Iterates over map entries, unpacking key and value
 *
 * std::string::find(substr)
 *   Returns position of substring, or std::string::npos if not found
 *
 * std::string::replace(pos, len, new_str)
 *   Replaces characters at position with new string
 *
 * R"(...)" raw string literals
 *   No escaping needed for quotes or newlines
 *   Perfect for templates!
 *
 * TEMPLATE PATTERNS:
 *
 * 1. Simple placeholders: {{VARIABLE}}
 *    Easy to spot, common convention
 *
 * 2. With defaults: {{VARIABLE:default_value}}
 *    Requires more complex parsing
 *
 * 3. Conditionals: {{#if CONDITION}}...{{/if}}
 *    For advanced template engines
 *
 * WHY TEMPLATES?
 *
 * 1. Reusability - Same prompt structure, different content
 * 2. Testability - Easy to test with known inputs
 * 3. Maintainability - Change prompt format in one place
 * 4. Separation - Content creators don't need to understand code
 */
