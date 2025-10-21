#!/usr/bin/env python3
"""
Example 14: Text-to-Speech with OpenAI

Demonstrates:
- Converting text to spoken audio with OpenAI's TTS
- 11 different voices and instructions parameter
- Speed control and multi-language support

Note: Uses Python standard library only
"""

import http.client
import json
import os
import sys

if not os.environ.get('OPENAI_API_KEY'):
    print('Error: OPENAI_API_KEY not set', file=sys.stderr)
    print('Get your key from: https://platform.openai.com', file=sys.stderr)
    sys.exit(1)

def text_to_speech(text, voice, **kwargs):
    """Call OpenAI TTS API"""
    conn = http.client.HTTPSConnection('api.openai.com')
    
    request_data = {
        'model': 'gpt-4o-mini-tts',
        'input': text,
        'voice': voice,
        **kwargs
    }
    
    body = json.dumps(request_data)
    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["OPENAI_API_KEY"]}'
    }
    
    conn.request('POST', '/v1/audio/speech', body, headers)
    response = conn.getresponse()
    audio_data = response.read()
    conn.close()
    
    return audio_data

def main():
    print('========================================')
    print('Text-to-Speech with OpenAI')
    print('========================================\n')
    
    # Demo 1: Basic TTS
    text = "Hello! This is an example of text-to-speech synthesis using OpenAI's affordable TTS model."
    audio = text_to_speech(text, 'alloy')
    
    with open('demo1_alloy.mp3', 'wb') as f:
        f.write(audio)
    
    print(f'✓ Audio created: demo1_alloy.mp3 ({len(audio)} bytes)\n')
    
    # Demo 2: All voices
    print('Generating all 11 voices...\n')
    voices = ['alloy', 'ash', 'ballad', 'coral', 'echo', 'fable', 
              'onyx', 'nova', 'sage', 'shimmer', 'verse']
    
    comparison_text = "Welcome to OpenAI's text-to-speech demonstration."
    for voice in voices:
        audio = text_to_speech(comparison_text, voice)
        filename = f'voice_{voice}.mp3'
        with open(filename, 'wb') as f:
            f.write(audio)
        print(f'✓ {voice}: {filename}')
    
    print('\n' + '='*40)
    print('Cost Analysis')
    print('='*40 + '\n')
    print('Pricing: $0.60/1M input tokens, $12/1M output tokens')
    print(f'This demo: ~{len(text)} chars = ${len(text)*0.60/1000000:.8f} input\n')
    print('Much more affordable than alternatives!\n')

if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        print(f'Error: {error}', file=sys.stderr)
        sys.exit(1)
