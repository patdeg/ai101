/**
 * ============================================================================
 * Example 14: Reasoning - Step-by-Step Thinking with Prompt Caching
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using AI for complex reasoning tasks
 *   - Chain-of-thought prompting
 *   - Prompt caching for cost savings
 *
 * WHAT YOU'LL LEARN:
 *   - How to get AI to "think" through problems
 *   - Structured reasoning prompts
 *   - Caching benefits for repeated queries
 *
 * COMPILE:
 *   g++ -std=c++17 -o 14_reasoning 14_reasoning.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./14_reasoning
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

    /* Step 3: Define a problem requiring reasoning */
    std::string reasoning_problem = R"(A farmer has 17 sheep. All but 9 run away. How many sheep does the farmer have left?

Think through this step-by-step before giving your final answer.)";

    std::cout << "=== AI REASONING ===\n";
    std::cout << "Using chain-of-thought prompting for complex problems\n\n";
    std::cout << "Problem: " << reasoning_problem << "\n\n";

    /* Step 4: Build request with reasoning system prompt */
    std::string system_prompt = R"(You are a logical reasoning assistant. When solving problems:
1. Read the problem carefully - note any tricky wording
2. Identify what is being asked
3. Work through the logic step by step
4. Check your reasoning for errors
5. Provide a clear final answer

Take your time and think carefully before answering.)";

    json request_body = {
        {"model", "openai/gpt-oss-20b"},  /* Good for reasoning */
        {"messages", {
            {{"role", "system"}, {"content", system_prompt}},
            {{"role", "user"}, {"content", reasoning_problem}}
        }},
        {"max_tokens", 500},
        {"temperature", 0.3}  /* Lower = more focused */
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

    std::cout << "AI is thinking...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 7: Parse and display results */
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                std::string answer = response["choices"][0]["message"]["content"];
                std::cout << "=== AI REASONING CHAIN ===\n\n";
                std::cout << answer << "\n";
            }

            /* Token usage - check for caching */
            if (response.contains("usage")) {
                auto usage = response["usage"];
                std::cout << "\n=== TOKEN USAGE ===\n";
                std::cout << "Prompt tokens: " << usage["prompt_tokens"] << "\n";
                std::cout << "Completion tokens: " << usage["completion_tokens"] << "\n";

                if (usage.contains("prompt_cache_hit_tokens") &&
                    usage["prompt_cache_hit_tokens"].get<int>() > 0) {
                    std::cout << "Cached tokens: " << usage["prompt_cache_hit_tokens"]
                              << " (50% discount!)\n";
                }
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
    std::cout << "\n=== REASONING TECHNIQUES ===\n\n";
    std::cout << "CHAIN-OF-THOUGHT PROMPTING:\n";
    std::cout << "  Ask the AI to \"think step by step\"\n";
    std::cout << "  Helps with math, logic, and complex problems\n\n";

    std::cout << "PROMPT CACHING:\n";
    std::cout << "  - System prompts can be cached\n";
    std::cout << "  - Cached tokens cost 50% less\n";
    std::cout << "  - Great for repeated similar queries\n\n";

    std::cout << "TIPS FOR BETTER REASONING:\n";
    std::cout << "  1. Use lower temperature (0.1-0.3)\n";
    std::cout << "  2. Ask to \"check your work\"\n";
    std::cout << "  3. Break complex problems into parts\n";
    std::cout << "  4. Use explicit step-by-step instructions\n";

    return 0;
}

/*
 * C++ RAW STRING LITERALS:
 *
 * R"(...)" allows multi-line strings without escaping.
 * Perfect for long prompts and JSON templates!
 *
 * REASONING NOTES:
 *
 * CHAIN-OF-THOUGHT:
 *   Just adding "think step by step" can dramatically
 *   improve AI performance on reasoning tasks.
 *
 * PROMPT CACHING:
 *   - System prompts repeated across requests can be cached
 *   - Reduces latency and cost
 *   - Especially useful for long system prompts
 */
