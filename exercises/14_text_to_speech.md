# Exercise 14: Text-to-Speech Experiments

[← Previous: Reasoning](13_reasoning.md) | [Back to Exercises](README.md)

## Overview

These exercises explore OpenAI's text-to-speech API (gpt-4o-mini-tts) - an affordable, high-quality TTS solution with 11 distinct voices, customizable instructions, speed control, and multi-language support.

**Key Learning Objectives:**
- Understanding voice characteristics and selection criteria
- Using the instructions parameter for delivery customization
- Controlling speed and format for different use cases
- Optimizing costs for TTS applications
- Building real-world accessible applications

**Pricing:** $0.60/1M input tokens, $12/1M output tokens (audio duration-based)

---

## Exercise 14.1: Voice Personality Lab

**Goal:** Discover which voices work best for different contexts by generating the same text with all 11 voices.

### The 11 Voices:

- **alloy** - Neutral, balanced (general-purpose)
- **ash** - Clear, articulate (technical content)
- **ballad** - Smooth, expressive (emotional content)
- **coral** - Warm, friendly (conversational)
- **echo** - Clear, professional (business)
- **fable** - Narrative, storytelling (fiction)
- **onyx** - Deep, authoritative (announcements)
- **nova** - Energetic, youthful (dynamic content)
- **sage** - Calm, measured (meditation, education)
- **shimmer** - Bright, cheerful (upbeat content)
- **verse** - Poetic, expressive (creative writing)

### Task:

Generate audio for these 5 different contexts using appropriate voices:

```python
contexts = [
    {
        "name": "bedtime_story",
        "text": "Once upon a time, in a cozy cottage deep in the enchanted forest, there lived a curious little fox named Luna.",
        "best_voices": ["fable", "ballad", "verse"]
    },
    {
        "name": "news_headline",
        "text": "Breaking news: Scientists have discovered a new method for clean energy production that could revolutionize the industry.",
        "best_voices": ["echo", "onyx", "sage"]
    },
    {
        "name": "product_ad",
        "text": "Introducing our latest innovation - a smartwatch that combines style, performance, and sustainability like never before!",
        "best_voices": ["nova", "shimmer", "coral"]
    },
    {
        "name": "meditation_guide",
        "text": "Take a deep breath in... and slowly exhale. Feel your body relax as you let go of all tension.",
        "best_voices": ["sage", "ballad", "ash"]
    },
    {
        "name": "technical_tutorial",
        "text": "First, open the terminal and navigate to your project directory. Then, run the install command to set up dependencies.",
        "best_voices": ["ash", "echo", "alloy"]
    }
]

# For each context, generate audio with all suggested voices
# Listen and rank them by appropriateness
```

### Reflection Questions:

1. Did your voice preferences match the suggestions? Why or why not?
2. Which voices are most versatile (work well across multiple contexts)?
3. Which voices are most specialized (excel in specific scenarios)?
4. How does voice selection impact listener engagement?

---

## Exercise 14.2: Instructions Parameter Deep Dive

**Goal:** Master the instructions parameter to customize voice delivery beyond default behavior.

### What Are Instructions?

Instructions guide the model on **HOW** to speak the text - tone, pacing, emphasis, emotion, style.

**Important:** Instructions affect delivery, not content. The text stays the same.

### Task 1: Emotional Variations

Generate the same sentence with different emotional instructions:

```python
text = "The test results are in, and we need to discuss them immediately."

instructions_set = [
    {
        "name": "urgent",
        "voice": "echo",
        "instructions": "Speak with urgency and concern, as if delivering critical news."
    },
    {
        "name": "reassuring",
        "voice": "coral",
        "instructions": "Speak in a calm, reassuring tone to reduce anxiety."
    },
    {
        "name": "excited",
        "voice": "nova",
        "instructions": "Speak with excitement and enthusiasm, as if sharing good news."
    },
    {
        "name": "neutral",
        "voice": "alloy",
        "instructions": "Speak in a neutral, matter-of-fact tone without emotion."
    }
]

# Generate audio for each instruction set
# Notice how the SAME words convey different meanings
```

