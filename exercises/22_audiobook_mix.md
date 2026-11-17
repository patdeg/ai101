# Exercise 22: Audiobook Mix

[Previous: Music](21_audiobook_music.md) | [Index](README.md)

---

## What You Learned

- A mix is adding numbers: each sample of each clip is added to the tape at the right time
- Gain is multiplication; fades prevent clicks
- Overlap (negative offsets) makes a mix sound natural
- Normalizing uses the full volume range without clipping

Code: [`python/22_audiobook_mix.py`](../python/22_audiobook_mix.py). This lesson makes **no API calls**, so experiment freely.

---

## Exercise 1: Read the Timeline (Understand)

Run it and read the printed table. At what second does the narrator start, and why not at second 0?

## Exercise 2: Balance (Apply)

Change the SFX gain from 0.7 to 1.5, then to 0.2. Listen. Why can the narration be buried by something louder than the voice, even though the voice is at 1.0?

## Exercise 3: Remove a Fade (Analyze)

Set `fade_in` and `fade_out` to 0 on the narration. Listen closely at the start and end. What do you hear? Why?

## Exercise 4: Ducking (Create)

Real mixers lower the music **while someone speaks**. Add a `duck` option to `place()` that multiplies the gain by 0.3 between two times, and use it so the outro music is quieter under the last SFX.

## Exercise 5: Your Own Episode (Create)

Write a new `episode.json` (new story, new voices, new music brief, two new effects) and rebuild the whole pipeline with lessons 19, 20, 21, 22. Share the `episode.wav`.

---

## Reflection

1. We only used one channel (mono). What would stereo let you do with the two characters?
2. Which of the five sounds in your episode did you trust the AI with, and which did you check by ear? Why?
