#!/usr/bin/env python3
################################################################################
# Example 21: Audiobook Step 3 - Music (AI writes the score, we play it)
################################################################################
#
# Lesson 3 of 4 in the audiobook series.
#
# Two very different jobs, each done by the right tool:
#   1. COMPOSE : an AI model writes the notes (it is good at ideas and text)
#   2. RENDER  : plain Python turns the notes into sound (it is exact and free)
#
# This is how real music software works too. A MIDI file is NOT sound. It is a
# list of instructions: "play note 69, starting at beat 4, for 2 beats, soft".
# Any synthesizer can turn those instructions into audio.
#
# What you'll learn:
# - Asking an AI for STRUCTURED output (a JSON score) and not trusting it blindly
# - Validating and clamping AI output before using it
# - What a MIDI file really is, and how to write one with `struct`
# - How a synthesizer works: sine waves + an envelope (attack and release)
# - Graceful fallback: no key or a bad answer -> a built-in score, never a crash
#
# Prerequisites:
# - OPENROUTER_API_KEY (optional!). Without it the lesson uses the built-in
#   score from episode.json, so you can always finish the series.
#
# Cost (napkin math): 1 chat call, about 500 tokens in and 1500 out with
# qwen/qwen3.8-flash ($0.15 / $0.47 per 1M tokens) = well under a tenth of a
# cent. The answer is cached, so a re-run makes 0 calls.
#
# Usage:
#   python3 21_audiobook_music.py              # AI composes (needs the key)
#   python3 21_audiobook_music.py --offline    # use the built-in score
#
# Output (in audiobook/out/): theme.mid (open it in any MIDI player or
# MuseScore) and theme.wav (our own rendering, used by lesson 22).
#
################################################################################

import hashlib
import http.client
import json
import math
import os
import struct
import sys
import wave

SAMPLE_RATE = 24000
MODEL = 'qwen/qwen3.8-flash'
TICKS_PER_BEAT = 480         # MIDI time resolution: 480 ticks in one beat

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(HERE, 'audiobook', 'out')
CACHE_DIR = os.path.join(OUT_DIR, 'cache')

# General MIDI instrument numbers (0-based) for our three timbres. A real
# MIDI player uses these to pick a sound; our own synth below uses the NAME.
GM_PROGRAM = {'pad': 89, 'pluck': 24, 'bass': 33}


# ------------------------------------------------------------------------------
# Part 1: Compose. Ask the AI for a score as JSON.
# ------------------------------------------------------------------------------
def ask_ai_for_score(brief, api_key):
    """One chat call. Returns the parsed JSON score, or None if anything fails."""

    # The prompt is a CONTRACT. We show the exact shape we want, and the limits.
    prompt = f"""You are a film composer. Write a short score for this brief:

{brief}

Reply with ONLY a JSON object (no explanation, no code fences) shaped like this:
{{"tempo": 80, "tracks": [
  {{"name": "pad", "timbre": "pad", "notes": [[57, 0, 8, 60], [53, 8, 8, 60]]}},
  {{"name": "melody", "timbre": "pluck", "notes": [[69, 0, 1, 90], [72, 1, 1, 80]]}},
  {{"name": "bass", "timbre": "bass", "notes": [[45, 0, 4, 90]]}}
]}}

Each note is [pitch, start_beat, length_in_beats, velocity].
Rules: pitch 28-96 (60 is middle C); velocity 1-127; timbre is one of
"pad", "pluck", "bass"; exactly those 3 tracks; at most 120 notes in total;
the piece is 32 beats long; the last notes must end at beat 32."""

    body = json.dumps({
        'model': MODEL,
        'messages': [{'role': 'user', 'content': prompt}],
        'max_tokens': 4000,
        # JSON mode: the API only allows valid JSON back. Thinking is off, because
        # hidden thinking tokens would eat the budget (see example 01).
        'response_format': {'type': 'json_object'},
        'reasoning': {'enabled': False}
    })

    # Cache: same brief + same model = same answer file, no second API call.
    key = hashlib.sha256(body.encode('utf-8')).hexdigest()[:16]
    cache_file = os.path.join(CACHE_DIR, 'score_' + key + '.json')
    if os.path.exists(cache_file):
        print('  (using cached AI answer)')
        with open(cache_file, encoding='utf-8') as f:
            return json.load(f)

    conn = http.client.HTTPSConnection('openrouter.ai', timeout=120)
    conn.request('POST', '/api/v1/chat/completions', body, {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {api_key}'
    })
    response = conn.getresponse()
    raw = response.read().decode('utf-8')
    conn.close()

    if response.status != 200:
        print(f'  AI call failed: HTTP {response.status} {raw[:200]}')
        return None

    try:
        text = json.loads(raw)['choices'][0]['message']['content']
        # Models sometimes wrap JSON in prose or fences. Keep only {...}.
        score = json.loads(text[text.index('{'):text.rindex('}') + 1])
    except (KeyError, ValueError, IndexError, TypeError, AttributeError) as err:
        print(f'  Could not read the AI answer as JSON: {err}')
        return None

    # Note: we do NOT save to the cache here. Only an answer that passes
    # clean_score() is worth keeping, so main code calls save_score_cache().
    score['_cache_file'] = cache_file
    return score


