#!/usr/bin/env python3
################################################################################
# Example 19: Audiobook Step 1 - Narration and Two-Person Dialogue (Gemini TTS)
################################################################################
#
# This is the first of four lessons that build ONE small audio drama:
#
#   music -> narration -> SFX -> dialogue (2 people) -> SFX -> music
#
#   Lesson 19 (this one): the VOICES   (Gemini text-to-speech)
#   Lesson 20:            the SFX      (ElevenLabs sound effects)
#   Lesson 21:            the MUSIC    (an AI writes the score, we render it)
#   Lesson 22:            the MIX      (put everything on a timeline)
#
# What you'll learn:
# - Single-speaker TTS (the narrator) and MULTI-speaker TTS (two characters
#   in ONE request) with the same Gemini model
# - Reading audio that comes back as base64 inside JSON
# - Wrapping raw PCM audio in a WAV header with Python's built-in `wave`
# - Caching: never pay twice for the same audio
#
# Prerequisites:
# - GEMINI_API_KEY environment variable (free key: https://aistudio.google.com/apikey)
#   Called DIRECTLY on Google's API, no proxy, like every other ai101 example.
#
# Cost (napkin math, written BEFORE the first call, as every API user should):
#   2 requests per run (1 narrator + 1 dialogue), about 100 words in total.
#   Text-to-speech models are priced per token of text in and audio out, and
#   this is a few cents at the very most. Re-runs cost $0 thanks to the cache.
#
# Usage:
#   export GEMINI_API_KEY="your-key"
#   python3 19_audiobook_voices.py
#
# Output (in audiobook/out/):
#   narration.wav   the narrator reading the opening
#   dialogue.wav    Maya and Tomas, two voices, one file
#
################################################################################

import base64
import hashlib
import http.client
import json
import os
import sys
import wave

# The model name changes over time. Check https://ai.google.dev/gemini-api/docs/speech-generation
# for the current list, and override it without editing this file:
#   export GEMINI_TTS_MODEL="some-other-tts-model"
MODEL = os.environ.get('GEMINI_TTS_MODEL', 'gemini-3.8-flash-tts')

# Gemini TTS always returns 24,000 samples per second, 16-bit, mono.
# Every lesson in this series uses the same format so the stems mix cleanly.
SAMPLE_RATE = 24000

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(HERE, 'audiobook', 'out')
CACHE_DIR = os.path.join(OUT_DIR, 'cache')

# Step 1: Check for the API key
api_key = os.environ.get('GEMINI_API_KEY')
if not api_key:
    print('Error: GEMINI_API_KEY not set')
    print('Get a free key from: https://aistudio.google.com/apikey')
    sys.exit(1)

# Step 2: Load the story. Every lesson reads the same file, so you can change
# the story in ONE place (audiobook/episode.json) and rebuild everything.
with open(os.path.join(HERE, 'audiobook', 'episode.json'), encoding='utf-8') as f:
    episode = json.load(f)

os.makedirs(CACHE_DIR, exist_ok=True)


def write_wav(path, pcm_bytes):
    """Wrap raw PCM samples in a WAV header so any player can open them."""
    with wave.open(path, 'wb') as w:
        w.setnchannels(1)          # mono
        w.setsampwidth(2)          # 2 bytes = 16 bit
        w.setframerate(SAMPLE_RATE)
        w.writeframes(pcm_bytes)


