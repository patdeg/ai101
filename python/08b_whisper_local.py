#!/usr/bin/env python3
################################################################################
# Example 8b: Whisper on YOUR computer (local speech-to-text, no API)
################################################################################
#
# Example 8 sent audio to a server and paid $0.006 per minute. This example runs
# the SAME kind of model (Whisper) on your own machine instead:
#
#                      Hosted (example 8)      Local (this example)
#   Cost               $0.006 / minute         $0
#   Privacy            audio leaves your PC    audio never leaves your PC
#   Setup              just an API key         pip install + a model download
#   Speed              fast                    depends on YOUR computer
#
# What you'll learn:
# - Running an AI model locally with the `faster-whisper` library
# - Model sizes: bigger = more accurate, but slower and larger to download
# - Segments and timestamps (the hosted example only gave you plain text)
# - Real-time factor (RTF): how fast is "fast enough"?
#
# This is the ONE ai101 Python example that needs a pip install. Use a virtual
# environment so it stays out of your system Python:
#
#   python3 -m venv .venv
#   . .venv/bin/activate
#   pip install faster-whisper
#   python3 08b_whisper_local.py                 # tiny model, first run downloads it
#   python3 08b_whisper_local.py base.en         # pick a bigger model
#   python3 08b_whisper_local.py small.en my_recording.mp3
#
# Model sizes (download / typical use): tiny.en ~75 MB, base.en ~150 MB,
# small.en ~480 MB, medium.en ~1.5 GB. The ".en" ones are English-only and a bit
# more accurate for English. Drop ".en" for other languages.
#
# Cost (napkin math): $0. Network use: one model download on the first run.
#
################################################################################

import os
import sys
import time

# Step 1: Import the library, and if it is missing, explain how to fix it.
try:
    from faster_whisper import WhisperModel
except ImportError:
    print('Error: faster-whisper is not installed.')
    print('Run:   python3 -m venv .venv && . .venv/bin/activate && pip install faster-whisper')
    sys.exit(1)

# Step 2: Read the choices from the command line (with sensible defaults).
model_name = sys.argv[1] if len(sys.argv) > 1 else 'tiny.en'
here = os.path.dirname(os.path.abspath(__file__))
audio_path = sys.argv[2] if len(sys.argv) > 2 else os.path.join(here, '..', 'gettysburg.mp3')

if not os.path.exists(audio_path):
    print(f'Error: audio file not found: {audio_path}')
    sys.exit(1)

# Step 3: Load the model. device="cpu" works everywhere. On a computer with an
# NVIDIA graphics card you can use device="cuda" for a big speed-up.
# compute_type="int8" stores the numbers in a smaller form: faster on a CPU and
# nearly as accurate.
print('========================================')
print(f'Local Whisper: {model_name}')
print('========================================')
print('Loading model (the first time this downloads it)...')
t0 = time.time()
model = WhisperModel(model_name, device='cpu', compute_type='int8')
print(f'Model ready in {time.time() - t0:.1f} s')
print('')

# Step 4: Transcribe. This returns a *generator*: the work happens as we loop,
# one segment (a phrase or sentence) at a time.
t0 = time.time()
segments, info = model.transcribe(audio_path, beam_size=5)

print(f'Language: {info.language} (confidence {info.language_probability:.0%})')
print(f'Audio length: {info.duration:.1f} s')
print('')

# Step 5: Print each segment with its start and end time in seconds.
texts = []
for seg in segments:
    print(f'[{seg.start:6.1f}s -> {seg.end:6.1f}s] {seg.text.strip()}')
    texts.append(seg.text.strip())

elapsed = time.time() - t0

# Step 6: Real-time factor. RTF below 1.0 means faster than real time.
# (RTF 0.2 = a 60 second recording takes 12 seconds.)
rtf = elapsed / info.duration if info.duration else 0
hosted_cost = info.duration / 60 * 0.006
print('')
print('========================================')
print('Summary')
print('========================================')
print(f'Words: {len(" ".join(texts).split())}')
print(f'Time: {elapsed:.1f} s   Real-time factor: {rtf:.2f}')
print(f'Cost: $0.00   (the hosted example would cost about ${hosted_cost:.5f})')

# Ideas to try:
# - Run tiny.en, base.en and small.en on the same file and compare the text.
# - Record yourself with a noisy background and see which model copes best.
# - Transcribe python/audiobook/out/episode.wav (from lesson 22) and compare it
#   with episode.json: did the AI voices say exactly what you wrote?
