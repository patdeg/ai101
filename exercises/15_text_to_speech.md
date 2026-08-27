# Exercise 14: Text-to-Speech Experiments

[← Previous: Reasoning](13_reasoning.md) | [Back to Exercises](README.md)

## Overview

These exercises explore **Fish Audio S2.1 Pro Free**, called through **OpenRouter's**
`/audio/speech` endpoint - a genuinely free text-to-speech model (no card required) that
picks voices via **voice-cloning reference IDs** instead of a fixed list of named
presets, and directs delivery style with **inline bracketed tags in the text itself**
(like `[excited]` or `[whispering]`) instead of a separate `instructions` parameter.

**Key Learning Objectives:**
- Building a personal voice library from Fish Audio's reference IDs
- Directing emotion/delivery with inline style tags instead of an API parameter
- Using `prosody` (speed) and format options for different use cases
- Understanding free-tier tradeoffs (rate limits, no SLA) vs. paid TTS
- Building real-world accessible applications

**Pricing:** `fish-audio/s2.1-pro-free:free` costs **$0** - rate-limited, best-effort
availability, meant for learning and prototyping rather than production traffic.

---

## Exercise 14.1: Build Your Own Voice Library

**Goal:** Since Fish Audio has no fixed named-voice list like `alloy` or `nova`, the
first skill is building your own reference-ID library by browsing and auditioning
voices.

### Task:

1. Create a free account at https://fish.audio
2. Browse the voice library and audition at least 8 voices across different
   characters: neutral, warm/friendly, deep/authoritative, energetic/youthful,
   calm/measured, narrative/storytelling
3. For each one you like, copy its reference ID and give it a short label, e.g.:

```python
my_voice_library = {
    "narrator":      "abc123...",   # deep, measured - good for stories
    "assistant":     "def456...",   # warm, friendly - good for chat
    "announcer":     "ghi789...",   # energetic - good for ads/headlines
    "calm_guide":    "jkl012...",   # slow, soothing - good for meditation
}
```

4. Generate the same sentence with each voice in your library:

```python
contexts = [
    {
        "name": "bedtime_story",
        "text": "Once upon a time, in a cozy cottage deep in the enchanted forest, there lived a curious little fox named Luna.",
        "try_voices": ["narrator", "calm_guide"]
    },
    {
        "name": "news_headline",
        "text": "Breaking news: Scientists have discovered a new method for clean energy production that could revolutionize the industry.",
        "try_voices": ["announcer", "narrator"]
    },
    {
        "name": "product_ad",
        "text": "Introducing our latest innovation - a smartwatch that combines style, performance, and sustainability like never before!",
        "try_voices": ["announcer", "assistant"]
    },
    {
        "name": "meditation_guide",
        "text": "Take a deep breath in... and slowly exhale. Feel your body relax as you let go of all tension.",
        "try_voices": ["calm_guide"]
    },
]

# For each context, generate audio with your suggested voices
# Listen and rank them by appropriateness
```

### Reflection Questions:

1. How is picking a voice by audition different from picking one by name/description?
2. Which of your voices are most versatile (work well across multiple contexts)?
3. Which are most specialized (excel in one scenario only)?
4. What information would you want to see on a voice's page that you don't get today?

---

## Exercise 14.2: Directing Delivery with Inline Style Tags

**Goal:** Fish Audio has no separate `instructions` parameter - style direction lives
**inside the text itself** as bracketed tags (e.g. `[excited]`, `[whispering]`,
`[sighs]`). Learn what works and what doesn't.

### Task 1: Emotional Variations

Generate the same sentence with different inline style tags and compare:

```python
text_variants = [
    {
        "name": "urgent",
        "text": "[urgent] The test results are in, and we need to discuss them immediately.",
    },
    {
        "name": "reassuring",
        "text": "[calm] The test results are in, and we need to discuss them immediately.",
    },
    {
        "name": "excited",
        "text": "[excited] The test results are in, and we need to discuss them immediately!",
    },
    {
        "name": "neutral",
        "text": "The test results are in, and we need to discuss them immediately.",
    },
]

# Generate audio for each variant with the same voice
# Notice how the SAME words convey different meanings
```

### Task 2: Pacing and Emphasis

Since there's no `instructions` field, experiment with punctuation, ellipses, and
capitalization to influence pacing within the text itself:

