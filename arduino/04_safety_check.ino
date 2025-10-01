/*
 * 04_safety_check.ino - LlamaGuard Safety Check Example for ESP32/ESP8266
 *
 * WHAT THIS EXAMPLE DEMONSTRATES:
 * This Arduino sketch shows how to use LlamaGuard, a specialized AI model that
 * detects unsafe or harmful content. Think of it as a content moderator that can
 * protect your IoT application from malicious or inappropriate inputs.
 *
 * WHAT YOU'LL LEARN:
 * - How to use LlamaGuard to check content safety
 * - The 13 safety categories that LlamaGuard monitors
 * - How to implement a fail-safe content moderation system
 * - When to check user input vs AI responses
 * - Real-world use cases for IoT content moderation
 *
 * LLAMAGUARD CAN DETECT:
 * - Violence and hate speech
 * - Sexual content and exploitation
 * - Criminal planning
 * - Weapons and dangerous substances
 * - Privacy violations
 * - Self-harm content
 * - And 7 other harmful content categories (13 total)
 *
 * USE CASE: Before sending user input to a chatbot or processing responses,
 * you can use LlamaGuard to filter out unsafe content. This is especially
 * important for public-facing IoT devices or chatbots accessible to children.
 *
 * HARDWARE REQUIRED:
 * - ESP32 or ESP8266 board (these have built-in WiFi)
 *   Examples: ESP32 DevKit, NodeMCU, Wemos D1 Mini
 *
 * LIBRARIES REQUIRED:
 * - ArduinoJson by Benoit Blanchon
 * - WiFiClientSecure (built-in for ESP32/ESP8266)
 *
 * HOW TO USE:
 * 1. Update WIFI_SSID and WIFI_PASSWORD
 * 2. Update GROQ_API_KEY
 * 3. Upload to ESP32/ESP8266
 * 4. Open Serial Monitor at 115200 baud
 *
 * EXPECTED SERIAL MONITOR OUTPUT:
 * You should see:
 * - WiFi connection status
 * - Safety checks on various test inputs
 * - "SAFE" or "UNSAFE" verdicts with category codes
 * - Educational information about the 13 safety categories
 */

// Include WiFi library
#if defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
#endif

#include <ArduinoJson.h>

// ============================================================================
// CONFIGURATION
// ============================================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* GROQ_API_KEY = "YOUR_GROQ_API_KEY";

const char* GROQ_HOST = "api.groq.com";
const int HTTPS_PORT = 443;
const char* API_PATH = "/openai/v1/chat/completions";

