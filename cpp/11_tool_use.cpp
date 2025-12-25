/**
 * ============================================================================
 * Example 11: Tool Use - AI Agents with Function Calling
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Giving AI the ability to call functions/tools
 *   - Building an AI agent that can take actions
 *   - Tool definition and execution loop
 *
 * WHAT YOU'LL LEARN:
 *   - OpenAI-compatible tool calling format
 *   - Agent loop pattern
 *   - Combining AI decisions with external actions
 *
 * COMPILE:
 *   g++ -std=c++17 -o 11_tool_use 11_tool_use.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./11_tool_use
 *
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <map>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    std::string *response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

/* ============================================================================
 * TOOL IMPLEMENTATIONS
 * ============================================================================
 * These are the actual functions the AI can call.
 */

std::string get_current_time() {
    time_t now = time(nullptr);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

std::string calculate(double a, double b, const std::string &operation) {
    double result = 0;
    if (operation == "add") result = a + b;
    else if (operation == "subtract") result = a - b;
    else if (operation == "multiply") result = a * b;
    else if (operation == "divide") result = (b != 0) ? a / b : 0;

    return std::to_string(result);
}

std::string get_weather(const std::string &location) {
    /* In real app, this would call a weather API */
    return "Weather in " + location + ": Sunny, 22C (72F), humidity 45%";
}

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("DEMETERICS_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: DEMETERICS_API_KEY not set\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Initialize */
    curl_global_init(CURL_GLOBAL_ALL);

    std::cout << "=== AI AGENT WITH TOOLS ===\n";
    std::cout << "The AI can call these tools:\n";
    std::cout << "  - get_current_time: Get current date/time\n";
    std::cout << "  - calculate: Perform math operations\n";
    std::cout << "  - get_weather: Get weather for a location\n\n";

    /* Step 3: Define the tools (functions) the AI can use */
    json tools = json::array({
        {
            {"type", "function"},
            {"function", {
                {"name", "get_current_time"},
                {"description", "Get the current date and time"},
                {"parameters", {
                    {"type", "object"},
                    {"properties", json::object()},
                    {"required", json::array()}
                }}
            }}
        },
        {
            {"type", "function"},
            {"function", {
                {"name", "calculate"},
                {"description", "Perform a mathematical calculation"},
                {"parameters", {
                    {"type", "object"},
                    {"properties", {
                        {"a", {{"type", "number"}, {"description", "First number"}}},
                        {"b", {{"type", "number"}, {"description", "Second number"}}},
                        {"operation", {
                            {"type", "string"},
                            {"enum", {"add", "subtract", "multiply", "divide"}},
                            {"description", "The operation to perform"}
                        }}
                    }},
                    {"required", {"a", "b", "operation"}}
                }}
            }}
        },
        {
            {"type", "function"},
            {"function", {
                {"name", "get_weather"},
                {"description", "Get current weather for a location"},
                {"parameters", {
                    {"type", "object"},
                    {"properties", {
                        {"location", {{"type", "string"}, {"description", "City name"}}}
                    }},
                    {"required", {"location"}}
                }}
            }}
        }
    });

    /* Step 4: User's request */
    std::string user_query = "What time is it, and what's 42 multiplied by 17?";
    std::cout << "User: " << user_query << "\n\n";

    /* Step 5: Build initial request */
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    json request_body = {
        {"model", "meta-llama/llama-4-scout-17b-16e-instruct"},
        {"messages", {
            {{"role", "user"}, {"content", user_query}}
        }},
        {"tools", tools},
        {"tool_choice", "auto"}
    };

    std::string json_payload = request_body.dump();

    /* Headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Configure and send */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    std::cout << "Sending request to AI...\n";
    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                auto message = response["choices"][0]["message"];

                if (message.contains("tool_calls") && message["tool_calls"].is_array()) {
                    std::cout << "\nAI wants to call " << message["tool_calls"].size()
                              << " tool(s):\n\n";

                    /* Process each tool call */
                    for (const auto &call : message["tool_calls"]) {
                        std::string name = call["function"]["name"];
                        std::string args_str = call["function"]["arguments"];
                        json args = json::parse(args_str);

                        std::cout << "Tool: " << name << "\n";
                        std::cout << "Args: " << args_str << "\n";

                        /* Execute the tool */
                        std::string result;

                        if (name == "get_current_time") {
                            result = get_current_time();
                        }
                        else if (name == "calculate") {
                            double a = args["a"];
                            double b = args["b"];
                            std::string op = args["operation"];
                            result = calculate(a, b, op);
                        }
                        else if (name == "get_weather") {
                            std::string location = args["location"];
                            result = get_weather(location);
                        }

                        std::cout << "Result: " << result << "\n\n";
                    }
                } else {
                    /* No tool calls - direct response */
                    if (message.contains("content") && !message["content"].is_null()) {
                        std::cout << "\nAI Response: " << message["content"] << "\n";
                    }
                }
            }
        } catch (const json::exception &e) {
            std::cerr << "Parse error: " << e.what() << "\n";
        }
    } else {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    }

    /* Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    std::cout << "=== TOOL USE PATTERN ===\n\n";
    std::cout << "1. Define tools with JSON schema\n";
    std::cout << "2. Send user query + tool definitions\n";
    std::cout << "3. AI returns tool_calls if needed\n";
    std::cout << "4. Execute tools locally\n";
    std::cout << "5. Send results back to AI\n";
    std::cout << "6. AI generates final response\n\n";

    std::cout << "USE CASES:\n";
    std::cout << "  - Web search agents\n";
    std::cout << "  - Database query agents\n";
    std::cout << "  - API integration\n";
    std::cout << "  - Code execution\n";
    std::cout << "  - Multi-step reasoning\n";

    return 0;
}

/*
 * C++ CONCEPTS:
 *
 * std::to_string(number)
 *   Converts number to string.
 *
 * json::array({...})
 *   Creates a JSON array with initializer list.
 *
 * json::object()
 *   Creates an empty JSON object.
 *
 * TOOL CALLING NOTES:
 *
 * TOOL DEFINITION SCHEMA:
 *   - name: Function identifier
 *   - description: What the function does
 *   - parameters: JSON Schema for arguments
 *
 * AI DECIDES:
 *   - Which tool to call (or none)
 *   - What arguments to pass
 *   - Can call multiple tools
 */
