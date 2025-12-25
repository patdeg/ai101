/**
 * ============================================================================
 * Example 6: Image Safety Check - Vision Content Moderation
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Using LlamaGuard with vision capabilities
 *   - Checking images for unsafe content
 *   - Combining base64 encoding with safety models
 *
 * WHY IMAGE SAFETY MATTERS:
 *   - User-uploaded images may contain inappropriate content
 *   - AI-generated images should be verified before display
 *   - Legal requirements for content moderation
 *
 * COMPILE:
 *   g++ -std=c++17 -o 06_image_safety_check 06_image_safety_check.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./06_image_safety_check
 *
 * ============================================================================
 */

#include <iostream>
#include <fstream>
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

/* Base64 encoding function */
std::string base64_encode(const std::vector<unsigned char> &data) {
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string result;
    result.reserve(4 * ((data.size() + 2) / 3));

    size_t i = 0;
    while (i < data.size()) {
        uint32_t octet_a = i < data.size() ? data[i++] : 0;
        uint32_t octet_b = i < data.size() ? data[i++] : 0;
        uint32_t octet_c = i < data.size() ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        result += table[(triple >> 18) & 0x3F];
        result += table[(triple >> 12) & 0x3F];
        result += table[(triple >> 6) & 0x3F];
        result += table[triple & 0x3F];
    }

    size_t mod = data.size() % 3;
    if (mod > 0) {
        result[result.size() - 1] = '=';
        if (mod == 1) result[result.size() - 2] = '=';
    }

    return result;
}

/* Read file into vector */
std::vector<unsigned char> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    return std::vector<unsigned char>(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

int main() {
    /* Step 1: Get API key */
    const char *api_key_cstr = std::getenv("DEMETERICS_API_KEY");
    if (api_key_cstr == nullptr) {
        std::cerr << "Error: DEMETERICS_API_KEY not set\n";
        return 1;
    }
    std::string api_key(api_key_cstr);

    /* Step 2: Read and encode image */
    std::string image_path = "../test_image.jpg";
    std::cout << "=== IMAGE SAFETY CHECKER ===\n";
    std::cout << "Using LlamaGuard Vision to check image safety\n\n";
    std::cout << "Checking image: " << image_path << "\n";

    std::vector<unsigned char> image_data;
    try {
        image_data = read_file(image_path);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Make sure test_image.jpg exists in parent directory\n";
        return 1;
    }

    std::string image_base64 = base64_encode(image_data);
    std::cout << "Image size: " << image_data.size() << " bytes\n";
    std::cout << "Encoded length: " << image_base64.size() << " characters\n\n";

    /* Step 3: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* Step 4: Build JSON request with image for LlamaGuard */
    std::string data_url = "data:image/jpeg;base64," + image_base64;

    json request_body = {
        {"model", "meta-llama/llama-guard-4-12b"},
        {"messages", {
            {
                {"role", "user"},
                {"content", {
                    {
                        {"type", "image_url"},
                        {"image_url", {{"url", data_url}}}
                    }
                }}
            }
        }},
        {"max_tokens", 100}
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

    std::cout << "Analyzing image for safety...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 7: Parse and display result */
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                std::string result = response["choices"][0]["message"]["content"];

                if (result.substr(0, 4) == "safe") {
                    std::cout << "Result: SAFE\n";
                    std::cout << "The image does not contain unsafe content.\n";
                } else {
                    std::cout << "Result: UNSAFE\n";
                    std::cout << "Raw response: " << result << "\n";
                }
            }
        } catch (const json::exception &e) {
            std::cerr << "Error parsing response: " << e.what() << "\n";
        }
    }

    /* Step 8: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    /* Educational info */
    std::cout << "\n=== IMAGE SAFETY CATEGORIES ===\n\n";
    std::cout << "LlamaGuard checks images for:\n";
    std::cout << "- Violence and gore\n";
    std::cout << "- Adult/sexual content\n";
    std::cout << "- Hate symbols\n";
    std::cout << "- Dangerous activities\n";
    std::cout << "- Child safety issues\n\n";

    std::cout << "=== WHEN TO USE IMAGE SAFETY ===\n\n";
    std::cout << "1. USER UPLOADS: Check all user-submitted images\n";
    std::cout << "2. AI GENERATION: Verify AI-generated images before display\n";
    std::cout << "3. CONTENT FEEDS: Filter images in social features\n";
    std::cout << "4. AVATARS: Validate profile pictures\n";

    return 0;
}
