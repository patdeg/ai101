#!/usr/bin/env python3
"""
Example 7: Whisper - Audio Transcription with OpenAI Whisper-1

WHAT THIS DEMONSTRATES:
    - How to transcribe audio files using OpenAI's whisper-1 model
    - Base64-encoding an audio file into a JSON request body
    - Processing speech-to-text through OpenRouter's transcription endpoint
    - Working with file uploads in Python

WHAT YOU'LL LEARN:
    - How to use the Whisper API for audio transcription
    - Base64 encoding for binary uploads (same trick as the vision example)
    - Handling audio file uploads with http.client
    - Processing binary file data
    - Calculating transcription costs

PREREQUISITES:
    - Python 3.6 or higher
    - OPENROUTER_API_KEY environment variable set
    - Audio file gettysburg.mp3 in the repository root

EXPECTED OUTPUT:
    - Complete transcription of the Gettysburg Address
    - JSON response with transcription text
    - Duration and cost information

Run with: python3 07_whisper.py
"""

import http.client  # For HTTPS API requests
import json         # For JSON encoding/decoding
import os           # For environment variables
import base64       # For encoding binary audio data to text
import time         # For measuring latency

# ==============================================================================
# Step 1: Load and validate API credentials
# ==============================================================================

# Retrieve API key from environment variable
api_key = os.environ.get('OPENROUTER_API_KEY')

# Exit if API key is missing
if not api_key:
    print("Error: OPENROUTER_API_KEY not set")
    exit(1)

# ==============================================================================
# Step 2: Prepare the audio file
# ==============================================================================

# Path to the audio file (relative to script location)
audio_file_path = "../gettysburg.mp3"

# Check if file exists
if not os.path.exists(audio_file_path):
    print(f"Error: Audio file not found at {audio_file_path}")
    exit(1)

# Read the audio file as binary data
with open(audio_file_path, 'rb') as f:
    audio_data = f.read()

# Before sending to the API, encode the binary audio to base64 text
# (same trick the vision example uses for images — JSON can't hold raw bytes)
audio_base64 = base64.b64encode(audio_data).decode('utf-8')

print("=" * 60)
print("Transcribing Audio with Whisper")
print("=" * 60)
print(f"Model: openai/whisper-1")
print(f"File: {audio_file_path}")
print(f"File size: {len(audio_data)} bytes")
print()

# ==============================================================================
# Step 3: Build the JSON request body
# ==============================================================================

# OpenRouter's transcription endpoint takes the audio as a base64 string
# plus its format, instead of a raw file upload
payload = {
    "model": "openai/whisper-1",
    "input_audio": {
        "data": audio_base64,
        "format": "mp3"
    },
    "response_format": "verbose_json"  # Ask for duration info, like before
}

# ==============================================================================
# Step 4: Make the API request
# ==============================================================================

# Create secure connection to OpenRouter
conn = http.client.HTTPSConnection("openrouter.ai")

# Set up required headers
headers = {
    'Authorization': f'Bearer {api_key}',
    'Content-Type': 'application/json'
}

# Measure API latency
start_time = time.time()

# Make the API request to transcribe audio
conn.request("POST", "/api/v1/audio/transcriptions", json.dumps(payload), headers)

# Get the response from the server
response = conn.getresponse()

# Read and parse the JSON response
response_data = json.loads(response.read().decode('utf-8'))

# Calculate latency
latency_ms = int((time.time() - start_time) * 1000)

# Close the connection
conn.close()

# ==============================================================================
# Step 5: Display the transcription result
# ==============================================================================

print("Transcription Result:")
print("=" * 60)
print(response_data.get('text', 'No transcription available'))
print()
print("=" * 60)
print()

# Calculate cost based on audio duration
duration = response_data.get('duration', 0)
cost = (duration / 60) * 0.006  # $0.006 per minute

print("Performance Metrics:")
print("=" * 60)
print(f"API Latency:    {latency_ms}ms")
print(f"Audio Duration: {duration}s")
print(f"Cost:           ${cost:.6f}")
print("=" * 60)
print()
print("Full API Response:")
print(json.dumps(response_data, indent=2))

