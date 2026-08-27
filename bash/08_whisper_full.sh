#!/bin/bash

################################################################################
# EXAMPLE 7: WHISPER - AUDIO TRANSCRIPTION WITH WHISPER-1
################################################################################
#
# What this demonstrates:
#   - How to transcribe audio files using OpenAI's Whisper-1 model
#   - Base64-encoding an audio file for a JSON request body
#   - Processing speech-to-text with a fast, well-known model
#
# What you'll learn:
#   - How to use the Whisper API for audio transcription
#   - Base64-encoding audio (same trick as Example 4's vision images)
#   - Working with JSON request bodies instead of multipart/form-data
#   - Calculating transcription costs
#
# Prerequisites:
#   - OPENROUTER_API_KEY environment variable must be set
#   - Audio file gettysburg.mp3 in the repository root
#   - base64 command (pre-installed on Linux/Mac)
#
# Expected output:
#   - Complete transcription of the Gettysburg Address
#   - JSON response with transcription text
#   - Cost information
#
# Why this matters:
#   Whisper enables voice interfaces, meeting transcriptions,
#   accessibility features, and voice-to-text applications
#
################################################################################

# Store the API endpoint URL
API_URL="https://openrouter.ai/api/v1/audio/transcriptions"

# Path to the audio file
AUDIO_FILE="../gettysburg.mp3"

echo "========================================"
echo "Transcribing Audio with Whisper"
echo "========================================"
echo "Model: openai/whisper-1"
echo "File: $AUDIO_FILE"
echo ""

# Check if the audio file exists
if [ ! -f "$AUDIO_FILE" ]; then
  echo "Error: Audio file not found at $AUDIO_FILE"
  echo "Please make sure gettysburg.mp3 exists in the root directory"
  exit 1
fi

# STEP 1: Base64-encode the audio file
# OpenRouter's /audio/transcriptions endpoint takes a JSON body (unlike the
# multipart/form-data upload some transcription APIs use), so we base64
# the file the same way Example 4 (vision) base64-encodes an image.
echo "Converting audio to base64 format..."
AUDIO_BASE64=$(base64 -w 0 "$AUDIO_FILE" 2>/dev/null || base64 "$AUDIO_FILE")

# STEP 2: Make the API request with timing
START_TIME=$(date +%s%3N)

RESPONSE=$(curl -s "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -d '{
    "model": "openai/whisper-1",
    "input_audio": {
      "data": "'"$AUDIO_BASE64"'",
      "format": "mp3"
    }
  }')

END_TIME=$(date +%s%3N)
LATENCY=$((END_TIME - START_TIME))

echo "Transcription Result:"
echo "========================================"
echo "$RESPONSE" | jq -r '.text'
echo ""
echo "========================================"
echo ""

echo "Performance Metrics:"
echo "========================================"
echo "API Latency: ${LATENCY}ms"
echo "========================================"
echo ""

# STEP 3: Whisper-1 is not free — note the per-minute pricing
# The response above only carries a "text" field (no duration), so unlike
# the token-based cost math in earlier examples, we can't compute an exact
# dollar figure here — budget by audio length instead.
echo "Cost Notes:"
echo "========================================"
echo "  openai/whisper-1 pricing: \$0.006 per minute of audio (not free)."
echo "  This example's gettysburg.mp3 is a few minutes long, so this run"
echo "  cost a fraction of a cent."
echo "========================================"
echo ""
echo "Full API Response:"
echo "$RESPONSE" | jq .

################################################################################
# EXTRACTING THE TRANSCRIPTION
################################################################################
#
# Get just the transcription text:
#   ./08_whisper_full.sh | jq -r '.text'
#
# Save transcription to file:
#   ./08_whisper_full.sh | jq -r '.text' > transcription.txt
#
# Check if transcription succeeded:
#   ./08_whisper_full.sh | jq -e '.text' > /dev/null && echo "SUCCESS" || echo "FAILED"

# Model: openai/whisper-1
#   - Well-known, highly accurate speech recognition
#   - Supports 99+ languages
#   - Handles various audio formats (mp3, wav, m4a, etc.)
#   - Pricing: $0.006 per minute of audio
#
# Supported audio formats:
#   - MP3, MP4, MPEG, MPGA, M4A, WAV, WEBM
#   - Maximum file size: 25 MB
#
# Use cases:
#   - Meeting transcriptions
#   - Voice notes to text
#   - Podcast transcriptions
#   - Accessibility (captions, subtitles)
#   - Voice interfaces
#   - Call center analytics
#
# Pricing examples (at $0.006/minute):
#   - 1 minute:   $0.006
#   - 5 minutes:  $0.03
#   - 30 minutes: $0.18
#   - 1 hour:     $0.36
#   - 10 hours:   $3.60