### Task 2: Pacing and Emphasis

Experiment with delivery style instructions:

```python
text = "Welcome to today's presentation. We'll cover three key topics: innovation, collaboration, and growth."

instructions_examples = [
    "Speak slowly and deliberately, pausing after each topic for emphasis.",
    "Speak quickly and energetically, as if addressing a large excited audience.",
    "Speak conversationally, as if talking to a small group of colleagues.",
    "Speak with theatrical flair, emphasizing each of the three key words dramatically."
]

# Try each instruction with different voices
# Which combinations work best?
```

### Task 3: Character Voices

Create distinct character voices for storytelling:

```python
characters = [
    {
        "name": "wise_mentor",
        "voice": "sage",
        "text": "Listen carefully, young one. The path ahead requires both courage and wisdom.",
        "instructions": "Speak as a wise, elderly mentor - slowly, with gravitas and warmth."
    },
    {
        "name": "enthusiastic_child",
        "voice": "shimmer",
        "text": "Can we go to the park? Please, please, please? I promise I'll be good!",
        "instructions": "Speak as an excited child - fast-paced, high energy, with pleading tone."
    },
    {
        "name": "mysterious_stranger",
        "voice": "onyx",
        "text": "I've been waiting for you. I have information that will change everything.",
        "instructions": "Speak mysteriously and quietly, with dramatic pauses and a secretive tone."
    }
]

# Generate audio for each character
# Could you build an audiobook generator with this?
```

### Reflection Questions:

1. How much can instructions change the meaning of text?
2. What types of instructions produce the strongest effects?
3. Are some voices more responsive to instructions than others?
4. How would you use instructions for accessibility (e.g., clearer speech)?

---

## Exercise 14.3: Speed Control Applications

**Goal:** Learn when and how to use speed variations (0.25x to 4.0x).

### Speed Range:

- **0.25x - 0.75x:** Slower (accessibility, learning, emphasis)
- **1.0x:** Normal (default)
- **1.25x - 2.0x:** Faster (summaries, time-saving)
- **2.5x - 4.0x:** Very fast (previews, skimming)

### Task 1: Language Learning Application

Build a vocabulary trainer with 3 speed levels:

```python
vocabulary_lesson = {
    "word": "serendipity",
    "pronunciation": "ser-en-dip-i-ty",
    "definition": "The occurrence of events by chance in a happy or beneficial way.",
    "example": "Finding that old photo album in the attic was pure serendipity."
}

# Generate the example sentence at:
# 0.5x speed - slow, clear enunciation for learning
# 1.0x speed - normal conversation
# 1.5x speed - native speaker pace

text = f"{vocabulary_lesson['word']}: {vocabulary_lesson['definition']}. Example: {vocabulary_lesson['example']}"

for speed in [0.5, 1.0, 1.5]:
    # Generate audio with 'ash' voice (clear, articulate)
    # Add speed parameter
    pass
```

### Task 2: Accessibility Features

Create different speed options for different needs:

```python
content = """
Climate change refers to long-term shifts in temperatures and weather patterns.
These shifts may be natural, but since the 1800s, human activities have been
the main driver of climate change, primarily due to the burning of fossil fuels.
"""

accessibility_profiles = [
    {"name": "visual_impairment", "speed": 0.9, "voice": "echo"},  # Slightly slower, clear
    {"name": "cognitive_support", "speed": 0.7, "voice": "sage"},  # Slower, calm
    {"name": "fast_scanning", "speed": 1.5, "voice": "ash"},      # Faster for quick review
    {"name": "standard", "speed": 1.0, "voice": "alloy"}          # Default
]

# Generate audio for each profile
# Test: Which speeds work best for different accessibility needs?
```

### Task 3: Content Summarization

Use speed to differentiate between full content and summaries:

```python
full_article = """
[Long article about AI development - 500 words]
"""

summary = """
[3-sentence summary of the article]
"""

# Generate:
# 1. Full article at 1.0x (normal reading)
# 2. Summary at 1.2x (slightly faster to signal it's a preview)
# 3. Ultra-fast 2.0x version for quick scanning

# Does speed help signal different content types?
```