```python
text_variants = [
    "Welcome to today's presentation. We'll cover three key topics: innovation, collaboration, and growth.",
    "Welcome to today's presentation... we'll cover three key topics... innovation... collaboration... and growth.",
    "[fast] Welcome to today's presentation! We'll cover three key topics: innovation, collaboration, and growth!",
    "Welcome to today's presentation. We'll cover THREE key topics: innovation, collaboration, and growth.",
]

# Try each variant with different voices from your library
# Which techniques actually change the delivery, and which don't?
```

### Task 3: Character Voices

Combine a chosen voice with inline tags for storytelling:

```python
characters = [
    {
        "name": "wise_mentor",
        "voice": "narrator",
        "text": "[slow, warm] Listen carefully, young one. The path ahead requires both courage and wisdom.",
    },
    {
        "name": "enthusiastic_child",
        "voice": "assistant",
        "text": "[excited, fast] Can we go to the park? Please, please, please? I promise I'll be good!",
    },
    {
        "name": "mysterious_stranger",
        "voice": "announcer",
        "text": "[whispering] I've been waiting for you. I have information that will change everything.",
    },
]

# Generate audio for each character
# Could you build an audiobook generator with this?
```

### Reflection Questions:

1. Which bracketed tags actually changed the delivery, and which had no effect?
2. Is directing style through the text itself easier or harder than a dedicated
   parameter? What are the tradeoffs?
3. Are some voices more responsive to style tags than others?
4. How would inline tags interact with multi-speaker dialogue (Example 16)?

---

## Exercise 14.3: Prosody and Format Applications

**Goal:** Learn Fish Audio's `prosody` controls (speed) and output format options -
check the exact parameter shape at https://openrouter.ai/fish-audio/s2.1-pro-free:free
before relying on precise values, since provider-passthrough parameters can change.

### Task 1: Language Learning Application

Build a vocabulary trainer that requests a few different speeds via `prosody.speed`:

```python
vocabulary_lesson = {
    "word": "serendipity",
    "pronunciation": "ser-en-dip-i-ty",
    "definition": "The occurrence of events by chance in a happy or beneficial way.",
    "example": "Finding that old photo album in the attic was pure serendipity."
}

text = f"{vocabulary_lesson['word']}: {vocabulary_lesson['definition']}. Example: {vocabulary_lesson['example']}"

# Try a few prosody.speed values (e.g. slower than 1.0, 1.0, faster than 1.0) -
# find where the API's practical range actually starts sounding wrong
for speed in [0.7, 1.0, 1.3]:
    # Body: {"model": "fish-audio/s2.1-pro-free:free", "input": text,
    #        "providerOptions": {"fishAudio": {"referenceId": my_voice_library["narrator"],
    #                                            "prosody": {"speed": speed}}}}
    pass
```

### Task 2: Accessibility Features

Create different speed/voice pairings for different needs:

```python
content = """
Climate change refers to long-term shifts in temperatures and weather patterns.
These shifts may be natural, but since the 1800s, human activities have been
the main driver of climate change, primarily due to the burning of fossil fuels.
"""

accessibility_profiles = [
    {"name": "visual_impairment", "speed": 0.9, "voice": "assistant"},
    {"name": "cognitive_support", "speed": 0.7, "voice": "calm_guide"},
    {"name": "fast_scanning",     "speed": 1.3, "voice": "assistant"},
    {"name": "standard",          "speed": 1.0, "voice": "assistant"},
]

# Generate audio for each profile
# Test: which speeds actually stay intelligible, and where does it break down?
```

### Task 3: Format Comparison

```python
text = "The quick brown fox jumps over the lazy dog."

formats = ["mp3", "opus", "aac", "flac", "wav", "pcm"]

for fmt in formats:
    # response_format = fmt
    # Compare file sizes
    pass

# Questions:
# - Which format has the smallest file size?
# - Which loads fastest for a web/mobile use case?
```

### Reflection Questions:

1. What's the practical speed range before intelligibility breaks down?
2. How does speed interact with the voice you picked?
3. How could speed control improve accessibility?
4. Which format would you pick for a podcast vs. a voice assistant vs. archival?

---

## Exercise 14.4: Multilingual Voices

**Goal:** Fish Audio S2.1 Pro is multilingual - check its current language coverage at
https://fish.audio (or the model's OpenRouter page) rather than assuming a fixed list,
since supported languages can change between models/releases.

### Task 1: Language Learning Companion

Build a phrase trainer for multiple languages:

