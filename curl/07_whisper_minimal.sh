#!/bin/sh
# Transcribe audio to text with Whisper
# Usage: ./07_whisper_minimal.sh

curl -s https://api.groq.com/openai/v1/audio/transcriptions \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -F "file=@../gettysburg.mp3" \
  -F "model=whisper-large-v3-turbo" \
  -F "response_format=verbose_json"

# Parameters explained:
# file - Audio file to transcribe (mp3, wav, m4a, etc., max 25MB)
# model - whisper-large-v3-turbo (fast, $0.04/hour of audio)
# response_format - json (default), verbose_json (includes duration), text, srt, vtt
# Supports 99+ languages with auto-detection