### Reflection Questions:

1. What's the optimal speed for different use cases?
2. How does speed interact with voice choice?
3. At what speed does comprehension start to degrade?
4. How could speed control improve accessibility?

---

## Exercise 14.4: Multi-Language Support

**Goal:** Understand TTS automatic language detection and build multilingual applications.

### Supported Languages (50+):

English, Spanish, French, German, Italian, Portuguese, Dutch, Russian, Arabic, Japanese, Korean, Chinese (Simplified & Traditional), Hindi, Turkish, Polish, Ukrainian, Romanian, Swedish, Danish, Norwegian, Finnish, Greek, Czech, Hebrew, Thai, Vietnamese, Indonesian, Malay, Filipino, and many more.

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
# 1. Generate audio in all languages
# 2. Use different voices for variety
# 3. Generate at 0.8x speed for learning
# 4. Generate at 1.0x speed for practice

# Build a simple language learning audio library
```

### Task 2: Multilingual Customer Service

Create a greeting message in 5 languages:

```python
greeting = "Welcome to our customer service. How can I help you today?"

translations = {
    "english": ("Welcome to our customer service. How can I help you today?", "coral"),
    "spanish": ("Bienvenido a nuestro servicio al cliente. ¿Cómo puedo ayudarte hoy?", "coral"),
    "french": ("Bienvenue à notre service client. Comment puis-je vous aider aujourd'hui?", "ballad"),
    "german": ("Willkommen bei unserem Kundenservice. Wie kann ich Ihnen heute helfen?", "echo"),
    "chinese": ("欢迎来到我们的客户服务。今天我能帮您什么忙？", "alloy")
}

# Generate audio files for each language
# Test: Does the TTS accurately pronounce each language?
```

### Task 3: Pronunciation Comparison

Compare pronunciation across languages:

```python
# Words that exist in multiple languages but sound different
words = [
    {
        "word": "restaurant",
        "english": "restaurant",
        "french": "restaurant",
        "german": "Restaurant",
        "spanish": "restaurante"
    },
    {
        "word": "hotel",
        "english": "hotel",
        "french": "hôtel",
        "german": "Hotel",
        "spanish": "hotel",
        "italian": "hotel"
    }
]

# Generate each word in each language
# Listen to pronunciation differences
# Which voices handle which languages best?
```

### Reflection Questions:

1. How accurate is the automatic language detection?
2. Do some voices work better for certain languages?
3. How would you handle mixed-language content?
4. What real-world applications need multilingual TTS?

---

## Exercise 14.5: Response Format Optimization

**Goal:** Choose the right audio format for different use cases.

### Available Formats:

- **mp3:** Compressed, good quality, widely compatible (default)
- **opus:** Low latency, best for streaming
- **aac:** Compressed, good for mobile apps
- **flac:** Lossless, larger files, highest quality
- **wav:** Uncompressed, largest files, compatibility
- **pcm:** Raw audio data, for processing

### Task 1: Format Comparison

Generate the same text in all formats and compare:

```python
text = "The quick brown fox jumps over the lazy dog."

formats = ["mp3", "opus", "aac", "flac", "wav", "pcm"]

for format in formats:
    # Generate audio with 'alloy' voice
    # response_format parameter = format
    # Compare file sizes and quality
    pass

# Questions:
# - Which format has the smallest file size?
# - Which format has the highest quality?
# - Which format loads fastest?
```

### Task 2: Use Case Matching

Choose formats for different applications:

```python
use_cases = [
    {
        "name": "podcast_episode",
        "duration": "30 minutes",
        "best_format": "mp3",  # Balance of quality and size
        "reason": "Widely compatible, good compression, acceptable quality"
    },
    {
        "name": "voice_assistant",
        "duration": "3 seconds",
        "best_format": "opus",  # Low latency
        "reason": "Fast streaming, immediate playback"
    },
    {
        "name": "audiobook_archival",
        "duration": "8 hours",
        "best_format": "flac",  # Lossless
        "reason": "Preserve quality for long-term storage"
    },
    {
        "name": "mobile_app_notifications",
        "duration": "2 seconds",
        "best_format": "aac",  # Mobile-optimized
        "reason": "Small size, good mobile support"
    },
    {
        "name": "audio_processing_pipeline",
        "duration": "varies",
        "best_format": "wav",  # Uncompressed
        "reason": "Easy to process, no compression artifacts"
    }
]

