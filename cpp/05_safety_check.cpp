/**
 * ============================================================================
 * Example 5: Safety Check - Content Moderation with LlamaGuard
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using AI safety models to check content
 *   - LlamaGuard for text content moderation
 *   - Building responsible AI applications
 *
 * WHY SAFETY MATTERS:
 *   - AI should refuse harmful requests
 *   - Content moderation protects users
 *   - Responsible AI is good AI
 *
 * COMPILE:
 *   g++ -std=c++17 -o 05_safety_check 05_safety_check.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./05_safety_check
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

/* Callback for curl to write response data */
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

/* Result structure for safety check */
struct SafetyResult {
    bool is_safe;
    std::string category;
    bool success;
};

/* Function to check if text is safe using LlamaGuard */
SafetyResult check_safety(const std::string &api_key, const std::string &text_to_check) {
    SafetyResult result{false, "", false};

    CURL *curl = curl_easy_init();
    if (curl == nullptr) return result;

    /* Build JSON request for LlamaGuard */
    json request_body = {
        {"model", "meta-llama/llama-guard-4-12b"},
        {"messages", {
            {{"role", "user"}, {"content", text_to_check}}
        }},
        {"max_tokens", 100}
    };

    std::string json_payload = request_body.dump();

    /* Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Configure curl */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                std::string response_text = response["choices"][0]["message"]["content"];

                /* LlamaGuard returns "safe" or "unsafe\n<category>" */
                if (response_text.substr(0, 4) == "safe") {
                    result.is_safe = true;
                    result.category = "none";
                } else {
                    result.is_safe = false;
                    /* Extract category if present */
                    size_t newline_pos = response_text.find('\n');
                    if (newline_pos != std::string::npos && newline_pos + 1 < response_text.size()) {
                        result.category = response_text.substr(newline_pos + 1);
                    } else {
                        result.category = "unknown";
                    }
                }
                result.success = true;
            }
        } catch (const json::exception &e) {
            /* Parse error, result.success stays false */
        }
    }

    /* Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

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

    /* Step 2: Initialize curl globally */
    curl_global_init(CURL_GLOBAL_ALL);

    std::cout << "=== AI CONTENT SAFETY CHECKER ===\n";
    std::cout << "Using LlamaGuard to check content safety\n\n";

    /* Step 3: Test with various inputs */
    std::vector<std::string> test_inputs = {
        "What is the capital of France?",
        "How do I bake chocolate chip cookies?",
        "Explain quantum computing in simple terms",
        "Tell me a joke about programmers"
    };

    for (const auto &input : test_inputs) {
        std::cout << "Checking: \"" << input << "\"\n";

        SafetyResult result = check_safety(api_key, input);

        if (result.success) {
            if (result.is_safe) {
                std::cout << "  Result: SAFE\n";
            } else {
                std::cout << "  Result: UNSAFE\n";
                std::cout << "  Category: " << result.category << "\n";
            }
        } else {
            std::cout << "  Error: Could not check safety\n";
        }
        std::cout << "\n";
    }

    /* Step 4: Clean up */
    curl_global_cleanup();

    /* Print educational information */
    std::cout << "=== LLAMAGUARD SAFETY CATEGORIES ===\n\n";
    std::cout << "S1: Violent Crimes\n";
    std::cout << "S2: Non-Violent Crimes\n";
    std::cout << "S3: Sex Crimes\n";
    std::cout << "S4: Child Exploitation\n";
    std::cout << "S5: Defamation\n";
    std::cout << "S6: Specialized Advice (medical, legal, financial)\n";
    std::cout << "S7: Privacy\n";
    std::cout << "S8: Intellectual Property\n";
    std::cout << "S9: Indiscriminate Weapons\n";
    std::cout << "S10: Hate\n";
    std::cout << "S11: Self-Harm\n";
    std::cout << "S12: Sexual Content\n";
    std::cout << "S13: Elections\n";
    std::cout << "S14: Code Interpreter Abuse\n\n";

    std::cout << "=== WHY SAFETY MATTERS ===\n\n";
    std::cout << "1. PROTECT USERS:\n";
    std::cout << "   - Prevent exposure to harmful content\n";
    std::cout << "   - Keep minors safe\n\n";
    std::cout << "2. LEGAL COMPLIANCE:\n";
    std::cout << "   - Many jurisdictions require content moderation\n";
    std::cout << "   - Avoid liability issues\n\n";
    std::cout << "3. RESPONSIBLE AI:\n";
    std::cout << "   - AI should help, not harm\n";
    std::cout << "   - Build trust with users\n\n";
    std::cout << "4. BEST PRACTICE:\n";
    std::cout << "   - Check BOTH user input AND AI output\n";
    std::cout << "   - Defense in depth!\n";

    return 0;
}

/*
 * C++ CONCEPTS USED:
 *
 * struct with default values:
 *   struct SafetyResult {
 *       bool is_safe;
 *       std::string category;
 *       bool success;
 *   };
 *   Can be initialized with: SafetyResult{false, "", false}
 *
 * std::string::substr(pos, len)
 *   Returns a substring starting at pos with length len.
 *   If len is omitted, returns to end of string.
 *
 * std::string::find(str)
 *   Returns position of str, or std::string::npos if not found.
 *
 * Range-based for loop:
 *   for (const auto &input : test_inputs) { ... }
 *   Iterates over each element in the container.
 *
 * SAFETY CHECK PATTERN:
 *
 * 1. CHECK USER INPUT
 *    Before sending to main AI, check if input is safe
 *    if (!check_safety(user_input).is_safe) {
 *        return "I can't help with that request.";
 *    }
 *
 * 2. CHECK AI OUTPUT
 *    Before showing to user, check if output is safe
 *    if (!check_safety(ai_response).is_safe) {
 *        return "I apologize, I generated inappropriate content.";
 *    }
 *
 * 3. DEFENSE IN DEPTH
 *    Multiple layers of protection:
 *    - Input validation
 *    - Prompt engineering (system prompts with rules)
 *    - Output filtering
 *    - Rate limiting
 *    - Logging for review
 */
