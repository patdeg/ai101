/*
 * 01_basic_chat.ino - Basic Chat Example for ESP32/ESP8266
 *
 * WHAT THIS EXAMPLE DEMONSTRATES:
 * This Arduino sketch shows you how to make a basic chat request to Groq's AI API
 * using an ESP32 or ESP8266 board with WiFi capability. It's the simplest possible
 * example - just send a question and get a response back.
 *
 * WHAT YOU'LL LEARN:
 * - How to connect your Arduino board to WiFi
 * - How to make HTTPS requests to an AI API
 * - How to build JSON requests using ArduinoJson
 * - How to parse JSON responses
 * - How to extract AI-generated text from the response
 *
 * HARDWARE REQUIRED:
 * - ESP32 or ESP8266 board (these have built-in WiFi)
 *   Examples: ESP32 DevKit, NodeMCU, Wemos D1 Mini
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
 * EXPECTED SERIAL MONITOR OUTPUT:
 * You should see:
 * - WiFi connection progress
 * - The JSON request being sent
 * - The AI's response (explaining recursion)
 * - Token usage statistics
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
  // Step 0: Initialize serial communication for debugging
  // -------------------------------------------------------
  // Serial.begin() starts serial communication at 115200 baud rate
  // Baud rate = bits per second. 115200 is fast enough for debugging
  // but not so fast that it causes timing issues on Arduino
  // This allows us to print debug messages and results to Serial Monitor
  Serial.begin(115200);

  // Small delay to let serial connection stabilize
  // On some boards, the USB-to-Serial chip needs time to initialize
  delay(1000);

  Serial.println("\n\n=== Groq API Basic Chat Example ===");
  Serial.println("Starting ESP32/ESP8266...\n");

  // ------------------------------------------------------------------------
  // STEP 1: Connect to WiFi network
  // ------------------------------------------------------------------------
  // Before we can make internet requests, we need to connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  // WiFi.begin() initiates the WiFi connection process
  // It's non-blocking - the board continues running while connecting
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait for WiFi connection to complete (timeout after 30 seconds)
  // WiFi.status() returns WL_CONNECTED when successfully connected
  // We use a timeout to prevent infinite loops if WiFi is unavailable
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 30) {
    delay(1000);  // Wait 1 second between checks
    Serial.print(".");  // Print progress dots
    wifi_timeout++;
  }

  // Check if WiFi connected successfully
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nERROR: WiFi connection failed!");
    Serial.println("Please check your SSID and password.");
    return; // Exit setup() - board will continue to loop() but do nothing
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // Print the IP address assigned by the router

  // ------------------------------------------------------------------------
  // STEP 2: Create HTTPS client and connect to Groq API
  // ------------------------------------------------------------------------
  // WiFiClientSecure is like WiFiClient but with SSL/TLS encryption
  // HTTPS uses port 443 and encrypts all data for security
  WiFiClientSecure client;

  // Configure SSL certificate validation
  // For production, you should verify SSL certificates to prevent man-in-the-middle attacks
  // For simplicity in this example, we skip certificate validation
  // WARNING: This makes the connection less secure but easier for beginners
  #if defined(ESP32)
    client.setInsecure(); // ESP32: Skip certificate validation
  #elif defined(ESP8266)
    client.setInsecure(); // ESP8266: Skip certificate validation
  #endif

  Serial.println("\nConnecting to Groq API...");

  // client.connect() establishes a TCP connection to the server
  // Parameters: hostname (string) and port (443 for HTTPS)
  // Returns true if connection successful, false if it fails
  if (!client.connect(GROQ_HOST, HTTPS_PORT)) {
    Serial.println("ERROR: Connection to Groq API failed!");
    Serial.println("Check your internet connection and API host.");
    return;
  }

  Serial.println("Connected to Groq API!");

  // ------------------------------------------------------------------------
  // STEP 3: Build JSON request body using ArduinoJson library
  // ------------------------------------------------------------------------
  // ArduinoJson uses a pre-allocated buffer for JSON documents
  // StaticJsonDocument allocates memory on the stack (fast, but limited size)
  // DynamicJsonDocument allocates on heap (slower, but more flexible)
  // The number (512) is the buffer size in bytes - must be large enough for our JSON
  StaticJsonDocument<512> requestDoc;

  // Set the AI model we want to use
  // "llama-3.1-8b-instant" is fast and good for simple tasks
  requestDoc["model"] = "llama-3.1-8b-instant";

  // Create the messages array
  // Chat APIs use an array of messages with "role" and "content"
  JsonArray messages = requestDoc.createNestedArray("messages");

  // Add a user message to the conversation
  // This is what we're asking the AI
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";  // Role can be "system", "user", or "assistant"
  userMessage["content"] = "Explain the concept of recursion in programming in one sentence.";

  // Optional: Set temperature for response randomness (0.0 to 2.0)
  // Lower values = more focused and deterministic
  // Higher values = more creative and random
  requestDoc["temperature"] = 0.7;

  // Optional: Set max tokens for the response
  // Tokens are roughly words (1 token ≈ 0.75 words)
  // This prevents very long responses and saves API costs
  requestDoc["max_tokens"] = 150;

  // Serialize (convert) the JSON document to a String
  // This creates the actual JSON text that we'll send to the API
  String requestBody;
  serializeJson(requestDoc, requestBody);

  Serial.println("\nRequest Body:");
  Serial.println(requestBody);

  // ------------------------------------------------------------------------
  // STEP 4: Build and send HTTP POST request
  // ------------------------------------------------------------------------
  // HTTP requests consist of headers (metadata) and body (data)
  // We need to build the request string manually with proper formatting
  // \r\n is the HTTP line ending (carriage return + line feed)

  // Start with the request line: METHOD PATH HTTP_VERSION
  String request = String("POST ") + API_PATH + " HTTP/1.1\r\n";

  // Add required HTTP headers:
  request += String("Host: ") + GROQ_HOST + "\r\n";  // Host header (required in HTTP/1.1)
  request += "Content-Type: application/json\r\n";   // Tell server we're sending JSON
  request += String("Authorization: Bearer ") + GROQ_API_KEY + "\r\n";  // API key for authentication
  request += String("Content-Length: ") + requestBody.length() + "\r\n";  // Length of body in bytes
  request += "Connection: close\r\n";  // Close connection after response (simpler for Arduino)
  request += "\r\n";  // Empty line separates headers from body (REQUIRED!)
  request += requestBody;  // Attach the JSON body

  // Send the entire request to the server
  // client.print() sends data over the TCP connection
  client.print(request);
  Serial.println("\nHTTP request sent!");

  // ------------------------------------------------------------------------
  // STEP 5: Wait for and read the HTTP response
  // ------------------------------------------------------------------------
  Serial.println("\nWaiting for response...\n");

  // Wait for response data to arrive (timeout after 10 seconds)
  // client.available() returns the number of bytes available to read
  // The server may take a few seconds to process our request
  int timeout = 0;
  while (!client.available() && timeout < 10000) {
    delay(100);  // Check every 100ms
    timeout += 100;
  }

  if (!client.available()) {
    Serial.println("ERROR: Response timeout!");
    Serial.println("The server didn't respond in time.");
    client.stop();  // Close the connection
    return;
  }

  // Skip HTTP response headers - we only want the JSON body
  // Headers end with an empty line: "\r\n\r\n"
  // We need to read and discard them to get to the actual data
  bool headersEnded = false;
  String line = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();  // Read one character at a time
      line += c;

      // Check for end of headers (double line break)
      if (line.endsWith("\r\n\r\n")) {
        headersEnded = true;
        break;  // Stop reading - we've found the end of headers
      }
    }
  }

  if (!headersEnded) {
    Serial.println("ERROR: Failed to parse HTTP headers!");
    client.stop();
    return;
  }

  // Read the JSON response body
  // Everything after the headers is the actual response data
  String responseBody = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();  // Read one character at a time
      responseBody += c;        // Build up the response string
    }
  }

  // Close the connection - we're done reading
  client.stop();

  Serial.println("Response received!");
  Serial.println("\nRaw JSON Response:");
  Serial.println(responseBody);

  // ------------------------------------------------------------------------
  // STEP 6: Parse the JSON response using ArduinoJson
  // ------------------------------------------------------------------------
  // We need a larger buffer for the response (AI responses can be long)
  // Using DynamicJsonDocument (heap allocation) because response size varies
  // 4096 bytes should be enough for most responses
  DynamicJsonDocument responseDoc(4096);

  // Deserialize (parse) the JSON string into a document we can navigate
  // This converts the text into a structured object we can query
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  // Check for parsing errors
  // If the JSON is malformed or too large, this will fail
  if (error) {
    Serial.print("ERROR: JSON parsing failed: ");
    Serial.println(error.c_str());  // Print the error message
    return;
  }

  // ------------------------------------------------------------------------
  // STEP 7: Extract and display the AI-generated response
  // ------------------------------------------------------------------------
  Serial.println("\n=== AI RESPONSE ===");

  // Navigate the JSON structure to find the AI's message
  // Groq API response structure: {choices: [{message: {content: "..."}}]}
  // We need to drill down through this hierarchy to get the actual text
  if (responseDoc.containsKey("choices")) {
    // Get the "choices" array from the response
    JsonArray choices = responseDoc["choices"];

    if (choices.size() > 0) {
      // Extract the content from the first choice
      // Path: choices[0] → message → content
      const char* content = choices[0]["message"]["content"];
      Serial.println(content);  // Print the AI's answer!

      // Also print some useful metadata about the response
      Serial.println("\n=== METADATA ===");

      // Which model was actually used (server confirms this)
      const char* model = responseDoc["model"];
      Serial.print("Model: ");
      Serial.println(model);

      // Token usage information (important for cost tracking)
      // Tokens are the basic units AI models use - roughly 0.75 words per token
      int promptTokens = responseDoc["usage"]["prompt_tokens"];      // Tokens in our question
      int completionTokens = responseDoc["usage"]["completion_tokens"];  // Tokens in AI's answer
      int totalTokens = responseDoc["usage"]["total_tokens"];        // Total = prompt + completion

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
    // Handle API errors (invalid key, rate limits, etc.)
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
