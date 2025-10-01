/*
 * 02_system_prompt.ino - System Prompt Example for ESP32/ESP8266
 *
 * This Arduino sketch demonstrates how to use system prompts to control
 * the AI's behavior and personality. System prompts provide context and
 * instructions that the AI follows throughout the conversation.
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
 * WHAT THIS EXAMPLE SHOWS:
 * - How to add a "system" role message to guide the AI's behavior
 * - How to combine system prompts with user prompts
 * - The difference between system and user messages
 */

// Include WiFi library - different for ESP32 vs ESP8266
#if defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
#endif

#include <ArduinoJson.h>

// ============================================================================
// CONFIGURATION - UPDATE THESE VALUES
// ============================================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* GROQ_API_KEY = "YOUR_GROQ_API_KEY";

const char* GROQ_HOST = "api.groq.com";
const int HTTPS_PORT = 443;
const char* API_PATH = "/openai/v1/chat/completions";

// ============================================================================
// ARDUINO SETUP FUNCTION
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== Groq API System Prompt Example ===");
  Serial.println("Demonstrating how to use system prompts to control AI behavior\n");

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
  // STEP 2: Create HTTPS client
  // ------------------------------------------------------------------------
  WiFiClientSecure client;

  #if defined(ESP32)
    client.setInsecure();
  #elif defined(ESP8266)
    client.setInsecure();
  #endif

  Serial.println("\nConnecting to Groq API...");

  if (!client.connect(GROQ_HOST, HTTPS_PORT)) {
    Serial.println("ERROR: Connection to Groq API failed!");
    return;
  }

  Serial.println("Connected to Groq API!");

  // ------------------------------------------------------------------------
  // STEP 3: Prepare JSON request with SYSTEM and USER messages
  // ------------------------------------------------------------------------
  // The messages array in ChatGPT-style APIs can contain three types of messages:
  // 1. "system" - Sets the behavior/personality of the AI assistant
  // 2. "user" - Messages from the user
  // 3. "assistant" - Previous responses from the AI (for conversation history)
  //
  // System prompts are powerful because they:
  // - Define the AI's role and expertise
  // - Set the tone and style of responses
  // - Provide context and constraints
  // - Guide the AI's behavior throughout the conversation

  StaticJsonDocument<1024> requestDoc;

  requestDoc["model"] = "llama-3.1-8b-instant";

  // Create the messages array
  JsonArray messages = requestDoc.createNestedArray("messages");

  // -------------------------------------------------------------------------
  // SYSTEM MESSAGE: This sets the AI's role and behavior
  // -------------------------------------------------------------------------
  // The system message is like giving instructions to the AI about how it
  // should behave. Think of it as "programming" the AI's personality.
  JsonObject systemMessage = messages.createNestedObject();
  systemMessage["role"] = "system";
  systemMessage["content"] = "You are a helpful Arduino programming assistant. "
                             "You specialize in embedded systems and IoT devices. "
                             "Always explain concepts in simple terms suitable for makers "
                             "and hobbyists. Keep responses concise and practical. "
                             "When discussing code, focus on Arduino C++ specifically.";

  // -------------------------------------------------------------------------
  // USER MESSAGE: This is the actual question or prompt from the user
  // -------------------------------------------------------------------------
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";
  userMessage["content"] = "What's the difference between analogRead() and digitalRead()?";

  // Set temperature (0.0 = deterministic, 2.0 = very random)
  requestDoc["temperature"] = 0.5;

  // Set max tokens
  requestDoc["max_tokens"] = 200;

  // Serialize JSON to string
  String requestBody;
  serializeJson(requestDoc, requestBody);

  Serial.println("\nRequest Body:");
  Serial.println(requestBody);

  // ------------------------------------------------------------------------
  // STEP 4: Send HTTP POST request
  // ------------------------------------------------------------------------
  String request = String("POST ") + API_PATH + " HTTP/1.1\r\n";
  request += String("Host: ") + GROQ_HOST + "\r\n";
  request += "Content-Type: application/json\r\n";
  request += String("Authorization: Bearer ") + GROQ_API_KEY + "\r\n";
  request += String("Content-Length: ") + requestBody.length() + "\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";
  request += requestBody;

  client.print(request);
  Serial.println("\nHTTP request sent!");

  // ------------------------------------------------------------------------
  // STEP 5: Read response
  // ------------------------------------------------------------------------
  Serial.println("\nWaiting for response...\n");

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

  // Skip HTTP headers
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

  if (!headersEnded) {
    Serial.println("ERROR: Failed to parse HTTP headers!");
    client.stop();
    return;
  }

  // Read JSON body
  String responseBody = "";
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      responseBody += c;
    }
  }

  client.stop();

  Serial.println("Response received!");

  // ------------------------------------------------------------------------
  // STEP 6: Parse JSON response
  // ------------------------------------------------------------------------
  DynamicJsonDocument responseDoc(4096);
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  if (error) {
    Serial.print("ERROR: JSON parsing failed: ");
    Serial.println(error.c_str());
    return;
  }

  // ------------------------------------------------------------------------
  // STEP 7: Display results
  // ------------------------------------------------------------------------
  Serial.println("\n=== AI RESPONSE (with system prompt guidance) ===");

  if (responseDoc.containsKey("choices")) {
    JsonArray choices = responseDoc["choices"];
    if (choices.size() > 0) {
      const char* content = choices[0]["message"]["content"];
      Serial.println(content);

      Serial.println("\n=== NOTICE ===");
      Serial.println("The response above should be tailored to Arduino/embedded systems");
      Serial.println("because of our system prompt. The AI acts as an Arduino expert!");

      Serial.println("\n=== METADATA ===");
      Serial.print("Model: ");
      Serial.println(responseDoc["model"].as<const char*>());
      Serial.print("Total tokens: ");
      Serial.println(responseDoc["usage"]["total_tokens"].as<int>());
    }
  } else if (responseDoc.containsKey("error")) {
    Serial.print("API ERROR: ");
    Serial.println(responseDoc["error"]["message"].as<const char*>());
  }

  // ------------------------------------------------------------------------
  // EDUCATIONAL NOTES
  // ------------------------------------------------------------------------
  Serial.println("\n=== SYSTEM PROMPT TIPS ===");
  Serial.println("1. System prompts set the AI's role, expertise, and personality");
  Serial.println("2. They remain active throughout the entire conversation");
  Serial.println("3. Use them to control tone, style, and response format");
  Serial.println("4. Examples of good system prompts:");
  Serial.println("   - 'You are a concise technical writer'");
  Serial.println("   - 'You are a patient teacher explaining to beginners'");
  Serial.println("   - 'Always respond in JSON format'");
  Serial.println("   - 'You are an expert in IoT security'");

  Serial.println("\n=== DONE ===");
}

// ============================================================================
// ARDUINO LOOP FUNCTION - Empty
// ============================================================================
void loop() {
  // Empty - all logic in setup()
}
