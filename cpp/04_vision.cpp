/**
 * ============================================================================
 * Example 4: Vision - Image Analysis with AI
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 *   - Sending images to AI vision models
 *   - Base64 encoding images in C++
 *   - Multimodal (text + image) API requests
 *
 * WHAT YOU'LL LEARN:
 *   - How vision models process images
 *   - Base64 encoding with modern C++
 *   - File I/O with std::ifstream
 *   - Building complex nested JSON
 *
 * COMPILE:
 *   g++ -std=c++17 -o 04_vision 04_vision.cpp -lcurl -I lib/
 *
 * RUN:
 *   ./04_vision
 *
 * NOTE: Requires test_image.jpg in the parent directory
 *
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
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

/* ============================================================================
 * Base64 encoding function
 * ============================================================================
 * Base64 converts binary data to ASCII text using 64 safe characters.
 * This is necessary because JSON can't contain raw binary data.
 */
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

    /* Add padding */
    size_t mod = data.size() % 3;
    if (mod > 0) {
        result[result.size() - 1] = '=';
        if (mod == 1) {
            result[result.size() - 2] = '=';
        }
    }

    return result;
}

/* Read entire file into a vector */
std::vector<unsigned char> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    /* Read file into vector */
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

    /* Step 2: Read and encode image file */
    std::string image_path = "../test_image.jpg";
    std::cout << "Reading image: " << image_path << "\n";

    std::vector<unsigned char> image_data;
    try {
        image_data = read_file(image_path);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Make sure test_image.jpg exists in the parent directory\n";
        return 1;
    }

    std::cout << "Image size: " << image_data.size() << " bytes\n";

    /* Step 3: Base64 encode the image */
    std::string image_base64 = base64_encode(image_data);
    std::cout << "Base64 length: " << image_base64.size() << " characters\n\n";

    /* Step 4: Initialize curl */
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        curl_global_cleanup();
        return 1;
    }

    /* ========================================================================
     * Step 5: Build JSON request with image
     * ========================================================================
     * Vision models accept images as base64-encoded data URLs.
     * Format: "data:image/jpeg;base64,<base64_data>"
     *
     * nlohmann/json makes this nested structure easy!
     */

    std::string data_url = "data:image/jpeg;base64," + image_base64;

    json request_body = {
        {"model", "meta-llama/llama-4-scout-17b-16e-instruct"},
        {"messages", {
            {
                {"role", "user"},
                {"content", {
                    /* Text part: the question about the image */
                    {
                        {"type", "text"},
                        {"text", "What do you see in this image? Describe it in detail."}
                    },
                    /* Image part: the base64-encoded image */
                    {
                        {"type", "image_url"},
                        {"image_url", {{"url", data_url}}}
                    }
                }}
            }
        }},
        {"max_tokens", 500}
    };

    std::string json_payload = request_body.dump();

    /* Step 6: Set up headers */
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());

    /* Step 7: Configure and perform request */
    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.demeterics.com/groq/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    std::cout << "Analyzing image with AI vision...\n\n";
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "Error: " << curl_easy_strerror(res) << "\n";
    } else {
        /* Step 8: Parse and display response */
        try {
            json response = json::parse(response_data);

            if (response.contains("choices") && !response["choices"].empty()) {
                std::string answer = response["choices"][0]["message"]["content"];
                std::cout << "AI Vision Analysis:\n";
                std::cout << "==================\n";
                std::cout << answer << "\n";
            }

            /* Display token usage */
            if (response.contains("usage")) {
                std::cout << "\nToken Usage:\n";
                std::cout << "  Prompt: " << response["usage"]["prompt_tokens"] << "\n";
                std::cout << "  Response: " << response["usage"]["completion_tokens"] << "\n";
            }
        } catch (const json::exception &e) {
            std::cerr << "Error parsing response: " << e.what() << "\n";
            std::cerr << "Raw response: " << response_data << "\n";
        }
    }

    /* Step 9: Clean up */
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

/*
 * VISION API NOTES:
 *
 * 1. IMAGE FORMATS:
 *    - JPEG, PNG, GIF, WebP are supported
 *    - Use appropriate MIME type in data URL
 *
 * 2. IMAGE SIZE:
 *    - Large images are automatically resized
 *    - More pixels = more tokens = higher cost
 *    - Consider resizing before sending
 *
 * 3. DATA URL FORMAT:
 *    data:<mime_type>;base64,<base64_data>
 *    Example: data:image/jpeg;base64,/9j/4AAQ...
 *
 * 4. ALTERNATIVE: URL
 *    Instead of base64, you can provide a URL:
 *    {"type": "image_url", "image_url": {"url": "https://..."}}
 *
 * C++ CONCEPTS USED:
 *
 * std::vector<unsigned char>
 *   Dynamic array for binary data. Manages its own memory.
 *
 * std::ifstream with std::ios::binary
 *   Binary file input. Essential for images!
 *
 * std::istreambuf_iterator
 *   Iterator to read file contents into a container.
 *   Elegant one-liner to read entire file!
 *
 * throw / try / catch
 *   Exception handling. Cleaner than error codes.
 *
 * string::reserve()
 *   Pre-allocate memory for better performance.
 */