def save_score_cache(score):
    cache_file = score.pop('_cache_file', None)
    if cache_file:
        with open(cache_file, 'w', encoding='utf-8') as f:
            json.dump(score, f)


def clean_score(score):
    """Never trust AI output. Check every field and clamp it into a safe range.

    Raises ValueError if the score is unusable. Everything else is repaired.
    """
    tempo = max(50, min(160, int(score['tempo'])))
    tracks = []
    total_notes = 0
    for track in score['tracks'][:4]:
        timbre = track.get('timbre')
        if timbre not in GM_PROGRAM:
            raise ValueError(f'unknown timbre: {timbre!r}')
        notes = []
        for pitch, start, length, vel in track['notes']:
            total_notes += 1
            if total_notes > 200:
                raise ValueError('too many notes')
            notes.append((max(28, min(96, int(pitch))),       # pitch
                          max(0.0, min(60.0, float(start))),   # start (beats)
                          max(0.1, min(16.0, float(length))),  # length (beats)
                          max(1, min(127, int(vel)))))         # velocity
        tracks.append({'name': str(track.get('name', timbre)), 'timbre': timbre, 'notes': notes})
    if not tracks or total_notes == 0:
        raise ValueError('empty score')
    return {'tempo': tempo, 'tracks': tracks}


# ------------------------------------------------------------------------------
# Part 2: Write a MIDI file. A MIDI file is a list of timed events.
# ------------------------------------------------------------------------------
def var_len(value):
    """MIDI stores times as 'variable length' numbers: 7 bits per byte, and the
    top bit says 'more bytes follow'. Small numbers take only one byte."""
    out = [value & 0x7F]
    value >>= 7
    while value:
        out.append((value & 0x7F) | 0x80)
        value >>= 7
    return bytes(reversed(out))


def midi_chunk(kind, data):
    return kind + struct.pack('>I', len(data)) + data


def write_midi(path, score):
    # Track 0 holds the tempo. MIDI wants "microseconds per beat".
    micros = int(60_000_000 / score['tempo'])
    tempo_track = b'\x00\xFF\x51\x03' + micros.to_bytes(3, 'big') + b'\x00\xFF\x2F\x00'
    chunks = [midi_chunk(b'MTrk', tempo_track)]

    for channel, track in enumerate(score['tracks']):
        events = [(0, bytes([0xC0 | channel, GM_PROGRAM[track['timbre']]]))]   # pick the instrument
        for pitch, start, length, vel in track['notes']:
            on = int(start * TICKS_PER_BEAT)
            off = int((start + length) * TICKS_PER_BEAT)
            events.append((on, bytes([0x90 | channel, pitch, vel])))    # note ON
            events.append((off, bytes([0x80 | channel, pitch, 0])))     # note OFF
        # At the same tick, note OFF (0x80) must come before note ON (0x90), or a repeated
        # note would be cut off the moment it starts.
        events.sort(key=lambda e: (e[0], (e[1][0] & 0xF0) == 0x90))

        data = b''
        last = 0
        for tick, message in events:
            data += var_len(tick - last) + message    # time since the previous event
            last = tick
        data += b'\x00\xFF\x2F\x00'                   # end of track
        chunks.append(midi_chunk(b'MTrk', data))

    header = midi_chunk(b'MThd', struct.pack('>HHH', 1, len(chunks), TICKS_PER_BEAT))
    with open(path, 'wb') as f:
        f.write(header + b''.join(chunks))


