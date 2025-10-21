# Exercise 7: Whisper - Audio Transcription

[← Previous: Prompt Guard](06_prompt_guard.md) | [Back to Exercises](README.md)

## What You Learned

- Speech-to-text API
- Audio encoding
- Whisper model capabilities
- Audio file handling

## Exercise 7.1: Audio Quality Degradation Test

Record the same sentence and save at different quality levels:
- 48kHz, 320kbps (studio quality)
- 44.1kHz, 128kbps (CD quality)
- 22kHz, 64kbps (AM radio quality)
- 16kHz, 32kbps (phone quality)
- 8kHz, 16kbps (very low quality)

**What to observe:** At what quality does transcription accuracy drop? Where's the cost/quality sweet spot?

**Tools needed:** Audacity, ffmpeg, or online audio converters

## Exercise 7.2: Language Testing

Record or find audio in different languages:
- English (baseline)
- Spanish, French, German (common languages)
- Mandarin, Arabic, Hindi (non-European)
- Mixed language in same audio

**Optional parameter:** Add `"language": "es"` (ISO-639-1 code) to the request

**What to learn:** How well does Whisper handle different languages? Does specifying language improve accuracy?

## Exercise 7.3: Accent and Dialect Testing

Test the same text spoken with different accents:
- American English
- British English
- Indian English
- Australian English
- Non-native speaker English

**What to observe:** Does accent affect accuracy? Which are easiest/hardest to transcribe?

## Exercise 7.4: Background Noise Challenge

Record speech with increasing background noise:
- Silent room (clean audio)
- Light background music
- Office/café ambience
- Traffic noise
- Very noisy environment

**What to learn:** Noise tolerance limits. When does transcription fail?

## Exercise 7.5: Temperature Parameter Experiment

Use the `"temperature"` parameter in Whisper API:
- Temperature 0.0 (most confident/deterministic)
- Temperature 0.2 (slightly more flexible)
- Temperature 0.4 to 1.0 (more alternatives)

**What to observe:** Does temperature affect transcription quality? When might higher temperature be useful (unclear audio)?

## Exercise 7.6: Long-Form Audio

Test transcription limits:
- 30 seconds (short)
- 2 minutes (medium)
- 10 minutes (long)
- 30+ minutes (very long)

**What to learn:**
- Cost scaling ($0.04 per hour)
- Accuracy over time
- Processing time limits

**Calculate:** If you transcribe a 1-hour podcast, what's the cost? (Answer: $0.04)

## Exercise 7.7: Timestamp and Punctuation Analysis

Examine the transcription output:
- Are there timestamps available?
- How accurate is punctuation?
- Are speaker changes detected?
- Are filler words ("um", "uh") included or removed?

**What to learn:** Understanding Whisper's output format and capabilities beyond raw text.

## Exercise 7.8: Multi-Step Audio Pipeline

Combine Whisper with other examples:

1. **Audio → Transcription → Safety Check:**
   - Transcribe audio with Whisper
   - Check transcript with LlamaGuard ([Exercise 4](04_safety_text.md))
   - Flag inappropriate content

2. **Audio → Transcription → AI Response:**
   - Transcribe user's voice question
   - Send transcript to AI ([Exercise 1](01_basic_chat.md))
   - Get text response (or use text-to-speech to respond)

3. **Audio → Transcription → Sentiment Analysis:**
   - Transcribe audio
   - Use AI to analyze sentiment/emotion ([Exercise 2](02_system_prompt.md))
   - Track conversation mood

**What to learn:** Building complete voice-enabled applications.

## Exercise 7.9: Cost Optimization

Calculate costs for different use cases:

**Podcast transcription service:**
- Average podcast: 45 minutes
- Cost per episode: $0.03
- 1000 episodes/month: $30/month

**Voice assistant:**
- Average query: 5 seconds
- Cost per query: $0.00006
- 10,000 queries/day: $0.60/day = $18/month

**Meeting transcription:**
- Average meeting: 1 hour
- Cost per meeting: $0.04
- 50 meetings/week: $2/week = $8/month

**What to learn:** Whisper is extremely cost-effective for speech-to-text.

## Reflection Questions

After completing these exercises, consider:

1. What audio quality is "good enough" for your use case?
2. How does Whisper compare to other transcription services?
3. When would you combine Whisper with other AI models?
4. What are the cost implications for high-volume applications?

## Next Steps

Congratulations! You've completed all 7 exercise sets. Now you can:

1. **Build a complete application** combining multiple examples
2. **Explore the Arduino examples** for embedded AI ([arduino/README.md](../arduino/README.md))
3. **Create your own exercises** and share them
4. **Contribute improvements** to this repository

---

**Related Examples:**
- [bash/07_whisper_minimal.sh](../bash/07_whisper_minimal.sh)
- [bash/07_whisper_full.sh](../bash/07_whisper_full.sh)
- [nodejs/07_whisper.js](../nodejs/07_whisper.js)
- [python/07_whisper.py](../python/07_whisper.py)
- [go/07_whisper.go](../go/07_whisper.go)
- [arduino/07_whisper/07_whisper.ino](../arduino/07_whisper/07_whisper.ino)

**See also:**
- [Exercise 1: Basic Chat](01_basic_chat.md) - Use transcripts as AI input
- [Exercise 4: Safety Check](04_safety_text.md) - Moderate transcribed content

[← Previous: Prompt Guard](06_prompt_guard.md) | [Back to Exercises](README.md)
