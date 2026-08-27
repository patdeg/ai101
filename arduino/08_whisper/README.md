# Voice Command Recorder with Whisper AI
### For Seeed Studio XIAO ESP32-S3 Sense

Build a thumb-sized voice-controlled device with **ZERO external components**!

## 🎯 What This Project Does

Press built-in button → Record 5 seconds of audio → Send to Whisper AI → Get text transcription

Perfect for:
- Learning embedded AI without messy wiring
- Prototyping wearable voice assistants
- Building ultra-compact IoT devices
- Understanding digital audio processing

## 🛒 Shopping List

### Required (Just ONE Thing!)

| Part | Quantity | Price | Where to Buy |
|------|----------|-------|--------------|
| Seeed XIAO ESP32-S3 Sense | 1 | $23.99 | [Amazon](https://www.amazon.com/dp/B0C69FFVHH) |
| USB-C Cable | 1 | ~$3 | Included or any USB-C cable |

**Total Cost: ~$27** (Literally just the board!)

### Why This Board is Amazing

✅ **Thumb-sized** (21 x 17.5mm) - Fits anywhere
✅ **Built-in PDM microphone** - No wiring needed
✅ **Built-in camera** (OV2640, 1600x1200) - For vision projects
✅ **Built-in LED** - Recording indicator
✅ **Built-in BOOT button** - No external button needed
✅ **8MB PSRAM** - Enough for audio/image buffering
✅ **8MB Flash** - Plenty of storage
✅ **WiFi + Bluetooth** - Full connectivity
✅ **USB-C** - Modern, reversible connector
✅ **Only $23.99** - Affordable all-in-one solution

## 📐 Wiring Diagram

**NO WIRING REQUIRED!** Just plug in USB-C and you're done! 🎉

```
        XIAO ESP32-S3 Sense
    ┌──────────────────────┐
    │   [Camera Module]    │ ← Detachable camera (included)
    │ ┌─────────────────┐  │
    │ │  (OV2640 Chip)  │  │
    │ └─────────────────┘  │
    │                      │
    │  ●LED    [MIC]●      │ ← Built-in PDM microphone
    │                      │
    │  [BOOT]●  [RESET]●   │ ← BOOT button for recording
    │                      │
    │     [USB-C Port]     │ ← Plug and play!
    └──────────────────────┘
         21mm x 17.5mm
```

### Board Pinout (FYI - No Wiring Needed)

| Feature | GPIO | Used By |
|---------|------|---------|
| BOOT Button | 0 | Our sketch (INPUT_PULLUP) |
| Built-in LED | 21 | Recording indicator |
| PDM Mic Data | 41 | I2S audio capture |
| PDM Mic Clock | 42 | I2S audio capture |
| Camera (various) | Multiple | Future vision projects |

## 💻 Software Setup

### 1. Install Arduino IDE
- Download from [arduino.cc](https://www.arduino.cc/en/software)
- Arduino IDE 2.x is recommended

### 2. Add ESP32 Board Support
1. Open Arduino IDE
2. Go to: **File → Preferences**
3. In "Additional Board Manager URLs", add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Click OK
5. Go to: **Tools → Board → Boards Manager**
6. Search for "esp32"
7. Install **"esp32 by Espressif Systems"** version **3.0.0 or higher**

### 3. Select the XIAO Board
1. **Tools → Board → esp32 → XIAO_ESP32S3**
2. **Tools → PSRAM → OPI PSRAM** (IMPORTANT!)
3. **Tools → USB CDC On Boot → Enabled**
4. **Tools → Port → [Select your USB port]**

### 4. Configure API Credentials
1. Copy `secrets_template.h` to `secrets.h`
2. Edit `secrets.h` with your info:
   ```cpp
   #define WIFI_SSID "YourWiFiName"
   #define WIFI_PASSWORD "YourWiFiPassword"
   #define OPENROUTER_API_KEY "sk-or-v1-your_api_key_here"
   ```
3. Get a free API key at [openrouter.ai](https://openrouter.ai)

### 5. Upload the Code
1. Connect XIAO to your computer via USB-C
2. Click the **Upload** button (→) in Arduino IDE
3. Wait for "Done uploading"
4. Open **Tools → Serial Monitor** (set to 115200 baud)

## 🎮 How to Use

1. **Power on** - Open Serial Monitor to see status
2. **Press and hold BOOT button** (next to USB port)
3. **Speak clearly** - LED blinks while recording (up to 5 seconds)
4. **Release button** - XIAO sends audio to Whisper
5. **Read transcription** - Appears in Serial Monitor!

### Example Serial Output

```
============================================================
XIAO ESP32-S3 Sense - Voice Command Recorder
============================================================
Board: Seeed Studio XIAO ESP32-S3 Sense
Microphone: Built-in PDM Digital Mic
Size: 21 x 17.5mm (Thumb-sized!)
============================================================

Allocating audio buffer (160 KB) in PSRAM...
✓ Audio buffer allocated in PSRAM
Configuring built-in PDM microphone...
✓ PDM microphone configured
Connecting to WiFi: MyHomeWiFi
..........
✓ Connected! IP address: 192.168.1.42

============================================================
READY! Press the BOOT button to record a voice command
(The BOOT button is next to the USB-C port)
============================================================

🎙️  RECORDING - Speak your command now!
------------------------------------------------------------
Recording: 1/5 seconds
Recording: 2/5 seconds
Recording: 3/5 seconds
------------------------------------------------------------
✓ Recording complete!
Captured 48000 samples (3.0 seconds)

📤 Sending audio to Whisper API...
Uploading: 25%
Uploading: 50%
Uploading: 75%
Uploading: 100%
✓ Upload complete, waiting for response...

============================================================
TRANSCRIPTION RESULT:
============================================================
Turn on the lights in the living room.
============================================================

Audio duration: 3.00 seconds
Cost: $0.000033

Press BOOT button again to record another command
```

## 🧠 How It Works

### The Audio Pipeline

1. **Microphone captures sound** - Built-in PDM mic converts air pressure → digital pulses
2. **I2S PDM reads data** - ESP32's I2S peripheral decodes PDM → PCM samples
3. **PSRAM buffering** - 8MB external RAM stores up to 5 seconds of audio
4. **WAV file creation** - Add 44-byte header describing the audio format
5. **WiFi upload** - HTTPS POST to Groq's Whisper API
6. **AI transcription** - GPU-powered Whisper model converts speech → text
7. **Serial output** - Display result on Serial Monitor

### PDM (Pulse Density Modulation) Explained

**What is PDM?**
- A way to represent analog signals using only 1s and 0s
- More 1s = louder sound, More 0s = quieter sound
- Think of it like pointillism in painting - dots create the picture

**PDM vs Traditional Analog Microphones:**

```
Analog Microphone (e.g., MAX4466):
  Sound → Varying Voltage → ADC → Digital Numbers
  Problem: Picks up electrical noise easily

PDM Microphone (XIAO built-in):
  Sound → Digital Pulses → I2S Decoder → Clean Numbers
  Benefit: Immune to electrical noise!
```

**How ESP32 Processes PDM:**
1. PDM mic sends 1-bit stream (1MHz typically)
2. I2S peripheral decimates (downsamples) to 16kHz
3. We get 16-bit PCM samples perfect for Whisper

### Memory Management

```
XIAO ESP32-S3 Memory Map:
┌─────────────────────────────────┐
│ Internal SRAM (512KB)           │
│ ┌─────────────────────────────┐ │
│ │ OS & WiFi Stack  (~200KB)   │ │
│ │ Program Code     (~50KB)    │ │
│ │ Variables/Stack  (~262KB)   │ │
│ └─────────────────────────────┘ │
└─────────────────────────────────┘

┌─────────────────────────────────┐
│ External PSRAM (8MB)            │
│ ┌─────────────────────────────┐ │
│ │ Audio Buffer     (160KB)    │ │ ← 5 sec @ 16kHz
│ │ Image Buffer     (variable) │ │ ← For camera projects
│ │ Free Space       (~7.8MB)   │ │ ← For your data!
│ └─────────────────────────────┘ │
└─────────────────────────────────┘
```

## 🔍 Troubleshooting

### "Error compiling for XIAO_ESP32S3"
- Make sure ESP32 board support is version 3.0.0+
- Select: **Tools → Board → esp32 → XIAO_ESP32S3** (not ESP32S3 Dev Module)
- Enable PSRAM: **Tools → PSRAM → OPI PSRAM**

### "Failed to allocate audio buffer"
- You MUST enable PSRAM: **Tools → PSRAM → OPI PSRAM**
- Without PSRAM, there's not enough RAM for 5 seconds of audio

### "WiFi connection failed"
- Double-check SSID and password in `secrets.h`
- XIAO only supports **2.4GHz WiFi** (not 5GHz)
- Make sure WiFi password doesn't have special quotes/characters

### "Upload failed" / "Port not found"
- Install CH340 USB driver (Google "CH340 driver" + your OS)
- Try a different USB-C cable (some are charge-only)
- Press RESET button after upload starts

### "No sound recorded" (all zeros)
- Make sure camera module is attached (it shares I2S pins)
- The mic is on the back of the board (not the camera side)
- Speak louder and closer to the board
- Check Serial Monitor for I2S driver errors

### "Audio is noisy/distorted"
- Normal - PDM mics can be sensitive to vibration
- Hold board steady while recording
- Avoid recording near loud electrical noise sources

## 💡 Project Ideas

### Beginner
✨ Voice-activated LED blink patterns
✨ Mood tracker (log how you're feeling by voice)
✨ Voice-controlled timer/alarm

### Intermediate
🚀 Wearable voice notes (clip to shirt pocket)
🚀 Smart pet feeder (voice activated)
🚀 Voice-controlled smart home hub
🚀 Language learning pronunciation checker

### Advanced
🔥 Real-time translator (speak → translate → display)
🔥 Security camera with voice alerts
🔥 Voice-controlled drone/robot
🔥 Bird song identifier (with ML classification)

## 📊 Cost Analysis

### Hardware
- XIAO ESP32-S3 Sense: **$13.99**
- USB-C cable: **~$3** (if you don't have one)
- **Total: $17**

### Software (per voice command)
- 5 seconds of audio = 0.00139 hours
- $0.04/hour × 0.00139 hours = **$0.000056**
- That's **17,857 commands for $1.00!**

### Monthly Usage Examples
- 10 commands/day × 30 days = **$0.017/month**
- 50 commands/day × 30 days = **$0.084/month**
- 100 commands/day × 30 days = **$0.168/month**

**Groq Free Tier:** 14,400 requests/day = plenty for hobbyists!

### Comparison to Other Platforms

| Service | Cost per 5-second command | vs Groq |
|---------|---------------------------|---------|
| Groq Whisper | $0.000056 | 1x (baseline) |
| Google Cloud Speech | $0.002 | 35x more expensive |
| AWS Transcribe | $0.002 | 35x more expensive |
| Azure Speech | $0.003 | 53x more expensive |

**Winner: Groq Whisper** 🏆

## 🔒 Security Notes

**Important for Production:**
- This example uses `client.setInsecure()` which skips SSL certificate validation
- For real products, implement proper certificate checking
- Never commit `secrets.h` to GitHub (add to `.gitignore`)
- Rotate your API keys periodically
- Consider local wake-word detection before cloud transcription

## 📚 Learning Resources

### XIAO ESP32-S3 Resources
- [Official Seeed Wiki](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- [Hardware Schematic](https://files.seeedstudio.com/wiki/SeeedStudio-XIAO-ESP32S3/res/XIAO_ESP32S3_SCH_v1.1.pdf)
- [Community Forum](https://forum.seeedstudio.com/)

### Audio Processing
- [PDM Microphones Explained](https://www.analog.com/en/analog-dialogue/articles/introduction-to-pdm-microphones.html)
- [I2S Protocol Guide](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf#i2s)
- [WAV File Format](http://soundfile.sapp.org/doc/WaveFormat/)

### ESP32 Development
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [Arduino-ESP32 GitHub](https://github.com/espressif/arduino-esp32)

## ❓ FAQ

**Q: Do I really need NO external components?**
A: Correct! Just the XIAO board and a USB cable. Everything else is built-in.

**Q: Can I use a regular ESP32 board?**
A: This code is optimized for XIAO ESP32-S3 Sense. Other ESP32-S3 boards will need pin changes.

**Q: Why not use Arduino Uno/Nano?**
A: They don't have WiFi, enough RAM, or I2S support. ESP32 is required.

**Q: Can I make the recording longer?**
A: Yes! Change `RECORD_TIME` to 10 or more seconds. 8MB PSRAM can hold ~50 seconds at 16kHz.

**Q: Does this work in other languages?**
A: Yes! Whisper supports 99+ languages. It auto-detects by default.

**Q: Can I run Whisper on the ESP32 itself?**
A: Not the full model (it's 3GB!). But you can use TinyML for simple wake words.

**Q: How do I power this with a battery?**
A: Connect a 3.7V LiPo battery to the BAT pins on the back. Built-in charging when USB is connected!

**Q: Can I use the camera at the same time?**
A: Yes, but not simultaneously (they share I2S). Capture audio, then capture image separately.

## 🎓 Educational Value

This project teaches:
- ✅ Embedded systems programming
- ✅ Digital audio processing (PDM/PCM)
- ✅ WiFi networking & HTTPS
- ✅ API integration (REST, multipart/form-data)
- ✅ Memory management (SRAM vs PSRAM)
- ✅ Real-time data streaming
- ✅ Error handling & debugging
- ✅ Power management concepts

Perfect for:
- 🎯 High school robotics teams
- 🎯 College IoT courses
- 🎯 Maker/hacker workshops
- 🎯 Self-taught developers

## 🤝 Next Steps

After mastering this project:
1. Try the **03_vision** example (camera + AI image recognition)
2. Combine voice + vision for multimodal AI
3. Add an OLED display for visual feedback
4. Build a complete voice assistant with wake word detection
5. Create a product and sell it on Tindie!

## 📄 License

MIT License - do whatever you want with this code!

## 🙏 Acknowledgments

- Whisper model by OpenAI
- Groq for lightning-fast, affordable API hosting
- Seeed Studio for the amazing XIAO hardware
- ESP32 community for excellent documentation
- You for building cool stuff! 🚀

---

**Questions?** Open an issue on GitHub!
**Built something cool?** Share it with #XIAOWhisper

*Happy building! 🎙️*
