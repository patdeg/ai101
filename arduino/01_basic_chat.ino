/*
 * 01_basic_chat.ino - Basic Chat Example for XIAO ESP32-S3 Sense
 *
 * WHAT THIS EXAMPLE DEMONSTRATES:
 * This Arduino sketch shows you how to make a basic chat request to Groq's AI models
 * through the Demeterics observability proxy
 * using the XIAO ESP32-S3 Sense board with WiFi. It's the simplest possible
 * example - just send a question and get a response back.
 *
 * WHAT YOU'LL LEARN:
 * - How to connect your ESP32 board to WiFi
 * - How to make HTTPS requests to an AI API
 * - How to build JSON requests using ArduinoJson
 * - How to parse JSON responses and extract AI-generated text
 * - How to diagnose network connectivity issues
 *
 * HARDWARE REQUIRED:
 * - Seeed Studio XIAO ESP32-S3 Sense (or any ESP32 board)
 *   https://www.amazon.com/dp/B0C69FFVHH
 *
 * LIBRARIES REQUIRED (install via Arduino Library Manager):
 * - ArduinoJson by Benoit Blanchon (for JSON parsing)
 *
 * HOW TO USE:
 * 1. Update WIFI_SSID and WIFI_PASSWORD with your WiFi credentials
 * 2. Update DEMETERICS_API_KEY with your Demeterics Managed LLM Key from https://demeterics.com
 * 3. Upload to your ESP32 board
 * 4. Open Serial Monitor at 115200 baud to see output
 *
 * EXPECTED SERIAL MONITOR OUTPUT:
 * You should see:
 * - WiFi connection progress with IP address
 * - Network diagnostics (DNS resolution, TLS connection test)
 * - The AI's response (a joke)
 * - No token usage shown (simplified output)
 *
 * TROUBLESHOOTING:
 * - If TLS connection fails, the diagnostics will help identify:
 *   - DNS resolution problems
 *   - Firewall/AP isolation issues
 *   - Captive portal detection
 *
 * NOTE: All logic is in setup() - loop() is empty as per Arduino copy/paste pattern
 */

// WiFi and HTTPS libraries (built-in for ESP32)
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>  // For JSON serialization and parsing

// ============================================================================
// CONFIGURATION - UPDATE THESE VALUES
// ============================================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";         // Replace with your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password
const char* DEMETERICS_API_KEY = "YOUR_DEMETERICS_API_KEY";   // Replace with your Demeterics Managed LLM Key

// API endpoint configuration
const char* DEMETERICS_HOST = "api.demeterics.com";
const int HTTPS_PORT = 443;
const char* API_PATH = "/groq/v1/chat/completions";

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// ----------------------------------------------------------------------------
// checkGroqConnection() - Diagnose network connectivity to Demeterics Groq proxy
// ----------------------------------------------------------------------------
// This function performs a series of diagnostic tests to verify we can reach
// the Demeterics Groq proxy servers. It's useful for troubleshooting connection problems.
//
// WHAT IT TESTS:
// 1. DNS resolution - Can we convert "api.demeterics.com" to an IP address?
// 2. TLS connection - Can we establish an encrypted HTTPS connection?
// 3. HTTP layer - Can we send a simple request and get a response?
//
// WHY THIS IS USEFUL:
// - Network problems are common on ESP32 (WiFi issues, firewall rules, etc.)
// - This helps identify exactly where the connection is failing
// - Provides specific error messages for each failure point
//
// RETURNS: true if all tests pass, false if any test fails
bool checkGroqConnection() {
  Serial.println("\n=== Network Diagnostics ===");

  // Test 1: DNS Resolution
  // -------------------------
  // Before we can connect, we need to convert the hostname to an IP address
  // This is called DNS resolution (Domain Name System)
  IPAddress ip;
  Serial.print("Testing DNS resolution for ");
  Serial.print(DEMETERICS_HOST);
  Serial.print("... ");

  if (!WiFi.hostByName(DEMETERICS_HOST, ip)) {
    Serial.println("FAILED");
    Serial.println("ERROR: DNS lookup failed. Check your internet connection.");
    return false;
  }

  Serial.print("OK (");
  Serial.print(ip);
  Serial.println(")");

  // Test 2: TLS Connection
  // -------------------------
  // Try to establish an encrypted HTTPS connection to the server
  // This verifies that firewall/router isn't blocking HTTPS traffic
  WiFiClientSecure testClient;
  testClient.setInsecure();           // Skip certificate validation for this test
  testClient.setHandshakeTimeout(30); // Wait up to 30 seconds for TLS handshake

  Serial.print("Testing TLS connection to ");
  Serial.print(DEMETERICS_HOST);
  Serial.print(":443... ");

  if (!testClient.connect(DEMETERICS_HOST, HTTPS_PORT)) {
    Serial.println("FAILED");
    Serial.println("ERROR: Cannot establish TLS connection.");
    Serial.println("Possible causes:");
    Serial.println("  - Router/firewall blocking HTTPS");
    Serial.println("  - AP isolation enabled on WiFi network");
    Serial.println("  - Captive portal requiring login");
    Serial.println("Try connecting to a mobile hotspot to test.");
    return false;
  }

  Serial.println("OK");

  // Test 3: HTTP Layer
  // -------------------------
  // Send a simple HTTP HEAD request to verify the HTTP layer is working
  // HEAD requests are like GET but only return headers (no body)
  Serial.print("Testing HTTP layer... ");

  testClient.print(String("HEAD / HTTP/1.1\r\n") +
                   "Host: " + DEMETERICS_HOST + "\r\n" +
                   "Connection: close\r\n\r\n");

  // Wait for response (up to 5 seconds)
  unsigned long startTime = millis();
  while (testClient.connected() && !testClient.available() && millis() - startTime < 5000) {
    delay(10);
  }

  if (!testClient.available()) {
    Serial.println("WARNING");
    Serial.println("TLS connected but no HTTP response.");
    Serial.println("This might indicate a proxy or firewall issue.");
  } else {
    Serial.println("OK");
  }

  testClient.stop();
  Serial.println("=== Diagnostics Complete ===\n");
  return true;
}

