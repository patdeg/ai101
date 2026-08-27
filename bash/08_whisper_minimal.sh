#!/bin/sh
# Transcribe audio to text with Whisper
# Usage: ./07_whisper_minimal.sh

# Base64 encode the audio file from root directory
# (OpenRouter's transcription endpoint takes JSON, not multipart/form-data,
# so audio is base64-encoded the same way images are for vision requests)
AUDIO_BASE64=$(base64 -w 0 ../gettysburg.mp3 2>/dev/null || base64 ../gettysburg.mp3)

curl -s https://openrouter.ai/api/v1/audio/transcriptions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENROUTER_API_KEY" \
  -d @- <<EOF
{
  "model": "openai/whisper-1",
  "input_audio": {"data": "$AUDIO_BASE64", "format": "mp3"}
}
EOF

# Parameters explained:
# input_audio.data - Base64-encoded audio file (mp3, wav, m4a, etc., max 25MB)
# input_audio.format - Audio format matching the file (mp3, wav, etc.)
# model - openai/whisper-1 ($0.006 per minute of audio, not free)
# Response is JSON with a "text" field holding the transcript
# Supports 99+ languages with auto-detection
