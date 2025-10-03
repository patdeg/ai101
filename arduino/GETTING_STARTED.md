# Getting Started with XIAO ESP32-S3 Sense

This guide walks you through setting up your Seeed Studio XIAO ESP32-S3 Sense board for the ai101 Arduino examples.

## Hardware You'll Need

### Core Components (Purchased)
- **[Seeed Studio XIAO ESP32-S3 Sense](https://www.amazon.com/dp/B0C69FFVHH)** ($23.99)
  - 2.4GHz Wi-Fi + BLE 5.0
  - OV2640 camera sensor (for vision examples)
  - Digital PDM microphone (for audio transcription)
  - 8MB PSRAM + 8MB Flash
  - Battery charge support
  - Rich GPIO interface

- **[12mm Momentary Push Buttons](https://www.amazon.com/dp/B09BKXT1J1)** (5-pack)
  - IP65 waterproof
  - Aluminum alloy shell
  - Pre-wired (1 normally open, no LED)
  - Black shell

- **[Breadboard Kit with Power Supply](https://www.amazon.com/dp/B09TX9CMG1)**
  - 830 + 400 tie-point breadboards
  - Power supply module
  - Jumper wires
  - Battery clips

### Additional Components (Coming Later)
- More hardware will be added as we build more complex examples

## Software Setup

### Step 1: Install Arduino IDE
1. Download Arduino IDE from https://www.arduino.cc/en/software
2. Install the IDE for your operating system

### Step 2: Add ESP32 Board Support
1. Open Arduino IDE
2. Go to **File → Preferences**
3. In **Additional Boards Manager URLs**, add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click **OK**

### Step 3: Install ESP32 Board Package
1. Go to **Tools → Board → Boards Manager**
2. Search for "esp32"
3. Install **"esp32" by Espressif Systems**
4. Wait for installation to complete

### Step 4: Select Your Board
1. Connect your XIAO ESP32-S3 via USB-C cable
2. Go to **Tools → Board → esp32**
3. Select **"XIAO_ESP32S3"**
4. Go to **Tools → Port** and select the COM port for your board

## Your First Program: Blink + Hello World

This example blinks the onboard LED and prints "Hello World" to the Serial Monitor.

### The Code

```cpp
// Blink + Hello World for Seeed XIAO ESP32S3 (Sense)
// Onboard LED is on GPIO 21

#define LED_PIN 21

void setup() {
  pinMode(LED_PIN, OUTPUT);

  // Initialize serial console
  Serial.begin(115200);      // 115200 baud is standard for ESP32
  while (!Serial) {
    ; // wait for Serial to be ready (needed on some boards)
  }
  Serial.println("Booting up..."); // First message
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Hello World");
  delay(1000);

  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
```

### Upload and Test

1. Copy the code above into Arduino IDE
2. Click **Verify** (checkmark icon) to compile
3. Click **Upload** (arrow icon) to flash to the board
4. Open **Tools → Serial Monitor**
5. Set baud rate to **115200**
6. You should see:
   - LED blinking every second
   - "Booting up..." followed by "Hello World" messages

### What's Happening?

- **GPIO 21**: The onboard LED pin (specific to XIAO ESP32-S3)
- **115200 baud**: Standard serial speed for ESP32 boards
- **`while (!Serial)`**: Waits for serial connection (useful for debugging)
- **1000ms delays**: Creates 1-second on/off blink pattern

## Next Steps

Now that your board is working, you're ready for the AI examples:

1. **[01_basic_chat](01_basic_chat/)** - Send questions to AI over WiFi
2. **[02_system_prompt](02_system_prompt/)** - Control AI behavior
3. **[03_vision](03_vision/)** - Use the onboard camera for image analysis
4. **[04_safety_check](04_safety_check/)** - Content moderation
5. **[05_image_safety_check](05_image_safety_check/)** - Image safety checks
6. **[06_prompt_guard](06_prompt_guard/)** - Jailbreak detection
7. **[07_whisper](07_whisper/)** - Audio transcription with onboard microphone

## Troubleshooting

### Board Not Detected
- Make sure USB cable supports data transfer (not just charging)
- Try a different USB port
- Install CH340 or CP2102 drivers if needed

### Upload Fails
- Press and hold BOOT button while clicking Upload
- Release BOOT button after "Connecting..." appears
- Try different upload speed: **Tools → Upload Speed → 115200**

### Serial Monitor Shows Garbage
- Check baud rate is set to **115200**
- Press the RESET button on the board

### LED Not Blinking
- XIAO ESP32-S3 has a small orange LED near the USB port
- It's not very bright - look closely in dim lighting

## Resources

- **Official Wiki**: https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/
- **Schematic**: https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32S3/res/XIAO_ESP32S3_ExpBoard_v1.0_SCH.pdf
- **Pinout Diagram**: https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32S3/img/2.jpg
- **Arduino-ESP32 Docs**: https://docs.espressif.com/projects/arduino-esp32/en/latest/

## What Makes This Board Special?

The XIAO ESP32-S3 Sense has **everything onboard** for AI examples:
- **WiFi** → API calls to Groq/OpenAI
- **Camera** → Vision AI (example 03)
- **Microphone** → Audio transcription (example 07)
- **PSRAM** → Buffer large images/audio files
- **Tiny size** → 21mm × 17.5mm, fits anywhere

No external sensors needed - just the board, breadboard, and button!
