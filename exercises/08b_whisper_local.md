# Exercise 8b: Whisper on Your Own Computer

[Previous: Whisper Audio](08_whisper.md) | [Index](README.md)

---

## What You Learned

In the basic example, you learned how to:
- Run a speech-to-text model **locally** with `faster-whisper`, with no API key and $0 cost
- Pick a model size (`tiny.en`, `base.en`, `small.en`) and trade accuracy for speed
- Read **segments** with start and end times
- Measure the real-time factor (RTF) of a model on your own hardware

Code: [`python/08b_whisper_local.py`](../python/08b_whisper_local.py). This is the one Python example that needs `pip install` (use a virtual environment, see the file header).

---

## Exercise 1: Size vs Accuracy (Analyze)

Run the same file with three models:

```
python3 08b_whisper_local.py tiny.en
python3 08b_whisper_local.py base.en
python3 08b_whisper_local.py small.en
```

Fill in a table: model, load time, transcription time, RTF, and every word that differs from the real Gettysburg Address ("...brought forth **on this continent**...").

**Questions**:
1. Which model first gets every word right?
2. Is the biggest model always worth its extra time?

## Exercise 2: Local vs Hosted (Analyze)

Run example 8 (hosted) and example 8b on the same file.

1. Compare the text, the speed, and the cost.
2. Make a rule for yourself: when would you pick each one? (Think about a private family recording, a 3-hour podcast, and a phone app with 10,000 users.)

## Exercise 3: Break It (Apply)

Record yourself (phone voice memo is fine) in three places: a quiet room, next to a fan, and with music playing. Transcribe each with `base.en`.

**Task**: Count the wrong words in each. Which noise hurts most?

## Exercise 4: Subtitles (Create)

Segments have start and end times, which is exactly what subtitle files need. Change the script to write an `.srt` file:

```
1
00:00:00,000 --> 00:00:05,600
Four score and seven years ago, ...
```

*Hint: you need a function that turns seconds like `5.6` into `00:00:05,600`.*

## Exercise 5: Check the Audio Drama (Create)

Transcribe `python/audiobook/out/episode.wav` from [lesson 22](22_audiobook_mix.md) and compare it with the lines in `python/audiobook/episode.json`.

1. Did the AI voices say every word you wrote?
2. Where does the music make transcription harder?
3. Write a small checker that prints the percentage of script words found in the transcript.

---

## Reflection

1. If the model is free and private, why does anyone still pay for a hosted one?
2. Whisper sometimes "hears" words that were never said when the audio is silent. How could you detect that?
