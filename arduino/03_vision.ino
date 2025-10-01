/*
 * 03_vision.ino - Vision API Example with Base64 Encoding for ESP32/ESP8266
 *
 * WHAT THIS EXAMPLE DEMONSTRATES:
 * This Arduino sketch shows how to use vision-capable AI models to analyze images.
 * The AI can "see" and describe what's in an image, just like a human!
 *
 * WHAT YOU'LL LEARN:
 * - How to send images to AI vision models
 * - What base64 encoding is and why we need it
 * - How to handle Arduino's memory limitations with images
 * - The difference between regular chat models and vision models
 * - How to structure requests with both text and image content
 *
 * IMPORTANT: Vision models require images in base64 format. Due to Arduino's
 * limited RAM (ESP32 has ~520KB, ESP8266 has ~80KB), this example uses a tiny
 * 1x1 pixel red PNG image. In real applications, you would:
 * 1. Store larger images on SPIFFS/SD card
 * 2. Read and encode them in chunks
 * 3. Use external memory if needed
 * 4. Consider using ESP32-CAM for built-in camera support
 *
 * HARDWARE REQUIRED:
 * - ESP32 or ESP8266 board (these have built-in WiFi)
 *   Examples: ESP32 DevKit, NodeMCU, Wemos D1 Mini
 *   For camera projects: ESP32-CAM
 *
 * LIBRARIES REQUIRED (install via Arduino Library Manager):
 * - ArduinoJson by Benoit Blanchon
 * - WiFiClientSecure (built-in for ESP32/ESP8266)
 * - base64 by Arturo Guadalupi (optional, for base64 encoding from SPIFFS)
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
 * - Free heap memory (to monitor RAM usage)
 * - The AI's description of the image (should identify it as red/colored)
 * - Tips for working with images on Arduino
 *
 * NOTE: Groq currently supports vision models like llama-3.2-11b-vision-preview
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
// DEMO IMAGE: 1x1 pixel red PNG encoded as base64
// ============================================================================
// This is a tiny 1x1 pixel red PNG image (67 bytes) encoded in base64
// Format: data:image/png;base64,<base64_data>
//
// To create your own base64 images:
// 1. On Linux/Mac: base64 -i yourimage.png
// 2. Online: use https://base64.guru/converter/encode/image
// 3. Python: base64.b64encode(open('image.png', 'rb').read()).decode()
//
// REMEMBER: Keep images small! Arduino has limited RAM.
// ESP32: ~520KB total, ESP8266: ~80KB total
const char* DEMO_IMAGE_BASE64 =
  "data:image/png;base64,"
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8DwHwAFBQIAX8jx0gAAAABJRU5ErkJggg==";

// ============================================================================
// ARDUINO SETUP FUNCTION - Runs once when board starts
// ============================================================================
void setup() {
  // Step 0: Initialize serial communication for debugging
  // -------------------------------------------------------
  // Start serial at 115200 baud for viewing output in Serial Monitor
  Serial.begin(115200);
  // Brief delay to allow serial connection to stabilize
  delay(1000);

  Serial.println("\n\n=== Groq API Vision Example ===");
  Serial.println("Analyzing an image using vision models\n");

  // Print memory info (ESP32 only)
  // This helps us monitor RAM usage, which is critical when working with images
  // Images in base64 format can be large and consume significant memory
  #if defined(ESP32)
    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
  #endif

  // ------------------------------------------------------------------------
  // STEP 1: Connect to WiFi network
  // ------------------------------------------------------------------------
  // WiFi must be connected before we can send requests to the vision API
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
  // STEP 3: Build JSON request with IMAGE content
  // ------------------------------------------------------------------------
  // Vision models accept messages with content as an array containing:
  // 1. Text parts: {"type": "text", "text": "..."}
  // 2. Image parts: {"type": "image_url", "image_url": {"url": "data:..."}}
  //
  // The image URL can be:
  // - A base64 data URI: "data:image/png;base64,iVBORw0KG..."
  // - An HTTP(S) URL: "https://example.com/image.jpg"
  //
  // We use base64 because it embeds the entire image in the JSON request

  // We need a larger document for vision requests (images are big in base64)
  // DynamicJsonDocument allocates on heap - better for large, variable-size data
  // 8192 bytes (8KB) should handle our small demo image plus overhead
  DynamicJsonDocument requestDoc(8192);

  // Use a vision-capable model (NOT all models support vision!)
  // Regular chat models will fail if you send them images
  requestDoc["model"] = "llama-3.2-11b-vision-preview";

  // Create the messages array
  JsonArray messages = requestDoc.createNestedArray("messages");

  // Create user message with mixed content (text + image)
  // Unlike regular chat, the content field is an ARRAY of parts
  JsonObject userMessage = messages.createNestedObject();
  userMessage["role"] = "user";

  // Content is an array of content parts (can mix text and images)
  JsonArray content = userMessage.createNestedArray("content");

  // Part 1: Text prompt - our question about the image
  // This tells the AI what we want to know about the image
  JsonObject textPart = content.createNestedObject();
  textPart["type"] = "text";
  textPart["text"] = "What color is this image? Describe what you see in detail.";

  // Part 2: Image data in base64 format
  // The AI will analyze this image to answer our question
  JsonObject imagePart = content.createNestedObject();
  imagePart["type"] = "image_url";

  // Nest the actual image URL/data inside an "image_url" object
  JsonObject imageUrl = imagePart.createNestedObject("image_url");
  imageUrl["url"] = DEMO_IMAGE_BASE64;  // Our base64-encoded 1x1 red pixel

  // Optional parameters
  requestDoc["temperature"] = 0.5;  // Lower = more deterministic descriptions
  requestDoc["max_tokens"] = 300;   // Vision models may need more tokens for descriptions

  // Convert the JSON document to a string for transmission
  String requestBody;
  serializeJson(requestDoc, requestBody);

  Serial.println("\nRequest prepared!");
  Serial.print("Request size: ");
  Serial.print(requestBody.length());
  Serial.println(" bytes");

  // Print first 500 chars of request for debugging
  Serial.println("\nRequest Body (first 500 chars):");
  if (requestBody.length() > 500) {
    Serial.println(requestBody.substring(0, 500) + "...");
  } else {
    Serial.println(requestBody);
  }

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
  Serial.println("\nWaiting for response (this may take longer for vision)...\n");

  // Vision requests may take longer - increase timeout to 30 seconds
  int timeout = 0;
  while (!client.available() && timeout < 30000) {
    delay(100);
    timeout += 100;
    if (timeout % 1000 == 0) {
      Serial.print(".");
    }
  }
  Serial.println();

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
  DynamicJsonDocument responseDoc(8192);
  DeserializationError error = deserializeJson(responseDoc, responseBody);

  if (error) {
    Serial.print("ERROR: JSON parsing failed: ");
    Serial.println(error.c_str());
    Serial.println("\nRaw response:");
    Serial.println(responseBody);
    return;
  }

  // ------------------------------------------------------------------------
  // STEP 7: Display results
  // ------------------------------------------------------------------------
  Serial.println("\n=== VISION MODEL RESPONSE ===");

  if (responseDoc.containsKey("choices")) {
    JsonArray choices = responseDoc["choices"];
    if (choices.size() > 0) {
      const char* content = choices[0]["message"]["content"];
      Serial.println(content);

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
  Serial.println("\n=== VISION API TIPS FOR ARDUINO ===");
  Serial.println("1. MEMORY: Images in base64 are ~33% larger than original");
  Serial.println("   - ESP32: Can handle images up to ~300KB (before base64)");
  Serial.println("   - ESP8266: Limited to very small images (~50KB max)");
  Serial.println("");
  Serial.println("2. STORAGE OPTIONS:");
  Serial.println("   - Embed small images as const char* (like this example)");
  Serial.println("   - Store on SPIFFS/LittleFS for ESP32/ESP8266");
  Serial.println("   - Use SD card for larger images");
  Serial.println("   - Stream from external camera (e.g., ESP32-CAM)");
  Serial.println("");
  Serial.println("3. BASE64 ENCODING:");
  Serial.println("   - Use 'base64' library: base64::encode()");
  Serial.println("   - Or encode offline and embed the string");
  Serial.println("");
  Serial.println("4. IMAGE FORMATS:");
  Serial.println("   - PNG, JPEG, GIF, WebP are supported");
  Serial.println("   - JPEG usually smallest for photos");
  Serial.println("   - PNG better for screenshots/diagrams");
  Serial.println("");
  Serial.println("5. REAL-WORLD EXAMPLE:");
  Serial.println("   - ESP32-CAM captures image → saves to SPIFFS");
  Serial.println("   - Read file in chunks, base64 encode");
  Serial.println("   - Send to vision API for analysis");

  Serial.println("\n=== DONE ===");
}

// ============================================================================
// ARDUINO LOOP FUNCTION - Empty
// ============================================================================
void loop() {
  // Empty - all logic in setup()
}

/*
 * ADVANCED: Reading and encoding images from SPIFFS
 *
 * If you want to read images from SPIFFS (ESP32/ESP8266 file system):
 *
 * 1. Include SPIFFS library:
 *    #include <SPIFFS.h>  // ESP32
 *    #include <LittleFS.h>  // ESP8266 (newer)
 *
 * 2. Initialize SPIFFS in setup():
 *    if (!SPIFFS.begin(true)) {
 *      Serial.println("SPIFFS mount failed");
 *      return;
 *    }
 *
 * 3. Read and encode image:
 *    File file = SPIFFS.open("/image.jpg", "r");
 *    if (!file) {
 *      Serial.println("Failed to open file");
 *      return;
 *    }
 *
 *    size_t fileSize = file.size();
 *    uint8_t* buffer = (uint8_t*)malloc(fileSize);
 *    file.read(buffer, fileSize);
 *    file.close();
 *
 *    // Encode to base64 using base64 library
 *    String base64Str = base64::encode(buffer, fileSize);
 *    free(buffer);
 *
 *    // Create data URI
 *    String imageDataUri = "data:image/jpeg;base64," + base64Str;
 *
 * 4. Upload images to SPIFFS using Arduino IDE:
 *    Tools → ESP32/ESP8266 Sketch Data Upload
 */
