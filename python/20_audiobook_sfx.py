#!/usr/bin/env python3
################################################################################
# Example 20: Audiobook Step 2 - Sound Effects (ElevenLabs)
################################################################################
#
# Lesson 2 of 4 in the audiobook series (see 19_audiobook_voices.py).
# Here we turn a one-sentence description into a real sound effect.
#
# What you'll learn:
# - Text-to-sound-effect: you DESCRIBE the sound, the model CREATES it
# - A different auth style: the `xi-api-key` header instead of `Bearer`
# - Asking for raw PCM so we need no audio converter
# - A hard call budget, so a typo in a loop can never drain your credits
#
# Prerequisites:
# - ELEVENLABS_API_KEY environment variable (https://elevenlabs.io, free plan works)
#
# Cost (napkin math BEFORE running):
#   Calls    = 2 effects x 1 call x 1 run = 2 calls
#   Credits  = sound effects are billed per second of audio you ask for.
#              2 effects x 3 s = 6 seconds. Check your plan's credit balance
#              in the ElevenLabs dashboard; this is a small fraction of the
#              free monthly credits. Cached re-runs cost 0.
#   Verdict  = go. MAX_CALLS below makes the limit part of the code.
#
# Usage:
#   export ELEVENLABS_API_KEY="your-key"
#   python3 20_audiobook_sfx.py
#
# Output (in audiobook/out/): storm_door.wav, big_switch.wav
#
################################################################################

import hashlib
import http.client
import json
import os
import sys
import wave

SAMPLE_RATE = 24000          # same format as lesson 19, so stems mix cleanly
MAX_CALLS = 4                # hard budget: the program refuses to spend more

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(HERE, 'audiobook', 'out')
CACHE_DIR = os.path.join(OUT_DIR, 'cache')

# Step 1: Check for the API key
api_key = os.environ.get('ELEVENLABS_API_KEY')
if not api_key:
    print('Error: ELEVENLABS_API_KEY not set')
    print('Get a key from: https://elevenlabs.io (Profile -> API Keys)')
    sys.exit(1)

# Step 2: Load the story (the same file as lesson 19)
with open(os.path.join(HERE, 'audiobook', 'episode.json'), encoding='utf-8') as f:
    episode = json.load(f)

os.makedirs(CACHE_DIR, exist_ok=True)

# Step 3: Safety check BEFORE any network call: how many calls could this run make?
# (If you add 50 effects to the JSON file, you want to find out here, not on your bill.)
if len(episode['sfx']) > MAX_CALLS:
    print(f"Error: {len(episode['sfx'])} effects requested but MAX_CALLS is {MAX_CALLS}.")
    print('Raise MAX_CALLS on purpose if you really mean it.')
    sys.exit(1)

print('========================================')
print('Audiobook sound effects')
print('========================================')

for name, sfx in episode['sfx'].items():
    out_path = os.path.join(OUT_DIR, name + '.wav')

    # Step 4: The request body.
    #   text             = your description. Be concrete: material, space, mood.
    #   duration_seconds = how long (0.5 to 30)
    #   prompt_influence = 0..1. High = follows your words literally,
    #                      low = more creative and varied.
    #   model_id         = the sound-effects model
    body = json.dumps({
        'text': sfx['prompt'],
        'duration_seconds': sfx['seconds'],
        'prompt_influence': 0.5,
        'model_id': 'eleven_text_to_sound_v2'
    })

    # Step 5: Cache check, same idea as lesson 19.
    key = hashlib.sha256(body.encode('utf-8')).hexdigest()[:16]
    cache_file = os.path.join(CACHE_DIR, key + '.pcm')

    if os.path.exists(cache_file):
        with open(cache_file, 'rb') as f:
            pcm = f.read()
        status = 'cached'
    else:
        # Step 6: Call the API. output_format=pcm_24000 means "send me raw
        # 16-bit samples at 24 kHz", exactly what the other lessons use.
        conn = http.client.HTTPSConnection('api.elevenlabs.io', timeout=120)
        conn.request('POST', '/v1/sound-generation?output_format=pcm_24000', body, {
            'Content-Type': 'application/json',
            'xi-api-key': api_key
        })
        response = conn.getresponse()
        pcm = response.read()
        conn.close()

        # Step 7: Stop at the FIRST error. On HTTP 429 (too many requests) or a
        # quota error, retrying in a loop makes things worse. Read the message.
        if response.status != 200:
            print(f'API Error on "{name}": HTTP {response.status}')
            print(pcm.decode('utf-8', errors='replace')[:600])
            print('Stopping on purpose. Nothing was retried.')
            sys.exit(1)

        with open(cache_file, 'wb') as f:
            f.write(pcm)
        status = 'new   '

    # Step 8: Wrap the raw samples in a WAV header (same as lesson 19)
    with wave.open(out_path, 'wb') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        w.writeframes(pcm)

    seconds = len(pcm) / (SAMPLE_RATE * 2)
    print(f'[{status}] {name}.wav  {seconds:.1f}s  "{sfx["prompt"][:50]}..."')

print('')
print('Success! Listen:')
print(f'  mpv {OUT_DIR}/storm_door.wav')
print('')
print('Next: python3 21_audiobook_music.py')