# For each use case:
# - Generate sample audio
# - Measure file size
# - Test loading/playback time
# - Validate format choice
```

### Reflection Questions:

1. When would you choose lossless over compressed formats?
2. How does format affect loading time in web applications?
3. What's the tradeoff between quality and bandwidth?
4. Which format is best for your specific project?

---

## Exercise 14.6: Cost Optimization Strategies

**Goal:** Minimize TTS costs while maintaining quality.

### Pricing Breakdown:

- **Input:** $0.60 per 1M tokens (~1 token per character)
- **Output:** $12.00 per 1M tokens (based on audio duration)

**Key insight:** Output cost is duration-based, so shorter audio = lower cost.

### Task 1: Cost Calculator

Build a cost estimator for TTS projects:

```python
def estimate_tts_cost(text, speed=1.0):
    """
    Estimate cost for TTS generation

    Input cost: $0.60 / 1M tokens
    Output cost: $12.00 / 1M tokens (audio duration-based)

    Rough estimate:
    - Input tokens ≈ character count
    - Output tokens ≈ (char count / 4) / speed
      (Approximation: 1 second of audio ≈ 25 output tokens)
    """
    input_tokens = len(text)

    # Estimate audio duration in seconds (rough: 150 chars per minute)
    duration_seconds = (len(text) / 150) * 60 / speed

    # Estimate output tokens (rough: 25 tokens per second of audio)
    output_tokens = duration_seconds * 25

    input_cost = (input_tokens / 1_000_000) * 0.60
    output_cost = (output_tokens / 1_000_000) * 12.00
    total_cost = input_cost + output_cost

    return {
        "input_tokens": input_tokens,
        "output_tokens": int(output_tokens),
        "duration_seconds": duration_seconds,
        "input_cost": input_cost,
        "output_cost": output_cost,
        "total_cost": total_cost
    }

# Test with different text lengths:
test_cases = [
    "Hello!",                                    # ~6 chars
    "The quick brown fox jumps over the lazy dog." * 10,  # ~440 chars
    open("long_article.txt").read()              # 5000+ chars
]

for text in test_cases:
    costs = estimate_tts_cost(text)
    print(f"Text length: {len(text)} chars")
    print(f"Estimated cost: ${costs['total_cost']:.6f}")
    print(f"Duration: {costs['duration_seconds']:.1f} seconds")
    print()
```

### Task 2: Cost Reduction Techniques

Compare costs across different strategies:

```python
original_text = """
This is a long article about artificial intelligence and its applications
in modern technology. [... 500 more words ...]
"""

strategies = [
    {
        "name": "baseline",
        "text": original_text,
        "speed": 1.0,
        "format": "mp3"
    },
    {
        "name": "faster_speed",
        "text": original_text,
        "speed": 1.5,  # 50% faster = lower output cost
        "format": "mp3"
    },
    {
        "name": "summarized",
        "text": "This article discusses AI applications in modern technology. [100 words]",
        "speed": 1.0,
        "format": "mp3"
    },
    {
        "name": "compressed_format",
        "text": original_text,
        "speed": 1.0,
        "format": "opus"  # More efficient encoding
    }
]

# For each strategy:
# 1. Generate audio
# 2. Calculate actual cost
# 3. Measure file size
# 4. Assess quality tradeoff

