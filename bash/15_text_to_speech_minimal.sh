#!/bin/sh
################################################################################
# Example 14: Text-to-Speech with OpenAI (Minimal Version)
################################################################################
#
# This example demonstrates text-to-speech synthesis using OpenAI's TTS API.
#
# What you'll learn:
# - How to convert text to spoken audio with OpenAI
# - Available voice options (11 voices)
# - Audio format handling
# - Token usage and pricing for TTS
#
# Key concepts:
# - gpt-4o-mini-tts: Affordable model ($0.60/1M input, $12/1M output)
# - 11 voices: alloy, ash, ballad, coral, echo, fable, onyx, nova, sage, shimmer, verse
# - Supports multiple languages automatically
# - Speed control (0.25 to 4.0)
#
################################################################################

# Check for API key
if [ -z "$OPENAI_API_KEY" ]; then
  echo "Error: OPENAI_API_KEY not set" >&2
  echo "Get your key from: https://platform.openai.com" >&2
  exit 1
fi

# API endpoint
API_URL="https://api.openai.com/v1/audio/speech"

# Text to convert to speech
TEXT="Hello! This is an example of text-to-speech synthesis using OpenAI's affordable TTS model."

# Voice to use (try: alloy, echo, fable, onyx, nova, shimmer)
VOICE="alloy"

# Output file
OUTPUT_FILE="output.mp3"

echo "Converting text to speech..."
echo "Voice: $VOICE"
echo "Text: $TEXT"
echo ""

# Make API request and save audio to file
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d @- <<EOF > "$OUTPUT_FILE"
{
  "model": "gpt-4o-mini-tts",
  "input": "$TEXT",
  "voice": "$VOICE",
  "response_format": "mp3"
}
EOF

# Check if file was created
if [ -f "$OUTPUT_FILE" ]; then
  FILE_SIZE=$(wc -c < "$OUTPUT_FILE")
  echo "✓ Audio file created: $OUTPUT_FILE"
  echo "  File size: $FILE_SIZE bytes"
  echo ""
  echo "Play with: mpv $OUTPUT_FILE       (Linux/macOS/Windows)"
  echo "      or:  afplay $OUTPUT_FILE   (macOS)"
  echo "      or:  start $OUTPUT_FILE    (Windows)"
else
  echo "Error: Failed to create audio file" >&2
  exit 1
fi

################################################################################
# Understanding Text-to-Speech Pricing
################################################################################
#
# Pricing (OpenAI TTS - October 2025):
# - gpt-4o-mini-tts:
#   - Input: $0.60 per 1M tokens
#   - Output: $12.00 per 1M tokens
#
# Token calculation:
# - Input: Approximately 1 token per character of input text
# - Output: Audio duration-based (varies by length and complexity)
# - This example (~100 chars) ≈ 100 input tokens ≈ $0.00006
#
# Cost examples (input only):
# - 100 characters:   $0.00006
# - 1,000 characters: $0.0006
# - 10,000 characters: $0.006
#
# Note: Much more affordable than playai-tts ($50/1M)!
#
# Available voices (11 total):
# - alloy   - Neutral, balanced
# - ash     - Clear, articulate
# - ballad  - Smooth, expressive
# - coral   - Warm, friendly
# - echo    - Clear, professional
# - fable   - Narrative, storytelling
# - onyx    - Deep, authoritative
# - nova    - Energetic, youthful
# - sage    - Calm, measured
# - shimmer - Bright, cheerful
# - verse   - Poetic, expressive
#
# Response formats:
# - mp3 (default, compressed, good quality)
# - opus (low latency, streaming)
# - aac (compressed, good for mobile)
# - flac (lossless, larger files)
# - wav (uncompressed, largest)
# - pcm (raw audio data)
#
# Speed control:
# - Range: 0.25 to 4.0
# - Default: 1.0 (normal speed)
# - 0.5 = half speed (slower)
# - 2.0 = double speed (faster)
#
# Supported languages:
# - Automatically detects language from input text
# - Supports 50+ languages including:
#   English, Spanish, French, German, Italian, Portuguese,
#   Dutch, Russian, Arabic, Japanese, Korean, Chinese, and many more
#
################################################################################