```python
common_phrases = [
    {
        "english": "Hello, how are you?",
        "spanish": "Hola, ¿cómo estás?",
        "french": "Bonjour, comment allez-vous?",
        "german": "Hallo, wie geht es dir?",
        "japanese": "こんにちは、お元気ですか？",
        "chinese": "你好，你好吗？"
    },
    {
        "english": "Thank you very much.",
        "spanish": "Muchas gracias.",
        "french": "Merci beaucoup.",
        "german": "Vielen Dank.",
        "japanese": "どうもありがとうございます。",
        "chinese": "非常感谢。"
    }
]

# For each phrase:
# 1. Generate audio in each language, same voice
# 2. Generate at a slower prosody.speed for learning, normal speed for practice
# 3. Build a simple language-learning audio library
```

### Task 2: Multilingual Customer Service

Create a greeting message in several languages, testing whether one voice handles
all of them acceptably or whether you need to pick per-language voices:

```python
translations = {
    "english": "Welcome to our customer service. How can I help you today?",
    "spanish": "Bienvenido a nuestro servicio al cliente. ¿Cómo puedo ayudarte hoy?",
    "french": "Bienvenue à notre service client. Comment puis-je vous aider aujourd'hui?",
    "german": "Willkommen bei unserem Kundenservice. Wie kann ich Ihnen heute helfen?",
    "chinese": "欢迎来到我们的客户服务。今天我能帮您什么忙？",
}

# Generate audio for each language with the SAME voice reference ID
# Test: does the voice hold up across languages, or does it need swapping?
```

### Reflection Questions:

1. How accurate does the pronunciation sound across languages, subjectively?
2. Do some of your voices handle certain languages better than others?
3. How would you handle mixed-language content in one request?
4. What real-world applications need multilingual TTS?

---

## Exercise 14.5: Free-Tier Tradeoffs and Scaling Beyond It

**Goal:** Understand what "free" actually costs you (rate limits, no SLA) and how
you'd plan for scaling past the free tier.

### Background

`fish-audio/s2.1-pro-free:free` is explicitly described as "no-cost... intended for
testing, prototyping, and low-volume applications... without production latency or
availability guarantees." The paid `fish-audio/s2.1-pro` model exists for production
use. This is a different tradeoff than OpenAI's `gpt-4o-mini-tts`, which is cheap
($0.60/1M input, $12/1M output tokens - duration-based) but has no free tier at all.

### Task 1: Reliability Testing

Build a small harness that calls the free model repeatedly and logs failures/latency:

```python
import time

def stress_test(text, voice_id, n_requests=20):
    results = []
    for i in range(n_requests):
        start = time.time()
        # make the request, catch errors
        # results.append({"latency": time.time() - start, "success": ..., "error": ...})
    return results

# Run this a few times. What does the failure/latency profile look like?
# Would this be acceptable for a live product, or only for prototyping?
```

### Task 2: Cost Comparison If You Outgrow Free

```python
def compare_backends(monthly_requests, avg_chars_per_request):
    """
    Compare the free tier's practical limits against paid alternatives.

    - fish-audio/s2.1-pro-free:free: $0, but rate-limited and best-effort
    - fish-audio/s2.1-pro (paid): production SLA, priced per character/second
    - openai/gpt-4o-mini-tts: no free tier, ~$0.60/1M input + $12/1M output tokens
    """
    # Look up current pricing for each option on openrouter.ai/models
    # and reason about the break-even point for your use case
    pass
```

### Reflection Questions:

1. At what point would you migrate a project off the free tier?
2. Is "free but unreliable" or "cheap but no free tier" the better fit for a
   student project vs. a real product?
3. How would you design a fallback (e.g., retry, queue, or swap providers) for a
   free-tier TTS call that fails?

---

## Exercise 14.6: Build a Real-World Application

**Goal:** Combine everything learned into a small end-to-end TTS application.

### Project Ideas:

#### Option 1: Accessible Blog Reader

```python
class BlogAudioGenerator:
    def __init__(self, voice_id):
        self.voice_id = voice_id  # from your library, Exercise 14.1
        self.format = "mp3"

    def generate_audio(self, blog_post):
        """
        Convert blog post to audio.

        Features:
        - Extract title, author, date, content
        - Generate intro ("Article title by author, published date")
        - Generate main content
        - Add outro ("End of article")
        - Combine into single file
        """
        intro = f"{blog_post['title']}, by {blog_post['author']}, published {blog_post['date']}."
        content = blog_post['content']
        outro = "End of article. Thank you for listening."

        full_text = f"{intro} {content} {outro}"

        # Generate audio via OpenRouter's /audio/speech
        # Save with blog post ID
        # Return audio file path
        pass

# Test with your blog or Medium articles
```

#### Option 2: Language Learning Flashcards