// ============================================================================
// HELPER FUNCTION: Check content safety using LlamaGuard
// ============================================================================
// This function sends content to LlamaGuard and returns whether it's safe
// LlamaGuard is a specialized AI model trained to detect harmful content
//
// Returns: true if safe, false if unsafe
// Parameters:
//   - contentToCheck: The text content to analyze
//   - role: Either "user" or "assistant" (who said the content)
//
// FAIL-SAFE DESIGN: If the check fails for any reason (network error, timeout,
// parsing error), we return false (unsafe) to be cautious.
bool checkContentSafety(const char* contentToCheck, const char* role) {
  // Create a new HTTPS client for this safety check
  WiFiClientSecure client;

  // Skip SSL certificate validation for simplicity
  #if defined(ESP32)
    client.setInsecure();
  #elif defined(ESP8266)
    client.setInsecure();
  #endif

  Serial.println("\n--- Starting Safety Check ---");
  Serial.print("Checking content from role: ");
  Serial.println(role);
  Serial.print("Content: \"");
  Serial.print(contentToCheck);
  Serial.println("\"");

  // Establish connection to Groq API
  if (!client.connect(GROQ_HOST, HTTPS_PORT)) {
    Serial.println("ERROR: Failed to connect to Groq API for safety check");
    return false; // Fail-safe: treat as unsafe if we can't check
  }

  // -------------------------------------------------------------------------
  // Build LlamaGuard request
  // -------------------------------------------------------------------------
  // LlamaGuard uses a special format different from regular chat models:
  // - The model is "llama-guard-3-8b" (specifically trained for safety)
  // - Content is formatted with special tokens [INST] and [/INST]
  // - Response will be "safe" or "unsafe\nS<number>" with category codes

  // Allocate memory for the request JSON
  StaticJsonDocument<1024> requestDoc;

  // Use the LlamaGuard model (NOT a regular chat model!)
  requestDoc["model"] = "llama-guard-3-8b";

  // Create the messages array
  JsonArray messages = requestDoc.createNestedArray("messages");

  // Create a single user message with the formatted content to check
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";

  // Format the content according to LlamaGuard's expected format
  // LlamaGuard expects special tokens to identify the content source
  // Format: "<|begin_of_text|>[INST] <role>: <content> [/INST]"
  // The role can be "user" (input from user) or "agent" (AI response)
  String formattedContent = String("<|begin_of_text|>[INST] ") +
                            role + ": " + contentToCheck +
                            " [/INST]";
  userMessage["content"] = formattedContent;

  // LlamaGuard works best with low temperature for deterministic results
  // We want consistent safety verdicts, not creative variations
  requestDoc["temperature"] = 0.0;

  // 100 tokens is enough for the verdict ("safe" or "unsafe\nS1, S2...")
  requestDoc["max_tokens"] = 100;

  // Convert JSON to string
  String requestBody;
  serializeJson(requestDoc, requestBody);

  // Build and send HTTP POST request
  String request = String("POST ") + API_PATH + " HTTP/1.1\r\n";
  request += String("Host: ") + GROQ_HOST + "\r\n";
  request += "Content-Type: application/json\r\n";
  request += String("Authorization: Bearer ") + GROQ_API_KEY + "\r\n";
  request += String("Content-Length: ") + requestBody.length() + "\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";
  request += requestBody;

  // Send the request to the server
  client.print(request);

  // Wait for response
  int timeout = 0;
  while (!client.available() && timeout < 10000) {
    delay(100);
    timeout += 100;
  }

  if (!client.available()) {
    Serial.println("ERROR: Safety check timeout");
    client.stop();
    return false; // Fail-safe
  }

  // Skip headers
  bool headersEnded = false;
  String line = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      line += c;
      if (line.endsWith("\r\n\r\n")) {
        headersEnded = true;
        break;
      }
    }
  }

  // Read response body
  String responseBody = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      responseBody += c;
    }
  }

  client.stop();

  // Parse response
  DynamicJsonDocument responseDoc(2048);
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  if (error) {
    Serial.print("ERROR: Failed to parse safety check response: ");
    Serial.println(error.c_str());
    return false; // Fail-safe
  }

  // Extract the safety verdict
  if (responseDoc.containsKey("choices")) {
    JsonArray choices = responseDoc["choices"];
    if (choices.size() > 0) {
      const char* verdict = choices[0]["message"]["content"];
      Serial.print("LlamaGuard verdict: ");
      Serial.println(verdict);

      // LlamaGuard returns "safe" if content is safe
      // or "unsafe\nS<number>" if unsafe (e.g., "unsafe\nS1" for violence)
      String verdictStr = String(verdict);
      verdictStr.trim();
      verdictStr.toLowerCase();

      if (verdictStr.startsWith("safe")) {
        Serial.println("✓ Content is SAFE");
        return true;
      } else {
        Serial.println("✗ Content is UNSAFE");
        // Parse the unsafe category if present
        if (verdictStr.indexOf("s1") >= 0) Serial.println("  Category: Violent Crimes");
        if (verdictStr.indexOf("s2") >= 0) Serial.println("  Category: Non-Violent Crimes");
        if (verdictStr.indexOf("s3") >= 0) Serial.println("  Category: Sex Crimes");
        if (verdictStr.indexOf("s4") >= 0) Serial.println("  Category: Child Exploitation");
        if (verdictStr.indexOf("s5") >= 0) Serial.println("  Category: Defamation");
        if (verdictStr.indexOf("s6") >= 0) Serial.println("  Category: Specialized Advice");
        if (verdictStr.indexOf("s7") >= 0) Serial.println("  Category: Privacy");
        if (verdictStr.indexOf("s8") >= 0) Serial.println("  Category: Intellectual Property");
        if (verdictStr.indexOf("s9") >= 0) Serial.println("  Category: Indiscriminate Weapons");
        if (verdictStr.indexOf("s10") >= 0) Serial.println("  Category: Hate");
        if (verdictStr.indexOf("s11") >= 0) Serial.println("  Category: Self-Harm");
        if (verdictStr.indexOf("s12") >= 0) Serial.println("  Category: Sexual Content");
        if (verdictStr.indexOf("s13") >= 0) Serial.println("  Category: Elections");
        return false;
      }
    }
  }

  Serial.println("ERROR: Unexpected response format from LlamaGuard");
  return false; // Fail-safe
}

