#!/usr/bin/env python3
"""
Example 7: Whisper - Audio Transcription with Whisper Large-v3-Turbo

WHAT THIS DEMONSTRATES:
    - How to transcribe audio files using Whisper large-v3-turbo
    - Uploading multipart form data to the API
    - Processing speech-to-text with fast, cost-effective model
    - Working with file uploads in Python

WHAT YOU'LL LEARN:
    - How to use the Whisper API for audio transcription
    - Working with multipart/form-data requests in Python
    - Handling audio file uploads with http.client
    - Processing binary file data
    - Calculating transcription costs

PREREQUISITES:
    - Python 3.6 or higher
    - DEMETERICS_API_KEY environment variable set
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
import mimetypes    # For detecting file MIME types
import uuid         # For generating multipart boundary
import time         # For measuring latency

# ==============================================================================
# Step 1: Load and validate API credentials
# ==============================================================================

# Retrieve API key from environment variable
api_key = os.environ.get('DEMETERICS_API_KEY')

# Exit if API key is missing
if not api_key:
    print("Error: DEMETERICS_API_KEY not set")
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

# Get the file name from the path
file_name = os.path.basename(audio_file_path)

# Detect the MIME type of the file
mime_type, _ = mimetypes.guess_type(audio_file_path)
if not mime_type:
    mime_type = 'audio/mpeg'  # Default to MP3 if detection fails

print("=" * 60)
print("Transcribing Audio with Whisper")
print("=" * 60)
print(f"Model: whisper-large-v3-turbo")
print(f"File: {audio_file_path}")
print(f"File size: {len(audio_data)} bytes")
print()

# ==============================================================================
# Step 3: Create multipart form data
# ==============================================================================

# Generate a unique boundary for multipart data
boundary = str(uuid.uuid4())

# Build the multipart form data body
body_parts = []

# Part 1: The audio file
body_parts.append(f'--{boundary}'.encode())
body_parts.append(f'Content-Disposition: form-data; name="file"; filename="{file_name}"'.encode())
body_parts.append(f'Content-Type: {mime_type}'.encode())
body_parts.append(b'')
body_parts.append(audio_data)

# Part 2: The model parameter
body_parts.append(f'--{boundary}'.encode())
body_parts.append(b'Content-Disposition: form-data; name="model"')
body_parts.append(b'')
body_parts.append(b'whisper-large-v3-turbo')

# Part 3: Response format (verbose_json for duration info)
body_parts.append(f'--{boundary}'.encode())
body_parts.append(b'Content-Disposition: form-data; name="response_format"')
body_parts.append(b'')
body_parts.append(b'verbose_json')

# Final boundary
body_parts.append(f'--{boundary}--'.encode())

# Combine all parts with CRLF line endings
body = b'\r\n'.join(body_parts)

# ==============================================================================
# Step 4: Make the API request
# ==============================================================================

# Create secure connection to Demeterics Groq proxy
conn = http.client.HTTPSConnection("api.demeterics.com")

# Set up required headers for multipart form data
headers = {
    'Authorization': f'Bearer {api_key}',
    'Content-Type': f'multipart/form-data; boundary={boundary}'
}

# Measure API latency
start_time = time.time()

# Make the API request to transcribe audio
conn.request("POST", "/groq/v1/audio/transcriptions", body, headers)

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
cost = (duration / 3600) * 0.04  # $0.04 per hour

print("Performance Metrics:")
print("=" * 60)
print(f"API Latency:    {latency_ms}ms")
print(f"Audio Duration: {duration}s")
print(f"Cost:           ${cost:.6f}")
print("=" * 60)
print()
print("Full API Response:")
print(json.dumps(response_data, indent=2))

# Model: whisper-large-v3-turbo
#   - Fast, cost-effective speech recognition from OpenAI
#   - Supports 99+ languages with high accuracy
#   - Automatic language detection
#   - Punctuation and formatting included
#   - Handles various audio formats (mp3, wav, m4a, etc.)
#   - Pricing: $0.04 per hour of audio
#
# Supported audio formats:
#   - MP3, MP4, MPEG, MPGA, M4A, WAV, WEBM
#   - Maximum file size: 25 MB
#   - Recommended: 16kHz or higher sample rate
#
# Pricing examples (at $0.04/hour):
#   - 1 minute:  $0.000667 (~$0.0007)
#   - 5 minutes: $0.003333 (~$0.003)
#   - 30 minutes: $0.02
#   - 1 hour:     $0.04
#   - 10 hours:   $0.40
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
#       # Read audio file
#       with open(audio_path, 'rb') as f:
#           audio_data = f.read()
#
#       # Prepare multipart data
#       boundary = str(uuid.uuid4())
#       body = create_multipart_body(audio_data, boundary)
#
#       # Make API request
#       conn = http.client.HTTPSConnection("api.demeterics.com")
#       headers = {
#           'Authorization': f'Bearer {api_key}',
#           'Content-Type': f'multipart/form-data; boundary={boundary}'
#       }
#       conn.request("POST", "/groq/v1/audio/transcriptions", body, headers)
#
#       # Get response
#       response = conn.getresponse()
#       data = json.loads(response.read().decode('utf-8'))
#       conn.close()
#
#       return data.get('text', '')
#
# Advanced options (add to form data):
#
#   # Specify language (optional - auto-detected by default)
#   body_parts.append(f'--{boundary}'.encode())
#   body_parts.append(b'Content-Disposition: form-data; name="language"')
#   body_parts.append(b'')
#   body_parts.append(b'en')  # ISO-639-1 code (en, es, fr, de, etc.)
#
#   # Response format (optional - json is default)
#   body_parts.append(f'--{boundary}'.encode())
#   body_parts.append(b'Content-Disposition: form-data; name="response_format"')
#   body_parts.append(b'')
#   body_parts.append(b'json')  # Options: json, text, srt, vtt
#
#   # Temperature for randomness (optional - 0.0 to 1.0)
#   body_parts.append(f'--{boundary}'.encode())
#   body_parts.append(b'Content-Disposition: form-data; name="temperature"')
#   body_parts.append(b'')
#   body_parts.append(b'0.0')  # 0.0 = deterministic, 1.0 = creative
#
#   # Timestamp granularities (optional)
#   body_parts.append(f'--{boundary}'.encode())
#   body_parts.append(b'Content-Disposition: form-data; name="timestamp_granularities[]"')
#   body_parts.append(b'')
#   body_parts.append(b'word')  # Options: word, segment
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