# Model: openai/whisper-1 (via OpenRouter)
#   - Fast, cost-effective speech recognition from OpenAI
#   - Supports 99+ languages with high accuracy
#   - Automatic language detection
#   - Punctuation and formatting included
#   - Handles various audio formats (mp3, wav, m4a, etc.)
#   - Pricing: $0.006 per minute of audio
#   - No free speech-to-text model exists on OpenRouter yet — this is
#     the cheapest correct option (fractions of a cent for a short clip)
#
# Supported audio formats:
#   - MP3, MP4, MPEG, MPGA, M4A, WAV, WEBM
#   - Maximum file size: 25 MB
#   - Recommended: 16kHz or higher sample rate
#
# Pricing examples (at $0.006/minute):
#   - 1 minute:  $0.006
#   - 5 minutes: $0.03
#   - 30 minutes: $0.18
#   - 1 hour:     $0.36
#   - 10 hours:   $3.60
#
# API Response format:
#   {
#     "text": "The transcribed text will appear here..."
#   }
#
# Use cases:
#   - Meeting transcriptions
#   - Voice notes to text
#   - Podcast transcriptions
#   - Accessibility (captions, subtitles)
#   - Voice interfaces
#   - Call center analytics
#   - Interview transcriptions
#   - Lecture notes
#
# Production code example:
#
#   def transcribe_audio(audio_path):
#       """Transcribe an audio file using Whisper"""
#
#       # Read and base64-encode the audio file
#       with open(audio_path, 'rb') as f:
#           audio_data = f.read()
#       audio_base64 = base64.b64encode(audio_data).decode('utf-8')
#
#       # Make API request
#       conn = http.client.HTTPSConnection("openrouter.ai")
#       headers = {
#           'Authorization': f'Bearer {api_key}',
#           'Content-Type': 'application/json'
#       }
#       payload = {
#           "model": "openai/whisper-1",
#           "input_audio": {"data": audio_base64, "format": "mp3"}
#       }
#       conn.request("POST", "/api/v1/audio/transcriptions", json.dumps(payload), headers)
#
#       # Get response
#       response = conn.getresponse()
#       data = json.loads(response.read().decode('utf-8'))
#       conn.close()
#
#       return data.get('text', '')
#
# Advanced options (add to the JSON payload):
#
#   payload["language"] = "en"              # ISO-639-1 code, auto-detected by default
#   payload["response_format"] = "json"     # Options: json, verbose_json, text, srt, vtt
#   payload["temperature"] = 0.0            # 0.0 = deterministic, 1.0 = creative
#
# Error handling example:
#
#   try:
#       with open(audio_path, 'rb') as f:
#           audio_data = f.read()
#   except FileNotFoundError:
#       print(f"Error: File not found: {audio_path}")
#       return None
#   except IOError as e:
#       print(f"Error reading file: {e}")
#       return None
#
#   # Check file size (25MB limit)
#   if len(audio_data) > 25 * 1024 * 1024:
#       print("Error: File size exceeds 25MB limit")
#       return None
#
# Saving transcription to file:
#
#   # Save as plain text
#   with open('transcription.txt', 'w', encoding='utf-8') as f:
#       f.write(response_data['text'])
#
#   # Save as JSON
#   with open('transcription.json', 'w', encoding='utf-8') as f:
#       json.dump(response_data, f, indent=2)
#
# Processing long audio files:
#
#   # For files > 25MB, split into chunks
#   from pydub import AudioSegment
#
#   def split_audio(file_path, chunk_length_ms=600000):  # 10 minutes
#       """Split audio into chunks"""
#       audio = AudioSegment.from_file(file_path)
#       chunks = []
#       for i in range(0, len(audio), chunk_length_ms):
#           chunk = audio[i:i + chunk_length_ms]
#           chunks.append(chunk)
#       return chunks
#
#   def transcribe_long_audio(file_path):
#       """Transcribe audio files longer than 25MB"""
#       chunks = split_audio(file_path)
#       full_transcription = []
#
#       for i, chunk in enumerate(chunks):
#           # Save chunk temporarily
#           chunk_path = f'temp_chunk_{i}.mp3'
#           chunk.export(chunk_path, format='mp3')
#
#           # Transcribe chunk
#           text = transcribe_audio(chunk_path)
#           full_transcription.append(text)
#
#           # Clean up
#           os.remove(chunk_path)
#
#       return ' '.join(full_transcription)
#
# Supported languages (99+):
#   English, Spanish, French, German, Italian, Portuguese, Dutch,
#   Russian, Chinese, Japanese, Korean, Arabic, Turkish, Polish,
#   Ukrainian, Vietnamese, Thai, Indonesian, Hindi, and many more
#
# Language codes (ISO-639-1):
#   en = English
#   es = Spanish
#   fr = French
#   de = German
#   it = Italian
#   pt = Portuguese
#   nl = Dutch
#   ru = Russian
#   zh = Chinese
#   ja = Japanese
#   ko = Korean
#   ar = Arabic
#   tr = Turkish
#   pl = Polish
#   uk = Ukrainian
#   vi = Vietnamese
#   th = Thai
#   id = Indonesian
#   hi = Hindi
#
# Response formats:
#
#   JSON (default):
#     {"text": "Transcription here"}
#
#   Text (plain text):
#     Transcription here
#
#   SRT (SubRip subtitles):
#     1
#     00:00:00,000 --> 00:00:05,000
#     Transcription here
#
#   VTT (WebVTT subtitles):
#     WEBVTT
#
#     00:00:00.000 --> 00:00:05.000
#     Transcription here
#
# Performance tips:
#   - Use MP3 format for smaller file sizes
#   - 16kHz sample rate is usually sufficient
#   - Mono audio reduces file size by ~50%
#   - Remove silence to reduce processing time
#
# Cost optimization:
#   - Whisper pricing is based on audio duration
#   - Remove silence before transcription
#   - Use appropriate quality (don't over-sample)
#   - Cache transcriptions to avoid re-processing
