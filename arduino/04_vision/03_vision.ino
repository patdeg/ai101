/*
 * ============================================================================
 * Example 3: AI Vision - What's in This Image?
 * For Seeed XIAO ESP32-S3 Sense
 * ============================================================================
 *
 * WHAT THIS PROJECT DOES:
 *   - Press the BOOT button to capture a photo
 *   - Built-in LED flashes during capture
 *   - Sends image to Groq Vision API (meta-llama/llama-4-scout-17b-16e-instruct)
 *   - AI analyzes and describes what's in the image
 *   - Displays description in Serial Monitor
 *
 * WHAT YOU'LL LEARN:
 *   - How to use the OV2640 camera module
 *   - Capturing JPEG images with ESP32
 *   - Base64 encoding for image upload
 *   - Working with vision-language models (VLMs)
 *   - Multimodal AI (combining images + text prompts)
 *
 * HARDWARE REQUIRED:
 *   - Seeed Studio XIAO ESP32-S3 Sense with camera attached
 *     https://www.amazon.com/dp/B0C69FFVHH
 *   - USB-C cable
 *   - That's it! No external components needed!
 *
 * BOARD FEATURES USED:
 *   - OV2640 Camera (1600x1200 max resolution)
 *   - Built-in LED (GPIO 21) - camera flash
 *   - Built-in BOOT Button (GPIO 0) - trigger capture
 *   - WiFi (2.4GHz)
 *   - 8MB PSRAM for image buffer
 *
 * SETUP INSTRUCTIONS:
 *   1. Make sure camera module is ATTACHED to the board
 *   2. Install "esp32" board support (v3.0.0+) in Arduino IDE
 *   3. Copy secrets_template.h to secrets.h
 *   4. Fill in your WiFi credentials and Demeterics Managed LLM Key
 *   5. Select Board: "XIAO_ESP32S3"
 *   6. Set PSRAM: "OPI PSRAM"
 *   7. Upload and open Serial Monitor at 115200 baud
 *
 * HOW TO USE:
 *   1. Point camera at something interesting
 *   2. Press BOOT button
 *   3. LED flashes (photo taken!)
 *   4. Wait ~2-5 seconds
 *   5. AI description appears in Serial Monitor
 *
 * COST ESTIMATE:
 *   - Each image analysis: ~$0.0001 (1/10th of a cent!)
 *   - 10,000 images for $1.00
 *
 * MODEL INFO:
 *   - meta-llama/llama-4-scout-17b-16e-instruct
 *   - Vision + chat model (sees images + understands text)
 *   - 17 billion parameters
 *   - Can describe objects, scenes, read text, count items
 *   - 131K token context (handles very detailed descriptions)
 *
 * ============================================================================
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"
#include "base64.h"
#include "secrets.h"

// ============================================================================
// CAMERA PINS - XIAO ESP32-S3 Sense Specific
// ============================================================================

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     10
#define SIOD_GPIO_NUM     40
#define SIOC_GPIO_NUM     39

#define Y9_GPIO_NUM       48
#define Y8_GPIO_NUM       11
#define Y7_GPIO_NUM       12
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       16
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM       17
#define Y2_GPIO_NUM       15
#define VSYNC_GPIO_NUM    38
#define HREF_GPIO_NUM     47
#define PCLK_GPIO_NUM     13

// ============================================================================
// CONFIGURATION
// ============================================================================

#define BUTTON_PIN    0     // BOOT button
#define LED_PIN       21    // Built-in LED (use as camera flash)

// Image quality settings
#define IMAGE_WIDTH   640   // 640x480 = good balance of quality vs size
#define IMAGE_HEIGHT  480
#define JPEG_QUALITY  10    // 0-63, lower = better quality

WiFiClientSecure client;
bool captureRequested = false;

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n");
  Serial.println("============================================================");
  Serial.println("XIAO ESP32-S3 Sense - AI Vision Camera");
  Serial.println("============================================================");
  Serial.println("Model: meta-llama/llama-4-scout-17b-16e-instruct");
  Serial.println("Camera: OV2640 (1600x1200)");
  Serial.println("============================================================");
  Serial.println();

  // Initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize camera
  if (!initCamera()) {
    Serial.println("ERROR: Camera initialization failed!");
    Serial.println("Check that camera module is properly attached");
    while(1) { delay(1000); }
  }

  // Connect to WiFi
  connectWiFi();

  Serial.println();
  Serial.println("============================================================");
  Serial.println("READY! Point camera at something and press BOOT button");
  Serial.println("============================================================");
  Serial.println();
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
  // Check for button press
  if (digitalRead(BUTTON_PIN) == LOW && !captureRequested) {
    delay(50);  // Debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      captureRequested = true;
      captureAndAnalyze();
      captureRequested = false;
    }
  }

  delay(100);
}

// ============================================================================
// CAMERA INITIALIZATION
// ============================================================================

bool initCamera() {
  Serial.println("Initializing OV2640 camera...");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Use PSRAM for frame buffer
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;  // 640x480
    config.jpeg_quality = JPEG_QUALITY;
    config.fb_count = 2;  // Double buffering
  } else {
    Serial.println("WARNING: PSRAM not found, using smaller frame");
    config.frame_size = FRAMESIZE_QVGA;  // 320x240
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }

  // Get sensor settings
  sensor_t *s = esp_camera_sensor_get();
  if (s != NULL) {
    // Adjust settings for better image quality
    s->set_brightness(s, 0);     // -2 to 2
    s->set_contrast(s, 0);       // -2 to 2
    s->set_saturation(s, 0);     // -2 to 2
    s->set_whitebal(s, 1);       // Enable auto white balance
    s->set_awb_gain(s, 1);       // Enable auto white balance gain
    s->set_wb_mode(s, 0);        // Auto white balance mode
    s->set_exposure_ctrl(s, 1);  // Enable auto exposure
    s->set_aec2(s, 0);           // Auto exposure algorithm
    s->set_gain_ctrl(s, 1);      // Enable auto gain
    s->set_agc_gain(s, 0);       // Auto gain value
    s->set_gainceiling(s, (gainceiling_t)0);  // Gain ceiling
    s->set_bpc(s, 0);            // Black pixel correction
    s->set_wpc(s, 1);            // White pixel correction
    s->set_raw_gma(s, 1);        // Gamma correction
    s->set_lenc(s, 1);           // Lens correction
    s->set_hmirror(s, 0);        // Horizontal mirror
    s->set_vflip(s, 0);          // Vertical flip
    s->set_dcw(s, 1);            // Downsize enable
    s->set_colorbar(s, 0);       // Disable test pattern
  }

  Serial.println("✓ Camera initialized");
  return true;
}

// ============================================================================
// WIFI CONNECTION
// ============================================================================

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("✓ Connected! IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("ERROR: WiFi connection failed!");
    while(1) { delay(1000); }
  }

  client.setInsecure();  // Skip certificate validation (for demo only!)
}

// ============================================================================
// CAPTURE AND ANALYZE
// ============================================================================

void captureAndAnalyze() {
  Serial.println();
  Serial.println("📸 Capturing image...");

  // Flash LED during capture
  digitalWrite(LED_PIN, HIGH);
  delay(100);

  // Capture image
  camera_fb_t *fb = esp_camera_fb_get();
  digitalWrite(LED_PIN, LOW);

  if (!fb) {
    Serial.println("ERROR: Camera capture failed!");
    return;
  }

  Serial.print("✓ Image captured! Size: ");
  Serial.print(fb->len / 1024);
  Serial.println(" KB");
  Serial.print("Resolution: ");
  Serial.print(fb->width);
  Serial.print("x");
  Serial.println(fb->height);
  Serial.println();

  // Convert to base64
  Serial.println("Converting to base64...");
  String base64Image = base64::encode(fb->buf, fb->len);

  // Return frame buffer
  esp_camera_fb_return(fb);

  Serial.println("✓ Base64 encoding complete");
  Serial.println();

  // Send to API
  sendToVisionAPI(base64Image);
}

// ============================================================================
// VISION API
// ============================================================================

void sendToVisionAPI(String base64Image) {
  Serial.println("📤 Sending to Vision API...");

  if (!client.connect("api.demeterics.com", 443)) {
    Serial.println("ERROR: Connection failed!");
    return;
  }

  // Build JSON payload
  String payload = "{";
  payload += "\"model\":\"meta-llama/llama-4-scout-17b-16e-instruct\",";
  payload += "\"messages\":[{";
  payload += "\"role\":\"user\",";
  payload += "\"content\":[";
  payload += "{\"type\":\"text\",\"text\":\"What's in this image? Describe what you see in detail.\"},";
  payload += "{\"type\":\"image_url\",\"image_url\":{\"url\":\"data:image/jpeg;base64,";
  payload += base64Image;
  payload += "\"}}";
  payload += "]}],";
  payload += "\"max_tokens\":300";
  payload += "}";

  // Send HTTP request
  client.println("POST /groq/v1/chat/completions HTTP/1.1");
  client.println("Host: api.demeterics.com");
  client.print("Authorization: Bearer ");
  client.println(DEMETERICS_API_KEY);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(payload.length());
  client.println("Connection: close");
  client.println();
  client.print(payload);

  Serial.println("✓ Request sent, waiting for response...");
  Serial.println();

  // Read response
  bool headersEnded = false;
  String responseBody = "";

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 30000) {
    if (client.available()) {
      String line = client.readStringUntil('\n');

      if (!headersEnded) {
        if (line == "\r" || line.length() == 0) {
          headersEnded = true;
        }
      } else {
        responseBody += line;
      }
    }
  }

  client.stop();

  // Parse response
  if (responseBody.length() > 0) {
    int contentStart = responseBody.indexOf("\"content\":\"");
    if (contentStart > 0) {
      contentStart += 11;  // Length of "\"content\":\""
      int contentEnd = responseBody.indexOf("\"", contentStart);

      if (contentEnd > contentStart) {
        String description = responseBody.substring(contentStart, contentEnd);

        // Unescape newlines
        description.replace("\\n", "\n");

        Serial.println("============================================================");
        Serial.println("AI VISION ANALYSIS:");
        Serial.println("============================================================");
        Serial.println(description);
        Serial.println("============================================================");
        Serial.println();
      } else {
        Serial.println("Could not parse response");
        Serial.println(responseBody);
      }
    } else {
      Serial.println("No content in response");
      Serial.println(responseBody.substring(0, 500));  // First 500 chars
    }
  } else {
    Serial.println("Empty response from API");
  }

  Serial.println("Ready for next capture - press BOOT button");
  Serial.println();
}

// ============================================================================
// EDUCATIONAL NOTES
// ============================================================================

/*
 * HOW VISION-LANGUAGE MODELS WORK
 * ================================
 *
 * Traditional AI:
 *   - Text models (GPT) only understand words
 *   - Image models (CNN) only understand pixels
 *   - Separate systems, can't communicate
 *
 * Vision-Language Models (VLMs):
 *   - Understand BOTH images and text
 *   - Can answer questions about what they see
 *   - Bridge the gap between visual and linguistic AI
 *
 * How LLaVA works:
 *   1. Image encoder (CLIP) → converts image to "visual tokens"
 *   2. Language model (Llama) → processes visual tokens + text
 *   3. Output → natural language description
 *
 * Think of it like:
 *   - You show someone a photo
 *   - They look at it (vision)
 *   - They describe what they see (language)
 *   - LLaVA does both!
 *
 * WHAT CAN VISION MODELS DO?
 * ===========================
 *
 * Object Detection:
 *   "I see a red apple on a wooden table"
 *
 * Scene Understanding:
 *   "This is a sunny beach with palm trees and blue ocean"
 *
 * Optical Character Recognition (OCR):
 *   "The sign says 'STOP'"
 *
 * Counting:
 *   "There are 3 people in this photo"
 *
 * Spatial Reasoning:
 *   "The cat is sitting on top of the laptop"
 *
 * Activity Recognition:
 *   "Two people are playing basketball"
 *
 * WHY BASE64 ENCODING?
 * ====================
 *
 * Raw JPEG → binary data (not text)
 * Base64 → converts binary to text characters
 *
 * Example:
 *   Binary: 11010110 10101001 (not transmittable in JSON)
 *   Base64: "1qmi" (safely transmittable as text)
 *
 * Process:
 *   1. Camera captures JPEG (10-50 KB)
 *   2. JPEG bytes → base64 string (13-66 KB, 33% larger)
 *   3. Embed in JSON as "data:image/jpeg;base64,..."
 *   4. API receives, decodes, processes image
 *
 * MEMORY CONSIDERATIONS
 * =====================
 *
 * 640x480 JPEG (~30 KB) → Base64 (~40 KB)
 * Total RAM needed:
 *   - Camera frame buffer: 30 KB
 *   - Base64 string: 40 KB
 *   - JSON payload: 41 KB
 *   - WiFi buffers: 8 KB
 *   - Total: ~120 KB
 *
 * XIAO has 8MB PSRAM, so no problem!
 *
 * Without PSRAM: Would need to use 320x240 resolution
 *
 * IMAGE QUALITY SETTINGS
 * ======================
 *
 * Resolution options (FRAMESIZE):
 *   - QVGA:  320x240  (9 KB)   ← Low quality
 *   - VGA:   640x480  (30 KB)  ← Good balance ✓
 *   - SVGA:  800x600  (50 KB)  ← High quality
 *   - XGA:   1024x768 (80 KB)  ← Very high
 *   - UXGA:  1600x1200 (200KB) ← Maximum (OV2640 limit)
 *
 * JPEG Quality (0-63):
 *   - 0-10:  Best quality, larger files
 *   - 10-20: Good quality ✓ (we use 10)
 *   - 20-40: Medium quality
 *   - 40-63: Low quality, tiny files
 *
 * For AI vision: 640x480 @ quality 10 is perfect!
 *
 * COST COMPARISON
 * ===============
 *
 * Groq LLaVA:
 *   - $0.0001 per image (~1/10th of a cent)
 *   - 10,000 images = $1
 *
 * OpenAI GPT-4 Vision:
 *   - $0.01 per image (1 cent)
 *   - 100 images = $1
 *   - 100x more expensive!
 *
 * Google Cloud Vision:
 *   - $1.50 per 1000 images
 *   - $0.0015 per image
 *   - 15x more expensive!
 *
 * AWS Rekognition:
 *   - $1.00 per 1000 images
 *   - $0.001 per image
 *   - 10x more expensive!
 *
 * Groq wins again! 🏆
 *
 * PROJECT IDEAS
 * =============
 *
 * Beginner:
 *   - Security camera (detect if someone's at door)
 *   - Plant identifier (point at plant, get species name)
 *   - Food calorie estimator
 *   - Color detector for accessibility
 *
 * Intermediate:
 *   - Smart fridge (see what groceries you have)
 *   - Parking spot detector
 *   - Reading assistant (OCR for books/signs)
 *   - Wildlife camera trap identifier
 *
 * Advanced:
 *   - Quality control inspector (detect defects)
 *   - Emotion detector (analyze facial expressions)
 *   - Scene translator (image → description → translation)
 *   - Autonomous navigation (describe obstacles)
 *
 * TIPS FOR BETTER RESULTS
 * ========================
 *
 * Lighting:
 *   ✓ Bright, even lighting works best
 *   ✗ Avoid backlit subjects (dark silhouettes)
 *   ✗ Harsh shadows can confuse the model
 *
 * Focus:
 *   ✓ Hold camera steady (OV2640 has fixed focus)
 *   ✓ Subject should be 10cm - 2m away
 *   ✗ Too close = blurry
 *
 * Framing:
 *   ✓ Put subject in center of frame
 *   ✓ Fill frame with subject (don't zoom out too much)
 *   ✓ Keep background simple
 *
 * Prompts:
 *   ✓ "What's in this image?" - General description
 *   ✓ "Read the text in this image" - OCR
 *   ✓ "How many [objects] are there?" - Counting
 *   ✓ "Is there a [object] in this image?" - Detection
 */