# Which strategy saves the most while maintaining quality?
```

### Task 3: Batch Processing Optimization

Calculate costs for large-scale TTS projects:

```python
project_scenarios = [
    {
        "name": "audiobook",
        "word_count": 80000,  # Typical novel
        "use_case": "Convert entire book to audio"
    },
    {
        "name": "course_narration",
        "word_count": 25000,  # 10 lessons
        "use_case": "Narrate online course content"
    },
    {
        "name": "product_descriptions",
        "word_count": 50000,  # 1000 products × 50 words each
        "use_case": "Generate audio for e-commerce"
    },
    {
        "name": "daily_news_brief",
        "word_count": 500,    # Daily summary
        "use_case": "365 days of news briefs"
    }
]

for scenario in project_scenarios:
    # Estimate total characters (words × 5)
    char_count = scenario["word_count"] * 5

    # Calculate costs
    costs = estimate_tts_cost("x" * char_count)

    print(f"{scenario['name']}:")
    print(f"  Words: {scenario['word_count']:,}")
    print(f"  Estimated cost: ${costs['total_cost']:.2f}")
    print(f"  Duration: {costs['duration_seconds']/60:.1f} minutes")
    print()
```

### Reflection Questions:

1. How do speed adjustments affect total cost?
2. When is summarization worth the quality tradeoff?
3. What's the break-even point for batch processing?
4. How does TTS cost compare to human voice actors?

---

## Exercise 14.7: Build a Real-World Application

**Goal:** Combine everything learned into a production-ready TTS application.

### Project Ideas:

#### Option 1: Accessible Blog Reader

Convert blog posts to audio for accessibility:

```python
class BlogAudioGenerator:
    def __init__(self):
        self.voice = "echo"  # Professional, clear
        self.speed = 1.0
        self.format = "mp3"

    def generate_audio(self, blog_post):
        """
        Convert blog post to audio

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

        # Generate audio via OpenAI TTS
        # Save with blog post ID
        # Return audio file path
        pass

    def generate_podcast_feed(self, blog_posts):
        """Generate audio for multiple posts, create RSS feed"""
        pass

# Test with your blog or Medium articles
```

#### Option 2: Language Learning Flashcards

Audio flashcards for vocabulary:

```python
class LanguageLearningAudio:
    def __init__(self, target_language, native_language="english"):
        self.target_language = target_language
        self.native_language = native_language

    def generate_flashcard_audio(self, word, translation, example):
        """
        Generate 4 audio files per flashcard:
        1. Word (target language, 0.8x speed)
        2. Translation (native language, 1.0x speed)
        3. Example sentence (target language, 0.9x speed)
        4. Full flashcard (all together, 1.0x speed)
        """
        # File 1: Target word (slow, clear)
        # File 2: Translation (normal)
        # File 3: Example (slightly slow)
        # File 4: Combined
        pass

    def generate_lesson_audio(self, vocabulary_list):
        """Generate audio for entire lesson"""
        # Intro, words, review, quiz
        pass

# Build a complete lesson for 20 vocabulary words
```

#### Option 3: News Briefing Generator

Daily personalized news audio:

```python
class NewsBriefingGenerator:
    def __init__(self):
        self.voices = {
            "intro": "nova",       # Energetic
            "headlines": "echo",   # Professional
            "weather": "coral",    # Friendly
            "outro": "alloy"       # Neutral
        }

    def generate_daily_briefing(self, news_data):
        """
        Create personalized news audio:

        Structure:
        1. Intro (nova): "Good morning! Here's your news briefing for [date]."
        2. Top headlines (echo): 3 main stories
        3. Weather (coral): Local forecast
        4. Outro (alloy): "That's your briefing. Have a great day!"
        """
        segments = []

        # Generate each segment with appropriate voice
        # Combine into single audio file
        # Add timestamps for seeking
        pass

    def schedule_daily_generation(self):
        """Run every morning at 6 AM"""
        pass

# Integrate with news API, generate daily briefing
```

#### Option 4: Interactive Storytelling

Choose-your-own-adventure audiobook:

```python
class InteractiveStoryAudio:
    def __init__(self):
        self.character_voices = {
            "narrator": "fable",
            "hero": "nova",
            "villain": "onyx",
            "mentor": "sage"
        }

    def generate_story_segment(self, segment):
        """
        Generate audio for story segment

        Features:
        - Different voice for each character
        - Instructions for emotional delivery
        - Choice prompts at end of segments
        """
        audio_parts = []

        for line in segment['lines']:
            character = line['character']
            text = line['text']
            emotion = line.get('emotion', 'neutral')

            instructions = self._get_instructions(emotion)
            voice = self.character_voices[character]

            # Generate audio with character voice + instructions
            # Add to audio_parts
            pass

        # Combine all parts
        # Add choice prompt
        return audio_file

    def _get_instructions(self, emotion):
        emotion_map = {
            "excited": "Speak with excitement and energy",
            "scared": "Speak with fear and tension in your voice",
            "wise": "Speak slowly and thoughtfully, as a mentor",
            "angry": "Speak with intensity and controlled anger"
        }
        return emotion_map.get(emotion, "Speak naturally")

# Build an interactive story with branching paths
```

### Implementation Checklist:

- [ ] API key management (environment variables)
- [ ] Error handling (rate limits, network errors)
- [ ] Cost tracking (log all API calls)
- [ ] File management (organized storage)
- [ ] Caching (avoid regenerating same content)
- [ ] User preferences (voice, speed, format)
- [ ] Testing (quality, cost, performance)
- [ ] Documentation (usage guide)

### Reflection Questions:

1. Which features are essential vs. nice-to-have?
2. How do you handle errors gracefully?
3. What's your strategy for cost management?
4. How will users customize audio generation?
5. What accessibility features should you include?

---

## Cost Analysis Summary

### Example Costs (October 2025):

**Input:** $0.60 / 1M tokens (≈ characters)
**Output:** $12.00 / 1M tokens (audio duration-based)

| Use Case | Text Length | Est. Cost | Notes |
|----------|-------------|-----------|-------|
| Short notification | 50 chars | $0.00001 | Tiny cost |
| Blog post | 2,000 chars | $0.0004 | Very affordable |
| Article | 5,000 chars | $0.001 | Under a cent |
| Short story | 20,000 chars | $0.004 | Less than a penny |
| Audiobook chapter | 50,000 chars | $0.010 | One cent |
| Full audiobook | 400,000 chars | $0.080 | 8 cents |

**Key Insight:** TTS is extremely affordable compared to human voice actors ($100-500 per finished hour of audio).

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
- Podcast generation from articles
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

1. **Experiment with instructions:** Try 50+ different instruction styles
2. **Build a voice profile:** Find the perfect voice for your brand
3. **Test multi-speaker:** Generate conversations with different voices
4. **Optimize costs:** Track actual usage, refine estimates
5. **Explore streaming:** Implement SSE streaming for real-time audio
6. **A/B testing:** Compare user engagement with different voices
7. **Accessibility audit:** Test with real users who need TTS

---

## Conclusion

OpenAI's TTS is:
- ✅ **Affordable:** 10x cheaper than many alternatives
- ✅ **High-quality:** Natural-sounding voices
- ✅ **Flexible:** 11 voices, instructions, speed, formats
- ✅ **Multilingual:** 50+ languages automatically
- ✅ **Easy to use:** Simple API, standard formats

**Key Takeaway:** With the right voice, instructions, and speed, TTS can create engaging, accessible audio content at a fraction of the cost of human narration.

Now build something amazing! 🎤

---

## Related Examples

Try the example code files to see TTS in action:

- [bash/14_text_to_speech_minimal.sh](../bash/14_text_to_speech_minimal.sh)
- [bash/14_text_to_speech_full.sh](../bash/14_text_to_speech_full.sh)
- [nodejs/14_text_to_speech.js](../nodejs/14_text_to_speech.js)
- [python/14_text_to_speech.py](../python/14_text_to_speech.py)
- [go/14_text_to_speech.go](../go/14_text_to_speech.go)

---

## Related Exercises

- [Exercise 1: Basic Chat](01_basic_chat.md) - API fundamentals and token usage
- [Exercise 13: Reasoning](13_reasoning.md) - Another OpenAI model integration

[← Previous: Reasoning](13_reasoning.md) | [Back to Exercises](README.md)