// ============================================================================
// ARDUINO SETUP FUNCTION
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== Groq API Safety Check Example ===");
  Serial.println("Using LlamaGuard to check content safety\n");

  // ------------------------------------------------------------------------
  // STEP 1: Connect to WiFi
  // ------------------------------------------------------------------------
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 30) {
    delay(1000);
    Serial.print(".");
    wifi_timeout++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nERROR: WiFi connection failed!");
    return;
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // ------------------------------------------------------------------------
  // STEP 2: Test various content samples
  // ------------------------------------------------------------------------
  Serial.println("\n=== TESTING CONTENT SAFETY ===\n");

  // Test 1: Safe content
  Serial.println("TEST 1: Safe Content");
  bool isSafe1 = checkContentSafety(
    "How do I build a simple LED circuit with Arduino?",
    "user"
  );
  Serial.print("Result: ");
  Serial.println(isSafe1 ? "SAFE" : "UNSAFE");

  delay(2000); // Brief delay between API calls

  // Test 2: Educational content (should be safe)
  Serial.println("\n\nTEST 2: Educational Content");
  bool isSafe2 = checkContentSafety(
    "Can you explain how encryption algorithms work?",
    "user"
  );
  Serial.print("Result: ");
  Serial.println(isSafe2 ? "SAFE" : "UNSAFE");

  delay(2000);

  // Test 3: Potentially problematic content
  Serial.println("\n\nTEST 3: Testing boundary case");
  bool isSafe3 = checkContentSafety(
    "I hate Mondays and bad weather",
    "user"
  );
  Serial.print("Result: ");
  Serial.println(isSafe3 ? "SAFE" : "UNSAFE");

  // ------------------------------------------------------------------------
  // STEP 3: Demonstrate practical use case
  // ------------------------------------------------------------------------
  Serial.println("\n\n=== PRACTICAL USE CASE ===");
  Serial.println("In a real IoT chatbot application:");
  Serial.println("1. Receive user input from sensor/button/web interface");
  Serial.println("2. Check input with LlamaGuard before processing");
  Serial.println("3. Only proceed if content is safe");
  Serial.println("4. Similarly, check AI responses before displaying them");

  // Example workflow
  const char* userInput = "What's the weather like today?";
  Serial.println("\nExample: User asks: \"What's the weather like today?\"");

  if (checkContentSafety(userInput, "user")) {
    Serial.println("\n✓ User input is safe, proceeding with chatbot...");
    Serial.println("(In real application, you would now send this to your main chat model)");
  } else {
    Serial.println("\n✗ User input flagged as unsafe, blocking request.");
    Serial.println("(In real application, you would show an error to the user)");
  }

  // ------------------------------------------------------------------------
  // EDUCATIONAL NOTES
  // ------------------------------------------------------------------------
  Serial.println("\n\n=== LLAMAGUARD SAFETY CATEGORIES ===");
  Serial.println("S1: Violent Crimes");
  Serial.println("S2: Non-Violent Crimes");
  Serial.println("S3: Sex Crimes");
  Serial.println("S4: Child Exploitation");
  Serial.println("S5: Defamation");
  Serial.println("S6: Specialized Advice (financial, medical, legal)");
  Serial.println("S7: Privacy violations");
  Serial.println("S8: Intellectual Property violations");
  Serial.println("S9: Indiscriminate Weapons");
  Serial.println("S10: Hate speech");
  Serial.println("S11: Self-Harm");
  Serial.println("S12: Sexual Content");
  Serial.println("S13: Elections (misinformation)");

  Serial.println("\n=== BEST PRACTICES ===");
  Serial.println("1. Check BOTH user input and AI responses");
  Serial.println("2. Use fail-safe approach: if check fails, treat as unsafe");
  Serial.println("3. Log unsafe content for review and improvement");
  Serial.println("4. Provide helpful error messages to users");
  Serial.println("5. Consider rate limiting to prevent abuse");
  Serial.println("6. Combine with other safety measures (input validation, etc.)");

  Serial.println("\n=== DONE ===");
}

// ============================================================================
// ARDUINO LOOP FUNCTION - Empty
// ============================================================================
void loop() {
  // Empty - all logic in setup()
  //
  // In a real application, you might:
  // - Read user input from Serial or sensors
  // - Check it with LlamaGuard
  // - Process safe input with your main AI model
  // - Check AI responses before displaying them
}