```python
class LanguageLearningAudio:
    def __init__(self, voice_id, target_language, native_language="english"):
        self.voice_id = voice_id
        self.target_language = target_language
        self.native_language = native_language

    def generate_flashcard_audio(self, word, translation, example):
        """
        Generate audio files per flashcard:
        1. Word (target language, slower prosody.speed)
        2. Translation (native language, normal speed)
        3. Example sentence (target language, slightly slow)
        4. Full flashcard (all together, normal speed)
        """
        pass

# Build a complete lesson for 20 vocabulary words
```

#### Option 3: Interactive Storytelling

```python
class InteractiveStoryAudio:
    def __init__(self, character_voices):
        # character_voices: dict mapping character name -> reference ID,
        # built from your library in Exercise 14.1
        self.character_voices = character_voices

    def generate_story_segment(self, segment):
        """
        Generate audio for a story segment.

        Features:
        - Different voice (reference ID) for each character
        - Inline bracketed tags for emotional delivery
        - Choice prompts at end of segments
        """
        audio_parts = []

        for line in segment['lines']:
            character = line['character']
            emotion = line.get('emotion', '')
            tag = f"[{emotion}] " if emotion else ""
            text = tag + line['text']
            voice_id = self.character_voices[character]

            # Generate audio with this voice + inline-tagged text
            # Add to audio_parts
            pass

        # Combine all parts, add choice prompt
        return audio_file

# Build an interactive story with branching paths
```

### Implementation Checklist:

- [ ] API key management (environment variables)
- [ ] Error handling (rate limits, free-tier failures, network errors)
- [ ] File management (organized storage)
- [ ] Caching (avoid regenerating same content - free tier is rate-limited!)
- [ ] User preferences (voice, speed, format)
- [ ] Testing (quality, reliability, performance)
- [ ] Documentation (usage guide)

### Reflection Questions:

1. Which features are essential vs. nice-to-have?
2. How do you handle a free-tier failure gracefully (retry? queue? fallback voice)?
3. How will users customize audio generation?
4. What accessibility features should you include?

---

## Real-World Applications

### 1. Accessibility
- Screen readers for visually impaired users
- Dyslexia-friendly content consumption
- Audio descriptions for images/videos
- Website narration

### 2. Education
- Language learning pronunciation
- Audiobook generation for textbooks
- Lecture summarization
- Interactive learning modules

### 3. Content Creation
- Podcast generation from articles (see Example 16 for two-speaker dialogue)
- YouTube video narration
- Social media audio posts
- Newsletter audio versions

### 4. Business
- Customer service IVR systems
- Product description audio
- Training material narration
- Multilingual support

### 5. Entertainment
- Interactive storytelling
- Character voices for games
- Audio drama production
- Personalized audio messages

---

## Further Exploration

1. **Grow your voice library:** Audition 20+ voices, build a labeled catalog
2. **Experiment with inline tags:** Try many different bracketed style cues, note
   which ones the model actually respects
3. **Test multi-speaker:** Move on to Example 16's two-speaker `<|speaker:N|>` format
4. **Stress-test the free tier:** Measure real-world reliability for your use case
5. **A/B testing:** Compare listener engagement with different voices
6. **Accessibility audit:** Test with real users who need TTS

---

## Conclusion

Fish Audio S2.1 Pro Free (via OpenRouter) is:
- ✅ **Free:** $0 on the free tier, no card required
- ✅ **Flexible voices:** any voice from Fish Audio's library, not a fixed preset list
- ✅ **Multilingual:** check current coverage at fish.audio
- ⚠️ **Best-effort:** no production SLA on the free tier - migrate to the paid model
  (or another provider) before relying on it in production

**Key Takeaway:** the free tier removes the cost barrier to experimenting with TTS,
but "free" here specifically means "not production-grade" - plan your reliability
strategy accordingly.

Now build something amazing! 🎤

---

## Related Examples

Try the example code files to see TTS in action:

- [bash/15_text_to_speech_minimal.sh](../bash/15_text_to_speech_minimal.sh)
- [bash/15_text_to_speech_full.sh](../bash/15_text_to_speech_full.sh)
- [nodejs/15_text_to_speech.js](../nodejs/15_text_to_speech.js)
- [python/15_text_to_speech.py](../python/15_text_to_speech.py)
- [go/15_text_to_speech.go](../go/15_text_to_speech.go)

---

## Related Exercises

- [Exercise 1: Basic Chat](01_basic_chat.md) - API fundamentals and token usage
- [Podcast](16_podcast.md) - the two-speaker version of this same model

[← Previous: Reasoning](13_reasoning.md) | [Back to Exercises](README.md)
