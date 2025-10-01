/*
 * 01_basic_chat.ino - Basic Chat Example for ESP32/ESP8266
 *
 * This Arduino sketch demonstrates how to make a basic chat request to Groq API
 * using an ESP32 or ESP8266 board with WiFi capability.
 *
 * HARDWARE REQUIRED:
 * - ESP32 or ESP8266 board (these have built-in WiFi)
 *
 * LIBRARIES REQUIRED (install via Arduino Library Manager):
 * - ArduinoJson by Benoit Blanchon (for JSON parsing)
 * - WiFiClientSecure (usually built-in for ESP32/ESP8266)
 *
 * HOW TO USE:
 * 1. Update WIFI_SSID and WIFI_PASSWORD with your WiFi credentials
 * 2. Update GROQ_API_KEY with your Groq API key from https://console.groq.com
 * 3. Upload to your ESP32/ESP8266 board
 * 4. Open Serial Monitor at 115200 baud to see output
 *
 * NOTE: All logic is in setup() - loop() is empty as per Arduino copy/paste pattern
 */

// Include WiFi library - different for ESP32 vs ESP8266
#if defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
#endif

#include <ArduinoJson.h>  // For JSON serialization and parsing

// ============================================================================
// CONFIGURATION - UPDATE THESE VALUES
// ============================================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";         // Replace with your WiFi network name
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password
const char* GROQ_API_KEY = "YOUR_GROQ_API_KEY";   // Replace with your Groq API key

// API endpoint configuration
const char* GROQ_HOST = "api.groq.com";
const int HTTPS_PORT = 443;
const char* API_PATH = "/openai/v1/chat/completions";