def speak(text, speech_config, out_path):
    """Send text to Gemini TTS and save the audio. Returns True if it was cached."""

    # Step 3: Build the request body. The text is the prompt, and speechConfig
    # says WHICH voice(s) read it. responseModalities AUDIO asks for sound back.
    body = {
        'contents': [{'parts': [{'text': text}]}],
        'generationConfig': {
            'responseModalities': ['AUDIO'],
            'speechConfig': speech_config
        }
    }
    payload = json.dumps(body)

    # Step 4: Cache check. The cache key is a hash of EVERYTHING that changes
    # the audio (model + request). Same input -> same file -> no new API call.
    key = hashlib.sha256((MODEL + payload).encode('utf-8')).hexdigest()[:16]
    cache_file = os.path.join(CACHE_DIR, key + '.pcm')
    if os.path.exists(cache_file):
        with open(cache_file, 'rb') as f:
            write_wav(out_path, f.read())
        return True

    # Step 5: Call the API. The key goes in a header, never in the URL.
    conn = http.client.HTTPSConnection('generativelanguage.googleapis.com', timeout=120)
    conn.request('POST', f'/v1beta/models/{MODEL}:generateContent', payload, {
        'Content-Type': 'application/json',
        'x-goog-api-key': api_key
    })
    response = conn.getresponse()
    raw = response.read().decode('utf-8')
    conn.close()

    # Step 6: Stop on ANY error. A 429 means "slow down": do NOT retry in a loop,
    # that is how accounts get blocked. Read the message, wait, run again later.
    if response.status != 200:
        print(f'API Error: HTTP {response.status}')
        print(raw[:600])
        sys.exit(1)

    # Step 7: The audio is base64 text inside the JSON, in the first part.
    data = json.loads(raw)
    part = data['candidates'][0]['content']['parts'][0]
    pcm = base64.b64decode(part['inlineData']['data'])

    with open(cache_file, 'wb') as f:
        f.write(pcm)
    write_wav(out_path, pcm)
    return False


# ------------------------------------------------------------------------------
# Part A: the narrator. ONE speaker, so we use a plain voiceConfig.
# The sentence before the colon is a DIRECTION: Gemini TTS follows style
# instructions written in plain English. Try changing it!
# ------------------------------------------------------------------------------
narrator_voice = episode['voices']['narrator']
narration_text = 'Say in a slow, quiet, mysterious storyteller voice: ' + episode['narration']

single_speaker = {
    'voiceConfig': {'prebuiltVoiceConfig': {'voiceName': narrator_voice}}
}

print('========================================')
print(f"Audiobook voices: {episode['title']}")
print('========================================')
print(f'Model: {MODEL}')
print('')

out = os.path.join(OUT_DIR, 'narration.wav')
cached = speak(narration_text, single_speaker, out)
print(f"[{'cached' if cached else 'new   '}] narration.wav  (voice: {narrator_voice})")

# ------------------------------------------------------------------------------
# Part B: the dialogue. TWO speakers in ONE request.
# The script lines look like "Maya: ..." and each name is mapped to a voice in
# multiSpeakerVoiceConfig. Gemini allows at most 2 speakers per request.
# Names in the script MUST match the 'speaker' names exactly.
# ------------------------------------------------------------------------------
names = []
for name, _line in episode['dialogue']:
    if name not in names:
        names.append(name)
if len(names) != 2:
    print(f'Error: the dialogue needs exactly 2 speakers, found {len(names)}: {names}')
    sys.exit(1)

script_lines = '\n'.join(f'{name}: {line}' for name, line in episode['dialogue'])
dialogue_text = ('TTS the following conversation between ' + ' and '.join(names) +
                 '. Maya is calm and brave, Tomas is nervous but excited:\n' + script_lines)

multi_speaker = {
    'multiSpeakerVoiceConfig': {
        'speakerVoiceConfigs': [
            {'speaker': n,
             'voiceConfig': {'prebuiltVoiceConfig': {'voiceName': episode['voices'][n]}}}
            for n in names
        ]
    }
}

out = os.path.join(OUT_DIR, 'dialogue.wav')
cached = speak(dialogue_text, multi_speaker, out)
voices_used = ', '.join(f"{n}={episode['voices'][n]}" for n in names)
print(f"[{'cached' if cached else 'new   '}] dialogue.wav   (voices: {voices_used})")

print('')
print('Success! Listen to your stems:')
print(f'  mpv {OUT_DIR}/narration.wav')
print(f'  mpv {OUT_DIR}/dialogue.wav')
print('')
print('Next: python3 20_audiobook_sfx.py')
