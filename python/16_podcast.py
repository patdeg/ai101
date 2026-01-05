#!/usr/bin/env python3
################################################################################
# Example 16: Multi-Speaker Podcast Generation
################################################################################
#
# This example demonstrates how to generate a podcast with multiple speakers
# using Demeterics multi-speaker TTS API in a SINGLE API call.
#
# What you'll learn:
# - Multi-speaker audio generation with distinct voices
# - NPR/NotebookLM "Deep Dive" podcast style
# - Voice selection from 30 available options
# - Script formatting with "Speaker: text" pattern
#
# Prerequisites:
# - DEMETERICS_API_KEY environment variable
#
# Usage:
#   export DEMETERICS_API_KEY="dmt_your-api-key"
#   python3 16_podcast.py
#
################################################################################

import os
import json
import http.client
import sys

# Step 1: Check for API key
api_key = os.environ.get('DEMETERICS_API_KEY')
if not api_key:
    print('Error: DEMETERICS_API_KEY not set')
    print('Get your key from: https://demeterics.com')
    sys.exit(1)

# Step 2: Define the podcast script
# Format: "SpeakerName: dialogue text"
# Speaker names must match exactly in speakers array
podcast_script = """Alex: Hey, welcome back to The Deep Dive! I am Alex.
Sam: And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it.
Alex: Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue.
Sam: Right! But here is the thing. That rhyme does not tell you much, does it?
Alex: Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia.
Sam: To India, specifically. Which is why he called the native people Indians.
Alex: Exactly! A massive geography fail that stuck around for five hundred years.
Sam: So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain.
Alex: Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no.
Sam: But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it.
Alex: And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is.
Sam: Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad.
Alex: Because it was! If America was not there, he and his crew would have starved in the middle of the ocean.
Sam: So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea.
Alex: And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas.
Sam: An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries.
Alex: So this whole discovery narrative is complicated, to say the least.
Sam: Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything.
Alex: That is the deep dive for today. Thanks for listening, everyone!
Sam: See you next time!"""

# Step 3: Configure voices for each speaker
# Available voices (30 total):
#   Puck (upbeat), Kore (firm), Charon (informative), Zephyr (bright),
#   Fenrir (excitable), Leda (youthful), Aoede (breezy), Sulafat (warm),
#   Achird (friendly), and 21 others.
speakers = [
    {'name': 'Alex', 'voice': 'Charon'},  # Informative, clear - explains concepts
    {'name': 'Sam', 'voice': 'Puck'}      # Upbeat - energetic, enthusiastic
]

# Step 4: Build the request body (Demeterics format)
request_body = {
    'provider': 'gemini',
    'input': podcast_script,
    'speakers': speakers
}

print('========================================')
print('Multi-Speaker Podcast Generation')
print('========================================')
print('')
print('Speakers:')
for s in speakers:
    print(f"  {s['name']}: {s['voice']}")
print('')
print(f'Script length: {len(podcast_script)} characters')
print('')
print('Generating podcast audio...')

# Step 5: Make the API request
conn = http.client.HTTPSConnection('api.demeterics.com')
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

conn.request('POST', '/tts/v1/generate', json.dumps(request_body), headers)
response = conn.getresponse()

# Step 6: Check for errors
if response.status != 200:
    print(f'API Error: HTTP {response.status}')
    print(response.read().decode('utf-8'))
    sys.exit(1)

# Step 7: Save the audio file directly (Demeterics returns audio bytes)
audio_bytes = response.read()
conn.close()

output_file = 'columbus_podcast.wav'
with open(output_file, 'wb') as f:
    f.write(audio_bytes)

print('')
print('Success!')
print('')
print('Output:')
print(f'  File: {output_file}')
print(f'  Size: {len(audio_bytes)} bytes ({len(audio_bytes) // 1024} KB)')
print('')
print('To play:')
print(f'  mpv {output_file}')