// ============================================================================
// ARDUINO SETUP FUNCTION - Runs once when board starts
// ============================================================================
void setup() {
  // Initialize serial communication at 115200 baud rate
  // This allows us to print debug messages and results to Serial Monitor
  Serial.begin(115200);

  // Small delay to let serial connection stabilize
  delay(1000);

  Serial.println("\n\n=== Groq API Basic Chat Example ===");
  Serial.println("Starting ESP32/ESP8266...\n");

  // ------------------------------------------------------------------------
  // STEP 1: Connect to WiFi
  // ------------------------------------------------------------------------
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  // Begin WiFi connection with SSID and password
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection (timeout after 30 seconds)
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 30) {
    delay(1000);
    Serial.print(".");
    wifi_timeout++;
  }

  // Check if WiFi connected successfully
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nERROR: WiFi connection failed!");
    Serial.println("Please check your SSID and password.");
    return; // Exit setup() - board will restart
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ------------------------------------------------------------------------
  // STEP 2: Create HTTPS client
  // ------------------------------------------------------------------------
  WiFiClientSecure client;

  // For production, you should verify SSL certificates
  // For simplicity in this example, we skip certificate validation
  // WARNING: This makes the connection less secure
  #if defined(ESP32)
    client.setInsecure(); // ESP32: Skip certificate validation
  #elif defined(ESP8266)
    client.setInsecure(); // ESP8266: Skip certificate validation
  #endif

  Serial.println("\nConnecting to Groq API...");

  // Connect to Groq API server on port 443 (HTTPS)
  if (!client.connect(GROQ_HOST, HTTPS_PORT)) {
    Serial.println("ERROR: Connection to Groq API failed!");
    return;
  }

  Serial.println("Connected to Groq API!");

  // ------------------------------------------------------------------------
  // STEP 3: Prepare JSON request body
  // ------------------------------------------------------------------------
  // ArduinoJson uses a pre-allocated buffer for JSON documents
  // StaticJsonDocument allocates memory on the stack (fast, but limited size)
  // DynamicJsonDocument allocates on heap (slower, but more flexible)
  // Size is in bytes - we need enough for our request
  StaticJsonDocument<512> requestDoc;

  // Set the model we want to use
  requestDoc["model"] = "llama-3.1-8b-instant";

  // Create the messages array
  JsonArray messages = requestDoc.createNestedArray("messages");

  // Add a user message
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";
  userMessage["content"] = "Explain the concept of recursion in programming in one sentence.";

  // Optional: Set temperature for response randomness (0.0 to 2.0)
  requestDoc["temperature"] = 0.7;

  // Optional: Set max tokens for the response
  requestDoc["max_tokens"] = 150;

  // Serialize JSON to string
  String requestBody;
  serializeJson(requestDoc, requestBody);

  Serial.println("\nRequest Body:");
  Serial.println(requestBody);

  // ------------------------------------------------------------------------
  // STEP 4: Send HTTP POST request
  // ------------------------------------------------------------------------
  // Build HTTP headers
  String request = String("POST ") + API_PATH + " HTTP/1.1\r\n";
  request += String("Host: ") + GROQ_HOST + "\r\n";
  request += "Content-Type: application/json\r\n";
  request += String("Authorization: Bearer ") + GROQ_API_KEY + "\r\n";
  request += String("Content-Length: ") + requestBody.length() + "\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";
  request += requestBody;

  // Send the request
  client.print(request);
  Serial.println("\nHTTP request sent!");

  // ------------------------------------------------------------------------
  // STEP 5: Read and parse response
  // ------------------------------------------------------------------------
  Serial.println("\nWaiting for response...\n");

  // Wait for response (timeout after 10 seconds)
  int timeout = 0;
  while (!client.available() && timeout < 10000) {
    delay(100);
    timeout += 100;
  }

  if (!client.available()) {
    Serial.println("ERROR: Response timeout!");
    client.stop();
    return;
  }

  // Skip HTTP headers - read until we find empty line "\r\n\r\n"
  bool headersEnded = false;
  String line = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      line += c;

      // Check for end of headers
      if (line.endsWith("\r\n\r\n")) {
        headersEnded = true;
        break;
      }
    }
  }

  if (!headersEnded) {
    Serial.println("ERROR: Failed to parse HTTP headers!");
    client.stop();
    return;
  }

  // Read the JSON response body
  String responseBody = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      responseBody += c;
    }
  }

  client.stop();

  Serial.println("Response received!");
  Serial.println("\nRaw JSON Response:");
  Serial.println(responseBody);

  // ------------------------------------------------------------------------
  // STEP 6: Parse JSON response
  // ------------------------------------------------------------------------
  // We need a larger buffer for the response (AI responses can be long)
  DynamicJsonDocument responseDoc(4096);

  // Deserialize the JSON response
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  // Check for parsing errors
  if (error) {
    Serial.print("ERROR: JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }

  // ------------------------------------------------------------------------
  // STEP 7: Extract and display the AI response
  // ------------------------------------------------------------------------
  Serial.println("\n=== AI RESPONSE ===");

  // Navigate the JSON structure: choices[0].message.content
  if (responseDoc.containsKey("choices")) {
    JsonArray choices = responseDoc["choices"];
    if (choices.size() > 0) {
      const char* content = choices[0]["message"]["content"];
      Serial.println(content);

      // Also print some metadata
      Serial.println("\n=== METADATA ===");
      const char* model = responseDoc["model"];
      Serial.print("Model: ");
      Serial.println(model);

      int promptTokens = responseDoc["usage"]["prompt_tokens"];
      int completionTokens = responseDoc["usage"]["completion_tokens"];
      int totalTokens = responseDoc["usage"]["total_tokens"];

      Serial.print("Tokens - Prompt: ");
      Serial.print(promptTokens);
      Serial.print(", Completion: ");
      Serial.print(completionTokens);
      Serial.print(", Total: ");
      Serial.println(totalTokens);
    } else {
      Serial.println("ERROR: No choices in response!");
    }
  } else if (responseDoc.containsKey("error")) {
    // Handle API error
    const char* errorMessage = responseDoc["error"]["message"];
    Serial.print("API ERROR: ");
    Serial.println(errorMessage);
  } else {
    Serial.println("ERROR: Unexpected response format!");
  }

  Serial.println("\n=== DONE ===");
}

// ============================================================================
// ARDUINO LOOP FUNCTION - Runs repeatedly (empty in this example)
// ============================================================================
void loop() {
  // Empty - all logic is in setup() for easy copy/paste testing
  // In a real application, you might want to add periodic chat requests here
}
