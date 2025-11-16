#!/bin/bash

################################################################################
# EXAMPLE 7: WHISPER - AUDIO TRANSCRIPTION WITH WHISPER-LARGE-V3-TURBO
################################################################################
#
# What this demonstrates:
#   - How to transcribe audio files using Whisper large-v3-turbo
#   - Uploading multipart form data to the API
#   - Processing speech-to-text with fast, cost-effective model
#
# What you'll learn:
#   - How to use the Whisper API for audio transcription
#   - Working with multipart/form-data requests
#   - Handling audio file uploads
#   - Calculating transcription costs and duration
#
# Prerequisites:
#   - DEMETERICS_API_KEY environment variable must be set
#   - Audio file gettysburg.mp3 in the repository root
#
# Expected output:
#   - Complete transcription of the Gettysburg Address
#   - JSON response with transcription text
#   - Duration and cost information
#
# Why this matters:
#   Whisper enables voice interfaces, meeting transcriptions,
#   accessibility features, and voice-to-text applications
#
################################################################################

# Store the API endpoint URL
API_URL="https://api.demeterics.com/groq/v1/audio/transcriptions"

# Path to the audio file
AUDIO_FILE="../gettysburg.mp3"

echo "========================================"
echo "Transcribing Audio with Whisper"
echo "========================================"
echo "Model: whisper-large-v3-turbo"
echo "File: $AUDIO_FILE"
echo ""

# Make the API request with multipart form data and timing
START_TIME=$(date +%s%3N)

RESPONSE=$(curl -s "$API_URL" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -F "file=@$AUDIO_FILE" \
  -F "model=whisper-large-v3-turbo" \
  -F "response_format=verbose_json")

END_TIME=$(date +%s%3N)
LATENCY=$((END_TIME - START_TIME))

echo "Transcription Result:"
echo "========================================"
echo "$RESPONSE" | jq -r '.text'
echo ""
echo "========================================"
echo ""

# Extract duration from response (in seconds)
DURATION=$(echo "$RESPONSE" | jq -r '.duration // 0')

# Calculate cost at $0.04 per hour
# Cost = (duration in seconds / 3600 seconds per hour) * $0.04
if command -v bc &> /dev/null; then
  COST=$(echo "scale=6; ($DURATION / 3600) * 0.04" | bc)
else
  COST=$(awk -v dur="$DURATION" 'BEGIN { printf "%.6f", (dur / 3600) * 0.04 }')
fi

echo "Performance Metrics:"
echo "========================================"
echo "API Latency:    ${LATENCY}ms"
echo "Audio Duration: ${DURATION}s"
echo "Cost:           \$${COST}"
echo "========================================"
echo ""
echo "Full API Response:"
echo "$RESPONSE" | jq .

################################################################################
# EXTRACTING THE TRANSCRIPTION
################################################################################
#
# Get just the transcription text:
#   ./07_whisper.sh | jq -r '.text'
#
# Save transcription to file:
#   ./07_whisper.sh | jq -r '.text' > transcription.txt
#
# Check if transcription succeeded:
#   ./07_whisper.sh | jq -e '.text' > /dev/null && echo "SUCCESS" || echo "FAILED"

# Model: whisper-large-v3-turbo
#   - Fast, cost-effective speech recognition
#   - Supports 99+ languages
#   - Highly accurate transcription
#   - Handles various audio formats (mp3, wav, m4a, etc.)
#   - Pricing: $0.04 per hour of audio
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
# Pricing examples (at $0.04/hour):
#   - 1 minute:  $0.000667 (~$0.0007)
#   - 5 minutes: $0.003333 (~$0.003)
#   - 30 minutes: $0.02
#   - 1 hour:     $0.04
#   - 10 hours:   $0.40
