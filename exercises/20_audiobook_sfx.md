# Exercise 20: Audiobook Sound Effects (ElevenLabs)

[Previous: Voices](19_audiobook_voices.md) | [Index](README.md) | [Next: Music](21_audiobook_music.md)

---

## What You Learned

- You can **describe** a sound and an AI creates it
- A second auth style: the `xi-api-key` header
- Asking for raw PCM (`output_format=pcm_24000`) so no converter is needed
- A hard `MAX_CALLS` budget and **stop on the first error** (never retry-loop on HTTP 429)

Code: [`python/20_audiobook_sfx.py`](../python/20_audiobook_sfx.py)

---

## Exercise 1: Prompt Craft (Apply)

Edit the `prompt` of one effect in `episode.json`. Compare:
- `"a door"`
- `"heavy wooden door creaking open slowly in a stone hallway, echo"`

Which is better, and why? (Every new prompt is a new paid call. Change one at a time.)

## Exercise 2: Napkin Math (Understand)

Before running anything, write this down: *calls x seconds x runs = total seconds requested*. Check your ElevenLabs plan for the credit price per second, and compute the cost of your episode.

## Exercise 3: The Budget Guard (Analyze)

Set `MAX_CALLS = 1` and run. What happens, and **when** (before or after any network call)? Why is that the right moment to stop?

## Exercise 4: Sound Design (Create)

Add a **third** effect to `episode.json` (for example, footsteps on a metal stair). Then change lesson 22 to place it between the narration and the first effect. *Hint: lesson 22 already shows the pattern.*

---

## Reflection

1. Why did we ask for 24 kHz PCM instead of MP3?
2. Real film sound is often recorded by people ("Foley"). What would be lost if every effect came from a prompt?
