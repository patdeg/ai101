# Exercise 19: Audiobook Voices (Gemini TTS)

[Previous: Pose Estimation](18_pose_estimation.md) | [Index](README.md) | [Next: Sound Effects](20_audiobook_sfx.md)

---

## What You Learned

- One TTS model, two modes: a single narrator, and **two characters in one request**
- Plain-English style directions ("say in a slow, quiet voice") change the performance
- Audio arrives as base64 PCM; the `wave` module turns it into a playable file
- A hash-based cache means a re-run costs nothing

Code: [`python/19_audiobook_voices.py`](../python/19_audiobook_voices.py)

---

## Exercise 1: Casting (Understand)

Change the voice names in `python/audiobook/episode.json` (Gemini offers 30 prebuilt voices; see the speech-generation page in Google's docs). Re-run and listen.

1. Which voice pair has the best contrast?
2. Does a voice change how much you trust the character?

## Exercise 2: Direction (Apply)

Edit the sentence that starts `Say in a slow, quiet...`. Try *whispering*, *excited*, *tired*, *like a pirate*.

**Task**: Find the direction that makes the narration scariest, and write it down.

## Exercise 3: Your Own Scene (Apply)

Replace the dialogue in `episode.json` with a 6-line scene of your own (keep exactly **two** names).

**Requirement**: Every line must move the story. No line may only say "hello".

## Exercise 4: Cost Detective (Analyze)

Run once, then run again. How many API calls did the second run make? Open `audiobook/out/cache/` and explain in two sentences how the program knew it could skip the call.

## Exercise 5: Three Speakers (Create)

The API allows only **two** speakers per request. Design (on paper, or in code) a way to produce a 3-person scene anyway. *Hint: you already know how to make two requests and join the results in lesson 22.*

---

## Reflection

1. Why is the cache key a hash of the model **and** the request, not just the text?
2. A voice actor can improvise. What can an AI voice not do yet?
