#!/usr/bin/env python3
################################################################################
# Example 16: Multi-Speaker Podcast Generation
################################################################################
#
# This example demonstrates how to generate a podcast with two speakers in a
# SINGLE API call, using OpenRouter's audio/speech endpoint with Fish Audio's
# free multi-speaker TTS model.
#
# What you'll learn:
# - Multi-speaker audio generation with distinct voices
# - NPR/NotebookLM "Deep Dive" podcast style
# - Marking speaker turns with <|speaker:N|> tags
# - Script formatting with "Speaker: text" pattern
#
# Prerequisites:
# - OPENROUTER_API_KEY environment variable
# - FISH_VOICE_1 / FISH_VOICE_2 environment variables: two Fish Audio voice
#   reference IDs. Get free ones by creating an account at https://fish.audio,
#   browsing the voice library, and copying a voice's ID.
#
# Usage:
#   export OPENROUTER_API_KEY="your-api-key"
#   export FISH_VOICE_1="<voice-id-for-Alex>"
#   export FISH_VOICE_2="<voice-id-for-Sam>"
#   python3 16_podcast.py
#
################################################################################

import os
import json
import http.client
import sys

# Step 1: Check for API key and voice IDs
api_key = os.environ.get('OPENROUTER_API_KEY')
if not api_key:
    print('Error: OPENROUTER_API_KEY not set')
    print('Get your key from: https://openrouter.ai/keys')
    sys.exit(1)

voice_1 = os.environ.get('FISH_VOICE_1')
voice_2 = os.environ.get('FISH_VOICE_2')
if not voice_1 or not voice_2:
    print('Error: FISH_VOICE_1 and FISH_VOICE_2 not set')
    print('Get two free voice IDs from https://fish.audio (create account,')
    print('browse the voice library, copy each voice\'s ID)')
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
# Fish Audio's multi-speaker model supports up to 2 speakers per request,
# selected by a "referenceId" array — index 0 in the array is <|speaker:0|>,
# index 1 is <|speaker:1|>, and so on.
speakers = [
    {'name': 'Alex', 'reference_id': voice_1},
    {'name': 'Sam', 'reference_id': voice_2}
]
speaker_index = {s['name']: i for i, s in enumerate(speakers)}

# Step 4: Convert "SpeakerName: text" lines into <|speaker:N|>-tagged text
# This is how Fish Audio's model knows which voice reads which line.
tagged_lines = []
for line in podcast_script.split('\n'):
    name, _, text = line.partition(':')
    idx = speaker_index[name.strip()]
    tagged_lines.append(f'<|speaker:{idx}|>{text.strip()}')
tagged_input = ' '.join(tagged_lines)

# Step 5: Build the request body
request_body = {
    'model': 'fish-audio/s2.1-pro-free:free',
    'input': tagged_input,
    'providerOptions': {
        'fishAudio': {
            'referenceId': [s['reference_id'] for s in speakers]
        }
    },
    'response_format': 'mp3'
}

print('========================================')
print('Multi-Speaker Podcast Generation')
print('========================================')
print('')
print('Speakers:')
for s in speakers:
    print(f"  {s['name']}: {s['reference_id']}")
print('')
print(f'Script length: {len(podcast_script)} characters')
print('')
print('Generating podcast audio...')

# Step 6: Make the API request
conn = http.client.HTTPSConnection('openrouter.ai')
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

conn.request('POST', '/api/v1/audio/speech', json.dumps(request_body), headers)
response = conn.getresponse()

# Step 7: Check for errors
if response.status != 200:
    print(f'API Error: HTTP {response.status}')
    print(response.read().decode('utf-8'))
    sys.exit(1)

# Step 8: Save the audio file directly (OpenRouter returns raw audio bytes)
audio_bytes = response.read()
conn.close()

output_file = 'columbus_podcast.mp3'
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