// ----------------------------------------------------------------------------
// groqRequest() - Send a chat request to Demeterics Groq proxy and return the response
// ----------------------------------------------------------------------------
// This function handles the entire API request/response cycle:
// 1. Establish HTTPS connection
// 2. Build JSON request body
// 3. Send HTTP POST request with authentication
// 4. Read and parse the response
// 5. Extract the AI's message from the response
//
// PARAMETERS:
// - prompt: The question or message to send to the AI
//
// RETURNS: The AI's response as a String, or empty string on error
String groqRequest(const char* prompt) {
  // Create HTTPS client with security settings
  WiFiClientSecure client;
  client.setInsecure();           // Skip certificate validation (simpler for beginners)
  client.setHandshakeTimeout(30); // 30 second timeout for TLS handshake
  WiFi.setSleep(false);           // Disable WiFi power saving (prevents connection stalls)

  // Step 1: Connect to Demeterics Groq proxy server
  Serial.println("Connecting to Demeterics Groq proxy...");
  if (!client.connect(DEMETERICS_HOST, HTTPS_PORT)) {
    Serial.println("ERROR: Connection failed!");
    return "";
  }
  Serial.println("Connected!");

  // Step 2: Build JSON request body using ArduinoJson
  // ---------------------------------------------------
  // StaticJsonDocument allocates memory on the stack (fast, but limited size)
  // 512 bytes is enough for our simple request
  StaticJsonDocument<512> doc;

  // Set the AI model
  // meta-llama/llama-4-scout-17b-16e-instruct is the latest Llama model (Oct 2025)
  // It's fast, accurate, and cost-effective for general chat
  doc["model"] = "meta-llama/llama-4-scout-17b-16e-instruct";

  // Create the messages array
  // Chat APIs expect an array of message objects with "role" and "content"
  JsonArray messages = doc.createNestedArray("messages");

  // Add the user's message
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";      // Role can be "system", "user", or "assistant"
  userMessage["content"] = prompt;   // The actual question/prompt

  // Optional parameters to control AI behavior
  doc["temperature"] = 0.7;   // Randomness: 0.0 (focused) to 2.0 (creative)
  doc["max_tokens"] = 150;    // Maximum length of response (prevents very long answers)

  // Convert JSON document to a String for sending
  String requestBody;
  serializeJson(doc, requestBody);

  Serial.println("\nRequest:");
  Serial.println(requestBody);

  // Step 3: Build and send HTTP POST request
  // ---------------------------------------------------
  // HTTP requests have this structure:
  //   REQUEST_LINE (POST /path HTTP/1.1)
  //   HEADERS (Host, Content-Type, etc.)
  //   BLANK_LINE
  //   BODY (JSON data)

  String httpRequest =
    String("POST ") + API_PATH + " HTTP/1.1\r\n" +
    "Host: " + DEMETERICS_HOST + "\r\n" +
    "Content-Type: application/json\r\n" +
    "Authorization: Bearer " + DEMETERICS_API_KEY + "\r\n" +
    "Content-Length: " + requestBody.length() + "\r\n" +
    "Connection: close\r\n" +
    "\r\n" +  // Blank line separates headers from body
    requestBody;

  // Send the complete HTTP request
  client.print(httpRequest);
  Serial.println("Request sent, waiting for response...\n");

  // Step 4: Read the entire HTTP response
  // ---------------------------------------------------
  // We read everything into a String, then split headers from body
  String response = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      response += client.readStringUntil('\n') + "\n";
    }
  }
  client.stop();

  // Step 5: Extract JSON body from HTTP response
  // ---------------------------------------------------
  // HTTP responses have headers and body separated by "\r\n\r\n"
  // We only care about the JSON body (after the separator)
  int bodyStart = response.indexOf("\r\n\r\n");
  String jsonBody = (bodyStart >= 0) ? response.substring(bodyStart + 4) : response;

  // Step 6: Parse JSON response
  // ---------------------------------------------------
  // Use a larger buffer for responses (AI responses can be long)
  DynamicJsonDocument responseDoc(4096);
  DeserializationError error = deserializeJson(responseDoc, jsonBody);

  if (error) {
    Serial.print("ERROR: JSON parsing failed: ");
    Serial.println(error.c_str());
    Serial.println("Raw response:");
    Serial.println(jsonBody);
    return "";
  }

  // Step 7: Extract the AI's message from the response
  // ---------------------------------------------------
  // Demeterics Groq proxy response structure:
  // {
  //   "choices": [
  //     {
  //       "message": {
  //         "content": "The AI's response text here"
  //       }
  //     }
  //   ]
  // }

  if (responseDoc.containsKey("choices") &&
      responseDoc["choices"].is<JsonArray>() &&
      responseDoc["choices"].size() > 0) {
    // Extract content from first choice
    const char* content = responseDoc["choices"][0]["message"]["content"];
    return String(content);
  } else if (responseDoc.containsKey("error")) {
    // Handle API errors (invalid key, rate limits, etc.)
    const char* errorMsg = responseDoc["error"]["message"];
    Serial.print("API ERROR: ");
    Serial.println(errorMsg);
    return "";
  } else {
    // Unexpected response format - return raw JSON for debugging
    Serial.println("WARNING: Unexpected response format");
    return jsonBody;
  }
}

