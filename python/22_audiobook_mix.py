#!/usr/bin/env python3
################################################################################
# Example 22: Audiobook Step 4 - The Mix (put it all on a timeline)
################################################################################
#
# Lesson 4 of 4 in the audiobook series. No API calls here: this lesson only
# uses the files made by lessons 19, 20 and 21, so it is free and instant.
#
# The finished episode follows this order:
#
#   music -> narration -> SFX -> dialogue (2 people) -> SFX -> music
#
# What you'll learn:
# - A mix is just ADDING sample numbers together, at the right start time
# - Gain (volume) as a plain multiplication
# - Fades: why audio needs them (an abrupt start or stop makes a click)
# - Overlap: sound effects and music tails sit UNDER the voices
# - Normalizing: using the full volume range without clipping
#
# Prerequisites: run lessons 19, 20 and 21 first.
#
# Usage:
#   python3 22_audiobook_mix.py
#
# Output (in audiobook/out/): episode.wav (and episode.mp3 if ffmpeg is installed)
#
################################################################################

import array
import json
import os
import shutil
import subprocess
import sys
import wave

SAMPLE_RATE = 24000
HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(HERE, 'audiobook', 'out')


def read_wav(name):
    """Load a WAV as a list of floats between -1.0 and 1.0."""
    path = os.path.join(OUT_DIR, name + '.wav')
    if not os.path.exists(path):
        print(f'Error: {name}.wav is missing. Run lessons 19, 20 and 21 first.')
        sys.exit(1)
    with wave.open(path, 'rb') as w:
        if w.getframerate() != SAMPLE_RATE or w.getnchannels() != 1 or w.getsampwidth() != 2:
            print(f'Error: {name}.wav must be 24 kHz, mono, 16 bit (the lessons make it so).')
            sys.exit(1)
        raw = w.readframes(w.getnframes())
    samples = array.array('h')
    samples.frombytes(raw)               # assumes a little-endian machine (PCs and Macs are)
    return [s / 32768.0 for s in samples]


def seconds(clip):
    return len(clip) / SAMPLE_RATE


class Mix:
    """A blank 'tape'. place() lays a clip onto it at a start time."""

    def __init__(self):
        self.tape = []
        self.log = []

    def place(self, label, clip, start, gain=1.0, fade_in=0.0, fade_out=0.0):
        begin = int(start * SAMPLE_RATE)
        end = begin + len(clip)
        if end > len(self.tape):                       # the tape grows as needed
            self.tape.extend([0.0] * (end - len(self.tape)))

        n_in = int(fade_in * SAMPLE_RATE)
        n_out = int(fade_out * SAMPLE_RATE)
        for i, sample in enumerate(clip):
            g = gain                                   # 1. volume
            if i < n_in:
                g *= i / n_in                          # 2. fade in: 0 -> 1 in a straight line
            if i >= len(clip) - n_out:
                g *= (len(clip) - i) / n_out           # 3. fade out: 1 -> 0
            self.tape[begin + i] += sample * g         # 4. ADD it to what is already there

        self.log.append((start, start + seconds(clip), label, gain))
        return start + seconds(clip)                   # when this clip ends

    def normalize(self, target=0.9):
        """Scale everything so the loudest moment is 90% of the maximum.
        Louder than 100% would CLIP (distort), so we leave a little headroom."""
        peak = max(abs(x) for x in self.tape) or 1.0
        self.tape = [x / peak * target for x in self.tape]
        return peak


# Step 1: Load every stem made in the earlier lessons
episode = json.load(open(os.path.join(HERE, 'audiobook', 'episode.json'), encoding='utf-8'))
sfx_names = list(episode['sfx'].keys())
if len(sfx_names) < 2:
    print('Error: this timeline needs two sound effects in episode.json.')
    sys.exit(1)

theme = read_wav('theme')
narration = read_wav('narration')
dialogue = read_wav('dialogue')
sfx_a = read_wav(sfx_names[0])
sfx_b = read_wav(sfx_names[1])

# Step 2: Build the timeline, one clip at a time.
# Each place() returns the moment the clip ENDS, so the next clip can start
# relative to it. Starting a clip a bit BEFORE the previous one ends (negative
# offset) makes them overlap, which sounds natural instead of robotic.
mix = Mix()

# 1) MUSIC opens the episode and fades out while the narrator begins.
mix.place('music (intro)', theme, 0.0, gain=0.8, fade_out=5.0)
narration_start = max(0.0, seconds(theme) - 5.0)

# 2) NARRATION. Speech is the star, so it stays at full gain.
t = mix.place('narration', narration, narration_start, gain=1.0, fade_in=0.05, fade_out=0.05)

# 3) SFX #1 right after the narrator stops.
sfx_a_start = t + 0.3
t = mix.place(f'sfx: {sfx_names[0]}', sfx_a, sfx_a_start, gain=0.7, fade_out=0.3)

# 4) DIALOGUE starts while the sound effect is still ringing out (0.5 s overlap).
dialogue_start = t - 0.5
t = mix.place('dialogue (2 voices)', dialogue, dialogue_start, gain=1.0, fade_in=0.05, fade_out=0.05)

# 5) SFX #2 slightly before the last words end, so it feels like part of the scene.
sfx_b_start = t - 0.3
t = mix.place(f'sfx: {sfx_names[1]}', sfx_b, sfx_b_start, gain=0.7, fade_out=0.3)

# 6) MUSIC returns underneath the end of the effect and fades out slowly.
outro_start = t - 1.5
mix.place('music (outro)', theme, outro_start, gain=0.8, fade_in=2.0, fade_out=6.0)

# Step 3: Normalize and save
peak = mix.normalize()
out_path = os.path.join(OUT_DIR, 'episode.wav')
pcm = array.array('h', (int(max(-1.0, min(1.0, x)) * 32767) for x in mix.tape))
with wave.open(out_path, 'wb') as w:
    w.setnchannels(1)
    w.setsampwidth(2)
    w.setframerate(SAMPLE_RATE)
    w.writeframes(pcm.tobytes())

# Step 4: Show the timeline (this table is the "score" of your whole episode)
print('========================================')
print(f"Mixing: {episode['title']}")
print('========================================')
print(f"{'start':>7} {'end':>7}  {'gain':>4}  clip")
for start, end, label, gain in sorted(mix.log):
    print(f'{start:7.1f} {end:7.1f}  {gain:4.1f}  {label}')
print('')
print(f'Total length: {len(mix.tape) / SAMPLE_RATE:.1f} s   (peak before normalizing: {peak:.2f})')

# Step 5: Optional MP3. WAV is big; MP3 is easy to share. Needs ffmpeg installed.
if shutil.which('ffmpeg'):
    mp3_path = os.path.join(OUT_DIR, 'episode.mp3')
    subprocess.run(['ffmpeg', '-y', '-loglevel', 'error', '-i', out_path,
                    '-codec:a', 'libmp3lame', '-b:a', '128k', mp3_path], check=True)
    print(f'MP3: {mp3_path}')
else:
    print('(Install ffmpeg to also get an MP3.)')

print('')
print('Success! Play your audio drama:')
print(f'  mpv {out_path}')
