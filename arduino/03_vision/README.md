# AI Vision Camera - What's in This Image?
### For Seeed Studio XIAO ESP32-S3 Sense

Point, click, and let AI describe what it sees!

## 🎯 What This Does

Press button → Capture photo → Send to AI → Get detailed description

## 🛒 Hardware Needed

- **Seeed XIAO ESP32-S3 Sense** with camera attached ($13.99)
- USB-C cable
- **Total: $14-17**

## 💻 Quick Setup

1. Attach camera module to XIAO board
2. Install Arduino IDE + ESP32 support (v3.0.0+)
3. Select Board: **XIAO_ESP32S3**, PSRAM: **OPI PSRAM**
4. Copy `secrets_template.h` → `secrets.h` and fill in WiFi/API key
5. Upload code, open Serial Monitor (115200 baud)
6. Press BOOT button to capture!

## 🎮 How to Use

1. Point camera at something interesting
2. Press BOOT button
3. LED flashes (photo taken!)
4. Wait 2-5 seconds
5. AI describes what it sees in Serial Monitor

## 📊 Cost

- **$0.0001 per image** (~1/10th of a cent!)
- 10,000 images for $1.00
- 100x cheaper than GPT-4 Vision!

## 🧠 What Can It Do?

✅ Describe scenes ("A sunny beach with palm trees")
✅ Identify objects ("Red apple on wooden table")
✅ Read text (OCR) ("The sign says 'STOP'")
✅ Count items ("There are 3 people")
✅ Understand spatial relationships ("Cat sitting on laptop")

## 💡 Project Ideas

🔸 Smart fridge (see what groceries you have)
🔸 Plant identifier
🔸 Security camera with AI descriptions
🔸 Reading assistant for visually impaired
🔸 Parking spot detector
🔸 Quality control inspector

## 🔧 Troubleshooting

**"Camera init failed"**
- Make sure camera module is properly attached
- Check ribbon cable connection

**"Image too large"**
- PSRAM must be enabled: Tools → PSRAM → OPI PSRAM
- Reduce image size in code if needed

**"Blurry images"**
- OV2640 has fixed focus, optimized for 10cm-2m distance
- Hold steady, ensure good lighting

## 📚 Technical Details

- **Model:** meta-llama/llama-4-scout-17b-16e-instruct (vision + chat model)
- **Camera:** OV2640, up to 1600x1200 resolution
- **Image format:** JPEG, base64-encoded for API
- **Response time:** 2-5 seconds
- **Max image size:** 200KB (UXGA), we use 30KB (VGA)

---

**Next:** Combine with Example 7 (Whisper) for voice + vision AI!
