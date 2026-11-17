# Exercise 21: Audiobook Music (AI score + MIDI + synth)

[Previous: Sound Effects](20_audiobook_sfx.md) | [Index](README.md) | [Next: Mix](22_audiobook_mix.md)

---

## What You Learned

- An AI composes; plain code renders. Each does what it is best at
- **Never trust AI output**: `clean_score()` checks and clamps every number
- A MIDI file is instructions (what note, when, how hard), not sound
- A synthesizer is sine waves plus an envelope (attack and release)
- Fall back gracefully: no key or a bad answer still gives you music

Code: [`python/21_audiobook_music.py`](../python/21_audiobook_music.py)

---

## Exercise 1: Listen to the Instructions (Understand)

Run with `--offline`, then open `theme.mid` in a MIDI player (or MuseScore) and compare it with `theme.wav`. Same notes, different instruments. Why?

## Exercise 2: Change the Sound (Apply)

In `TIMBRES`, change the `harmonics` list of `pad` to `[1.0]` (one pure sine wave). Re-run. Then try `[1.0, 1.0, 1.0, 1.0]`. What did the harmonics control?

## Exercise 3: Brief the Composer (Apply)

Edit `music_brief` in `episode.json` (for example: *a fast, tense chase*). Run with your key. Did the AI follow the brief? Look at the tempo and the notes it chose.

## Exercise 4: Break It on Purpose (Analyze)

Edit `episode.json`'s fallback score so one track says `"timbre": "kazoo"`. What happens (you will see a traceback)? Which function raised it? Now find where the AI path catches that same error and falls back to the built-in score. Why must code that uses AI output catch it?

## Exercise 5: A Fourth Instrument (Create)

Add a `bell` timbre: high notes, very short attack, a long decay. You need an entry in `TIMBRES` and in `GM_PROGRAM` (try General MIDI program 9, glockenspiel). Add a few bell notes to the score.

---

## Reflection

1. Why is the MIDI tempo stored as microseconds per beat?
2. Is a human composer who uses this tool still the composer? Who owns the music?
