#!/bin/sh
################################################################################
# Example 15: Text-to-Speech with Fish Audio (via OpenRouter) (Minimal Version)
################################################################################
#
# This example demonstrates text-to-speech synthesis using OpenRouter's
# /audio/speech endpoint and Fish Audio's S2.1 Pro Free model.
#
# What you'll learn:
# - How to convert text to spoken audio through OpenRouter
# - Optional voice selection via a Fish Audio reference ID
# - Audio format handling
#
# Key concepts:
# - fish-audio/s2.1-pro-free:free: genuinely free, no card required
# - No referenceId supplied -> Fish Audio picks a default voice
# - With a referenceId -> pick any voice from https://fish.audio
#
################################################################################

# Check for API key
if [ -z "$OPENROUTER_API_KEY" ]; then
  echo "Error: OPENROUTER_API_KEY not set" >&2
  echo "Get your key from: https://openrouter.ai/keys" >&2
  exit 1
fi

# API endpoint
API_URL="https://openrouter.ai/api/v1/audio/speech"

# Text to convert to speech
TEXT="Hello! This is an example of text-to-speech synthesis using Fish Audio's free TTS model."

# Output file
OUTPUT_FILE="output.mp3"

echo "Converting text to speech..."
echo "Text: $TEXT"
echo ""

# Make API request and save audio to file
curl -s -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -d @- <<EOF > "$OUTPUT_FILE"
{
  "model": "fish-audio/s2.1-pro-free:free",
  "input": "$TEXT",
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
# Pricing (Fish Audio S2.1 Pro Free, via OpenRouter):
# - fish-audio/s2.1-pro-free:free: $0 (rate-limited, best-effort
#   availability - meant for learning/prototyping, not production)
#
# Picking a voice:
# - No "voice" field like OpenAI's TTS - instead, pass a Fish Audio
#   "referenceId" via providerOptions.fishAudio.referenceId
# - Get free voice IDs at https://fish.audio: sign up, browse the voice
#   library, copy the ID of any voice you like
# - Without a referenceId, Fish Audio picks a default voice
#
# Example with a chosen voice:
#   {
#     "model": "fish-audio/s2.1-pro-free:free",
#     "input": "...",
#     "providerOptions": { "fishAudio": { "referenceId": "your-voice-id" } },
#     "response_format": "mp3"
#   }
#
# Response formats:
# - mp3 (default, compressed, good quality)
# - opus (low latency, streaming)
# - aac (compressed, good for mobile)
# - flac (lossless, larger files)
# - wav (uncompressed, largest)
# - pcm (raw audio data)
#
################################################################################