# ------------------------------------------------------------------------------
# Part 3: Render the score to audio with a tiny synthesizer.
# Every note = a few sine waves (harmonics) shaped by an ENVELOPE.
# ------------------------------------------------------------------------------
# harmonics: relative loudness of the 1st, 2nd, 3rd... sine wave (this IS the "timbre")
# attack/release: seconds to fade in / fade out    decay: pluck dies away quickly
TIMBRES = {
    'pad':   {'harmonics': [1.0, 0.4, 0.15], 'attack': 0.50, 'release': 0.90, 'decay': 0.0, 'gain': 0.30},
    'pluck': {'harmonics': [1.0, 0.5, 0.25, 0.1], 'attack': 0.005, 'release': 0.15, 'decay': 3.0, 'gain': 0.35},
    'bass':  {'harmonics': [1.0, 0.3], 'attack': 0.02, 'release': 0.20, 'decay': 0.8, 'gain': 0.45},
}


def render(score):
    seconds_per_beat = 60.0 / score['tempo']
    end_beat = max(s + l for t in score['tracks'] for _p, s, l, _v in t['notes'])
    tail = 1.5                                       # room for the last release
    total = int((end_beat * seconds_per_beat + tail) * SAMPLE_RATE)
    mix = [0.0] * total

    for track in score['tracks']:
        tb = TIMBRES[track['timbre']]
        for pitch, start, length, vel in track['notes']:
            freq = 440.0 * 2 ** ((pitch - 69) / 12)            # note number -> Hz
            begin = int(start * seconds_per_beat * SAMPLE_RATE)
            held = length * seconds_per_beat                   # seconds the key is down
            n = int((held + tb['release']) * SAMPLE_RATE)
            amp = tb['gain'] * vel / 127.0

            for i in range(n):
                if begin + i >= total:
                    break
                t = i / SAMPLE_RATE
                # Envelope: fade in (attack), optional decay, fade out after the key is released
                env = min(1.0, t / tb['attack'])
                if tb['decay']:
                    env *= math.exp(-tb['decay'] * t)
                if t > held:
                    env *= max(0.0, 1.0 - (t - held) / tb['release'])
                # Add up the harmonics: that is the whole instrument
                wave_sum = 0.0
                for h, level in enumerate(tb['harmonics'], start=1):
                    wave_sum += level * math.sin(2 * math.pi * freq * h * t)
                mix[begin + i] += amp * env * wave_sum

    # Normalize so the loudest sample sits at 90% of full scale (no clipping).
    peak = max(abs(x) for x in mix) or 1.0
    return [x / peak * 0.9 for x in mix]


def write_wav(path, samples):
    pcm = b''.join(struct.pack('<h', int(max(-1.0, min(1.0, x)) * 32767)) for x in samples)
    with wave.open(path, 'wb') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(SAMPLE_RATE)
        w.writeframes(pcm)


# ------------------------------------------------------------------------------
# Main
# ------------------------------------------------------------------------------
if __name__ == '__main__':
    with open(os.path.join(HERE, 'audiobook', 'episode.json'), encoding='utf-8') as f:
        episode = json.load(f)
    os.makedirs(CACHE_DIR, exist_ok=True)

    print('========================================')
    print('Audiobook music')
    print('========================================')

    score = None
    api_key = os.environ.get('OPENROUTER_API_KEY')
    if '--offline' in sys.argv or not api_key:
        print('Using the built-in score (no OPENROUTER_API_KEY, or --offline).')
    else:
        print(f'Asking {MODEL} to compose...')
        raw_score = ask_ai_for_score(episode['music_brief'], api_key)
        if raw_score is not None:
            try:
                cache_holder = dict(raw_score)
                score = clean_score(raw_score)
                save_score_cache(cache_holder)
                print('  AI score accepted.')
            except (ValueError, KeyError, TypeError, AttributeError, OverflowError) as err:
                print(f'  AI score rejected ({err}).')

    if score is None:
        score = clean_score(episode['fallback_score'])     # Fail gracefully

    notes = sum(len(t['notes']) for t in score['tracks'])
    print(f"Score: {score['tempo']} BPM, {len(score['tracks'])} tracks, {notes} notes")

    os.makedirs(OUT_DIR, exist_ok=True)
    write_midi(os.path.join(OUT_DIR, 'theme.mid'), score)
    print('Rendering audio (pure Python, a few seconds)...')
    samples = render(score)
    write_wav(os.path.join(OUT_DIR, 'theme.wav'), samples)

    print('')
    print(f'Success! theme.wav is {len(samples) / SAMPLE_RATE:.1f} seconds long.')
    print(f'  mpv {OUT_DIR}/theme.wav')
    print(f'  (theme.mid opens in any MIDI player or notation app)')
    print('')
    print('Next: python3 22_audiobook_mix.py')