void setup() {
  // Step 1: Initialize serial communication
  // ---------------------------------------------------
  // Serial.begin() starts serial communication at 115200 baud (bits per second)
  // This lets us print debug messages and results to the Serial Monitor
  Serial.begin(115200);
  delay(1000);  // Small delay to let serial connection stabilize

  Serial.println("\n\n=== Demeterics Groq proxy Basic Chat Example ===");
  Serial.println("XIAO ESP32-S3 Sense\n");

  // Step 2: Connect to WiFi
  // ---------------------------------------------------
  // Before making internet requests, we must connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  // WiFi.begin() initiates the connection process (non-blocking)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for connection with 30-second timeout
  // WiFi.status() returns WL_CONNECTED when successful
  int wifiTimeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTimeout < 30) {
    delay(1000);
    Serial.print(".");
    wifiTimeout++;
  }

  // Check connection status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nERROR: WiFi connection failed!");
    Serial.println("Check your SSID and password.");
    return;  // Exit setup() - loop() will do nothing
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Disable WiFi power saving to prevent connection stalls
  WiFi.setSleep(false);

  // Step 3: Run network diagnostics
  // ---------------------------------------------------
  // Test DNS, TLS, and HTTP connectivity to Demeterics Groq proxy
  // This helps identify connection problems early
  if (!checkGroqConnection()) {
    Serial.println("\nWARNING: Diagnostics failed, but continuing anyway...");
    // You can uncomment the next line to stop on diagnostic failure:
    // return;
  }

  // Step 4: Make API request
  // ---------------------------------------------------
  // Send a simple question to the AI and get a response
  Serial.println("Asking AI for a joke...");
  String response = groqRequest("Tell me a joke");

  // Step 5: Display the response
  // ---------------------------------------------------
  if (response.length() > 0) {
    Serial.println("\n=== AI RESPONSE ===");
    Serial.println(response);
    Serial.println("\n=== DONE ===");
  } else {
    Serial.println("\nERROR: No response from AI");
  }
}

// ============================================================================
// ARDUINO LOOP FUNCTION - Runs repeatedly (empty in this example)
// ============================================================================
void loop() {
  // Empty - all logic is in setup() for easy copy/paste testing
  // In a real application, you might want to add periodic chat requests here
}
