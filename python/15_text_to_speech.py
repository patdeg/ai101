#!/usr/bin/env python3
"""
Example 15: Text-to-Speech with Fish Audio (via OpenRouter)

Demonstrates:
- Converting text to spoken audio using OpenRouter's /audio/speech endpoint
- Fish Audio S2.1 Pro Free — a genuinely free TTS model, no card required
- Picking a voice by reference ID from Fish Audio's voice library

Note: Uses Python standard library only
"""

import http.client
import json
import os
import sys

if not os.environ.get('OPENROUTER_API_KEY'):
    print('Error: OPENROUTER_API_KEY not set', file=sys.stderr)
    print('Get your key from: https://openrouter.ai/keys', file=sys.stderr)
    sys.exit(1)

def text_to_speech(text, reference_id=None):
    """Call OpenRouter's /audio/speech endpoint (Fish Audio S2.1 Pro Free)."""
    conn = http.client.HTTPSConnection('openrouter.ai')

    request_data = {
        'model': 'fish-audio/s2.1-pro-free:free',
        'input': text,
        'response_format': 'mp3'
    }
    # Without a referenceId, Fish Audio picks a default voice — good enough for
    # a first TTS demo. Pass one to pick a specific voice (see Example 16 for
    # the multi-speaker version of this).
    if reference_id:
        request_data['providerOptions'] = {'fishAudio': {'referenceId': reference_id}}

    body = json.dumps(request_data)
    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["OPENROUTER_API_KEY"]}'
    }

    conn.request('POST', '/api/v1/audio/speech', body, headers)
    response = conn.getresponse()
    if response.status != 200:
        print(f'API Error: HTTP {response.status}', file=sys.stderr)
        print(response.read().decode('utf-8'), file=sys.stderr)
        sys.exit(1)

    audio_data = response.read()
    conn.close()

    return audio_data

def main():
    print('========================================')
    print('Text-to-Speech with Fish Audio (OpenRouter)')
    print('========================================\n')

    # Demo 1: Basic TTS with the default voice
    text = "Hello! This is an example of text-to-speech synthesis using Fish Audio's free TTS model."
    audio = text_to_speech(text)

    with open('demo1_default_voice.mp3', 'wb') as f:
        f.write(audio)

    print(f'✓ Audio created: demo1_default_voice.mp3 ({len(audio)} bytes)\n')

    # Demo 2: A specific voice, if the learner has picked one
    reference_id = os.environ.get('FISH_VOICE_1')
    if reference_id:
        comparison_text = "This is the same text, spoken with a voice picked from Fish Audio's library."
        audio = text_to_speech(comparison_text, reference_id)
        filename = 'demo2_chosen_voice.mp3'
        with open(filename, 'wb') as f:
            f.write(audio)
        print(f'✓ Chosen voice: {filename}\n')
    else:
        print('Tip: set FISH_VOICE_1 to a voice ID from https://fish.audio to')
        print('hear a specific voice instead of the default.\n')

    print('=' * 40)
    print('Cost Analysis')
    print('=' * 40 + '\n')
    print('Pricing: fish-audio/s2.1-pro-free:free costs $0 (rate-limited,')
    print('best-effort availability — meant for learning/prototyping)\n')

if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        print(f'Error: {error}', file=sys.stderr)
        sys.exit(1)
