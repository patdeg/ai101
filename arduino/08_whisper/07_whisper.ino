/*
 * ============================================================================
 * Example 7: Voice Command Recorder with Whisper Transcription
 * For Seeed XIAO ESP32-S3 Sense
 * ============================================================================
 *
 * WHAT THIS PROJECT DOES:
 *   - Press the onboard BOOT button to record 5 seconds of audio
 *   - Built-in LED blinks while recording
 *   - Sends audio to Groq Whisper API for transcription
 *   - Displays the transcribed command on Serial Monitor
 *
 * WHAT YOU'LL LEARN:
 *   - How to use the built-in I2S digital microphone (PDM)
 *   - Working with WiFi on compact ESP32 boards
 *   - Making HTTPS API calls from microcontrollers
 *   - Creating WAV files in memory
 *   - Handling multipart form data uploads
 *   - Digital signal processing basics (PDM to PCM conversion)
 *
 * HARDWARE REQUIRED:
 *   - Seeed Studio XIAO ESP32-S3 Sense (this is the ONLY board you need!)
 *     https://www.amazon.com/dp/B0C69FFVHH
 *   - USB-C cable for programming and power
 *   - That's it! No breadboard, no wiring, no external components!
 *
 * BOARD FEATURES USED:
 *   - Built-in PDM Digital Microphone
 *   - Built-in LED (GPIO 21)
 *   - Built-in BOOT Button (GPIO 0)
 *   - WiFi (2.4GHz)
 *   - 8MB PSRAM for audio buffer
 *
 * SETUP INSTRUCTIONS:
 *   1. Install "esp32" board support (v3.0.0+) in Arduino IDE
 *   2. Install "ESP32_I2S_PDM_Mic" library from Library Manager
 *   3. Copy secrets_template.h to secrets.h
 *   4. Fill in your WiFi credentials and Demeterics Managed LLM Key
 *   5. Select Board: "XIAO_ESP32S3"
 *   6. Set PSRAM: "OPI PSRAM"
 *   7. Upload and open Serial Monitor at 115200 baud
 *
 * HOW TO USE:
 *   1. Press and hold the BOOT button (next to USB port)
 *   2. Speak your command (LED blinks while recording)
 *   3. Release button (or wait 5 seconds)
 *   4. Watch Serial Monitor for transcription
 *
 * COST ESTIMATE:
 *   - 5 seconds of audio = ~$0.000056 per command
 *   - That's 17,857 commands for $1.00!
 *
 * WHY THIS BOARD IS AMAZING:
 *   - Tiny (21x17.5mm) - smaller than a thumb!
 *   - Everything built-in (mic, camera, LED, button)
 *   - 8MB PSRAM + 8MB Flash
 *   - Only $23.99 on Amazon
 *   - Perfect for wearable/portable projects
 *
 * ============================================================================
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <driver/i2s.h>
#include "secrets.h"  // Contains WIFI_SSID, WIFI_PASSWORD, DEMETERICS_API_KEY

// ============================================================================
// CONFIGURATION - Pin Definitions for XIAO ESP32-S3 Sense
// ============================================================================

// Built-in hardware pins
#define BUTTON_PIN    0     // BOOT button (built-in, press = LOW)
#define LED_PIN       21    // Built-in LED on XIAO ESP32-S3 Sense
#define I2S_PORT      I2S_NUM_0

// PDM Microphone pins (built-in on XIAO ESP32-S3 Sense)
#define I2S_WS        42    // PDM Clock
#define I2S_SD        41    // PDM Data

// ============================================================================
// CONFIGURATION - Audio Parameters
// ============================================================================

#define SAMPLE_RATE   16000  // 16kHz - perfect for voice commands
#define RECORD_TIME   5      // Record for 5 seconds
#define I2S_READ_LEN  (2 * 1024)  // Buffer size for I2S reads
#define BITS_PER_SAMPLE 16
#define CHANNELS      1      // Mono

// Calculate total audio buffer size
const int AUDIO_BUFFER_SIZE = SAMPLE_RATE * RECORD_TIME * sizeof(int16_t);

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

int16_t* audioBuffer = nullptr;  // Will hold our recorded audio
int audioBufferIndex = 0;
bool isRecording = false;
unsigned long recordStartTime = 0;

WiFiClientSecure client;

// ============================================================================
// WAV FILE HEADER STRUCTURE
// ============================================================================
// A WAV file needs a 44-byte header that tells software how to play the audio

struct WavHeader {
  // RIFF Chunk
  char riff[4] = {'R', 'I', 'F', 'F'};
  uint32_t fileSize;
  char wave[4] = {'W', 'A', 'V', 'E'};

  // Format Chunk
  char fmt[4] = {'f', 'm', 't', ' '};
  uint32_t fmtSize = 16;
  uint16_t audioFormat = 1;  // PCM
  uint16_t numChannels = CHANNELS;
  uint32_t sampleRate = SAMPLE_RATE;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample = BITS_PER_SAMPLE;

  // Data Chunk
  char data[4] = {'d', 'a', 't', 'a'};
  uint32_t dataSize;
};

// ============================================================================
// SETUP FUNCTION - Runs once when ESP32 starts
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n");
  Serial.println("============================================================");
  Serial.println("XIAO ESP32-S3 Sense - Voice Command Recorder");
  Serial.println("============================================================");
  Serial.println("Board: Seeed Studio XIAO ESP32-S3 Sense");
  Serial.println("Microphone: Built-in PDM Digital Mic");
  Serial.println("Size: 21 x 17.5mm (Thumb-sized!)");
  Serial.println("============================================================");
  Serial.println();

  // Initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // BOOT button (pressed = LOW)
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Allocate memory for audio buffer in PSRAM
  Serial.print("Allocating audio buffer (");
  Serial.print(AUDIO_BUFFER_SIZE / 1024);
  Serial.println(" KB) in PSRAM...");

  audioBuffer = (int16_t*)ps_malloc(AUDIO_BUFFER_SIZE);
  if (!audioBuffer) {
    Serial.println("ERROR: Failed to allocate audio buffer!");
    Serial.println("Make sure PSRAM is enabled in Tools > PSRAM > OPI PSRAM");
    while(1) { delay(1000); }
  }
  Serial.println("✓ Audio buffer allocated in PSRAM");

  // Setup I2S for built-in PDM microphone
  setupI2S();

  // Connect to WiFi
  connectWiFi();

  Serial.println();
  Serial.println("============================================================");
  Serial.println("READY! Press the BOOT button to record a voice command");
  Serial.println("(The BOOT button is next to the USB-C port)");
  Serial.println("============================================================");
  Serial.println();
}

// ============================================================================
// MAIN LOOP - Runs continuously
// ============================================================================

void loop() {
  // Check if button is pressed (LOW when pressed due to INPUT_PULLUP)
  if (digitalRead(BUTTON_PIN) == LOW && !isRecording) {
    // Small debounce delay
    delay(50);
    if (digitalRead(BUTTON_PIN) == LOW) {
      startRecording();
    }
  }

  // If we're recording, keep capturing audio
  if (isRecording) {
    captureAudio();

    // Blink LED while recording
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 200) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      lastBlink = millis();
    }

    // Check if recording time is up or button released
    if (millis() - recordStartTime >= RECORD_TIME * 1000 ||
        digitalRead(BUTTON_PIN) == HIGH) {
      stopRecording();
    }
  }

  delay(10);  // Small delay to prevent CPU hogging
}

// ============================================================================
// I2S PDM MICROPHONE SETUP
// ============================================================================

void setupI2S() {
  Serial.println("Configuring built-in PDM microphone...");

  // Configuration for PDM microphone
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,
    .ws_io_num = I2S_WS,      // PDM Clock
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD     // PDM Data
  };

  // Install and start I2S driver
  esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("ERROR: I2S driver install failed: %d\n", err);
    Serial.println("Make sure you selected 'XIAO_ESP32S3' as your board");
    while(1) { delay(1000); }
  }

  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("ERROR: I2S pin config failed: %d\n", err);
    while(1) { delay(1000); }
  }

  // Set clock for PDM mode
  i2s_set_clk(I2S_PORT, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  Serial.println("✓ PDM microphone configured");
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
    Serial.println("Check your credentials in secrets.h");
    Serial.println("Note: XIAO ESP32-S3 only supports 2.4GHz WiFi");
    while(1) { delay(1000); }
  }

  // Configure SSL client to skip certificate validation
  // NOTE: For production, you should validate certificates!
  client.setInsecure();
}

// ============================================================================
// RECORDING FUNCTIONS
// ============================================================================

void startRecording() {
  Serial.println();
  Serial.println("🎙️  RECORDING - Speak your command now!");
  Serial.println("------------------------------------------------------------");

  isRecording = true;
  audioBufferIndex = 0;
  recordStartTime = millis();
  digitalWrite(LED_PIN, HIGH);

  // Clear the I2S buffer to remove any stale data
  i2s_zero_dma_buffer(I2S_PORT);
}

void captureAudio() {
  // Read audio data from I2S microphone
  size_t bytesRead = 0;
  int16_t i2sBuffer[I2S_READ_LEN / 2];

  esp_err_t result = i2s_read(I2S_PORT, &i2sBuffer, I2S_READ_LEN, &bytesRead, portMAX_DELAY);

  if (result == ESP_OK && bytesRead > 0) {
    int samplesRead = bytesRead / 2;

    // Copy samples to audio buffer
    for (int i = 0; i < samplesRead && audioBufferIndex < AUDIO_BUFFER_SIZE / 2; i++) {
      audioBuffer[audioBufferIndex++] = i2sBuffer[i];
    }

    // Show recording progress
    if (audioBufferIndex % (SAMPLE_RATE / 2) == 0) {
      int secondsRecorded = audioBufferIndex / SAMPLE_RATE;
      Serial.print("Recording: ");
      Serial.print(secondsRecorded);
      Serial.print("/");
      Serial.print(RECORD_TIME);
      Serial.println(" seconds");
    }
  }
}

void stopRecording() {
  isRecording = false;
  digitalWrite(LED_PIN, LOW);

  Serial.println("------------------------------------------------------------");
  Serial.println("✓ Recording complete!");
  Serial.print("Captured ");
  Serial.print(audioBufferIndex);
  Serial.print(" samples (");
  Serial.print((float)audioBufferIndex / SAMPLE_RATE, 1);
  Serial.println(" seconds)");
  Serial.println();

  // Send to Whisper for transcription
  sendToWhisper();
}

// ============================================================================
// WHISPER API FUNCTIONS
// ============================================================================

void sendToWhisper() {
  Serial.println("📤 Sending audio to Whisper API...");

  // Connect to Demeterics Groq proxy
  if (!client.connect("api.demeterics.com", 443)) {
    Serial.println("ERROR: Connection to api.demeterics.com failed!");
    return;
  }

  // Create WAV header
  WavHeader header;
  header.dataSize = audioBufferIndex * sizeof(int16_t);
  header.fileSize = header.dataSize + 36;
  header.byteRate = SAMPLE_RATE * CHANNELS * (BITS_PER_SAMPLE / 8);
  header.blockAlign = CHANNELS * (BITS_PER_SAMPLE / 8);

  // Calculate total content length
  String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  int headerSize = sizeof(WavHeader);
  int audioDataSize = audioBufferIndex * sizeof(int16_t);
  int totalWavSize = headerSize + audioDataSize;

  // Multipart form data structure
  String formStart = "--" + boundary + "\r\n";
  formStart += "Content-Disposition: form-data; name=\"file\"; filename=\"voice.wav\"\r\n";
  formStart += "Content-Type: audio/wav\r\n\r\n";

  String formModel = "\r\n--" + boundary + "\r\n";
  formModel += "Content-Disposition: form-data; name=\"model\"\r\n\r\n";
  formModel += "whisper-large-v3-turbo\r\n";

  String formEnd = "--" + boundary + "--\r\n";

  int contentLength = formStart.length() + totalWavSize + formModel.length() + formEnd.length();

  // Send HTTP POST request
  client.println("POST /groq/v1/audio/transcriptions HTTP/1.1");
  client.println("Host: api.demeterics.com");
  client.print("Authorization: Bearer ");
  client.println(DEMETERICS_API_KEY);
  client.print("Content-Type: multipart/form-data; boundary=");
  client.println(boundary);
  client.print("Content-Length: ");
  client.println(contentLength);
  client.println("Connection: close");
  client.println();

  // Send multipart form data
  client.print(formStart);

  // Send WAV header
  client.write((uint8_t*)&header, sizeof(WavHeader));

  // Send audio data in chunks
  const int chunkSize = 4096;
  for (int i = 0; i < audioBufferIndex; i += chunkSize / sizeof(int16_t)) {
    int remaining = audioBufferIndex - i;
    int toSend = (remaining > chunkSize / sizeof(int16_t)) ? chunkSize / sizeof(int16_t) : remaining;
    client.write((uint8_t*)&audioBuffer[i], toSend * sizeof(int16_t));

    // Show upload progress
    if (i % (SAMPLE_RATE / 2) == 0) {
      Serial.print("Uploading: ");
      Serial.print((i * 100) / audioBufferIndex);
      Serial.println("%");
    }
  }

  client.print(formModel);
  client.print(formEnd);

  Serial.println("✓ Upload complete, waiting for response...");
  Serial.println();

  // Wait for response
  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000) {
    if (client.available()) {
      String line = client.readStringUntil('\n');

      // Look for the JSON response body
      if (line.startsWith("{")) {
        Serial.println("============================================================");
        Serial.println("TRANSCRIPTION RESULT:");
        Serial.println("============================================================");

        // Parse JSON to extract text
        int textStart = line.indexOf("\"text\":\"") + 8;
        int textEnd = line.indexOf("\"", textStart);
        if (textStart > 7 && textEnd > textStart) {
          String transcription = line.substring(textStart, textEnd);
          Serial.println(transcription);
        } else {
          Serial.println(line);
        }

        Serial.println("============================================================");
        Serial.println();
        break;
      }
    }
  }

  client.stop();

  // Calculate cost
  float duration = (float)audioBufferIndex / SAMPLE_RATE;
  float cost = (duration / 3600.0) * 0.04;

  Serial.print("Audio duration: ");
  Serial.print(duration, 2);
  Serial.println(" seconds");
  Serial.print("Cost: $");
  Serial.println(cost, 6);
  Serial.println();
  Serial.println("Press BOOT button again to record another command");
  Serial.println();
}

// ============================================================================
// EDUCATIONAL NOTES FOR XIAO ESP32-S3 SENSE
// ============================================================================

/*
 * WHAT MAKES THIS BOARD SPECIAL?
 * ===============================
 *
 * 1. THUMB-SIZED (21 x 17.5mm)
 *    - Smaller than most development boards
 *    - Perfect for wearables, drones, IoT devices
 *    - Can fit inside a small enclosure
 *
 * 2. ALL-IN-ONE DESIGN
 *    - Built-in PDM microphone (no external mic needed!)
 *    - Detachable camera (OV2640, 1600x1200)
 *    - Built-in LED
 *    - Built-in BOOT button
 *    - No breadboard or wiring required!
 *
 * 3. GENEROUS MEMORY
 *    - 8MB PSRAM (for buffering audio/images)
 *    - 8MB Flash (for program storage)
 *    - SD card slot (up to 32GB FAT)
 *
 * 4. DUAL-CORE PROCESSOR
 *    - ESP32-S3 @ 240MHz
 *    - Can run WiFi on one core, processing on another
 *
 * 5. GREAT CONNECTIVITY
 *    - WiFi 2.4GHz (100m+ range with antenna)
 *    - Bluetooth LE 5.0
 *    - USB-C (programming + power)
 *
 * PDM vs I2S MICROPHONES
 * =======================
 *
 * The XIAO's built-in mic uses PDM (Pulse Density Modulation):
 *   - Simpler than I2S (only needs 2 wires: Clock + Data)
 *   - Common in smartphones and true wireless earbuds
 *   - We convert PDM → PCM (Pulse Code Modulation) for Whisper
 *
 * I2S (like INMP441) uses 4 wires: Clock, Word Select, Data, Ground
 *   - More complex but more flexible
 *   - PDM is essentially a simplified I2S variant
 *
 * MEMORY LAYOUT ON XIAO ESP32-S3
 * ===============================
 *
 * Internal SRAM (512KB):
 *   - OS/WiFi stack (~200KB)
 *   - Your program code (~50KB)
 *   - Variables and stack (~262KB free)
 *
 * External PSRAM (8MB):
 *   - Audio buffer (160KB for 5 seconds)
 *   - Image buffer (if using camera)
 *   - Remaining 7.8MB for your data!
 *
 * WHY 16KHZ FOR VOICE?
 * ====================
 *
 * Human voice frequency range: 85Hz - 8kHz
 * Nyquist theorem: Sample rate must be 2x highest frequency
 * 16kHz captures everything up to 8kHz (perfect for voice)
 *
 * Comparison:
 *   - Telephone quality: 8kHz
 *   - Voice commands: 16kHz ← We use this!
 *   - Music: 44.1kHz (CD quality)
 *   - High-res audio: 96kHz/192kHz
 *
 * POWER CONSUMPTION
 * =================
 *
 * Typical current draw:
 *   - Deep sleep: 14µA
 *   - Light sleep: 240µA
 *   - Idle (WiFi off): 20mA
 *   - Active (WiFi on): 80-120mA
 *   - Recording audio: 100mA
 *   - Camera capture: 150mA
 *
 * Battery life with 1000mAh LiPo:
 *   - Recording 10 commands/day: ~2 weeks
 *   - Always listening (wake word): ~1 day
 *   - Deep sleep (wake on button): ~2 months
 *
 * PROJECT IDEAS WITH THIS BOARD
 * ==============================
 *
 * Beginner:
 *   - Voice-activated LED strip controller
 *   - Smart doorbell (camera + voice notes)
 *   - Portable voice memo recorder
 *
 * Intermediate:
 *   - Wearable voice assistant (clip to shirt)
 *   - Pet treat dispenser (voice activated)
 *   - Voice-controlled robot car
 *
 * Advanced:
 *   - Real-time language translator
 *   - Bird song identifier (record + classify)
 *   - Security camera with voice alerts
 *   - Voice-controlled drone
 *
 * COST ANALYSIS
 * =============
 *
 * Hardware:
 *   - XIAO ESP32-S3 Sense: $23.99
 *   - USB-C cable: $3 (if you don't have one)
 *   - Total: ~$27
 *
 * Software (per command):
 *   - 5 seconds @ $0.04/hour = $0.000056
 *   - 1000 commands = $0.056
 *   - 10000 commands = $0.56
 *
 * Compare to other voice platforms:
 *   - Google Cloud Speech: $0.006 per 15 seconds = $0.002/command (35x more!)
 *   - AWS Transcribe: $0.024 per minute = $0.002/command (35x more!)
 *   - Groq Whisper: $0.000056/command (cheapest!)
 */
