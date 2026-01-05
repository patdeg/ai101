# Exercise 16: Multi-Speaker Podcast Generation

[Previous: Text-to-Speech](15_text_to_speech.md) | [Index](README.md)

---

## What You Learned

In the basic example, you learned how to:
- Generate podcast audio with **two distinct speakers** in a single API call
- Format scripts using the `Speaker: text` pattern
- Configure different voices for each speaker using Demeterics multi-speaker TTS
- Save audio directly to WAV files (no base64 decoding needed)

Now let's practice and extend these concepts!

---

## Exercise 1: Voice Casting (Understand)

The example uses Aoede (female) and Charon (male). Try different voice pairings:

**Task**: Modify the script to use these alternative pairings and compare the results:

| Pairing | Host 1 | Host 2 | Style |
|---------|--------|--------|-------|
| A | Kore | Puck | Friendly, energetic |
| B | Zephyr | Fenrir | Bright female, deep male |
| C | Leda | Orus | Classic podcast duo |

**Questions**:
1. Which pairing sounds most like NPR's Planet Money?
2. Which pairing has the best contrast between hosts?
3. How does voice selection affect the "feel" of the content?

---

## Exercise 2: Topic Adaptation (Apply)

Create a new podcast script about a different historical topic.

**Task**: Write a 10-line podcast script (5 lines each speaker) about:
- The moon landing (1969)
- The invention of the telephone
- The construction of the pyramids

**Requirements**:
- Use the same `Speaker: text` format
- Include NPR-style vocal cues ("get this", "here's the thing")
- Balance speaking time between hosts
- End with a clear sign-off

---

## Exercise 3: Conversation Dynamics (Apply)

Analyze and improve the conversation flow.

**Task**: Rewrite this flat exchange to be more engaging:

```
Alex: Columbus sailed in 1492.
Sam: He had three ships.
Alex: They were the Nina, Pinta, and Santa Maria.
Sam: He landed in the Bahamas.
```

**Make it sound like NotebookLM's "Deep Dive"**:
- Add reactions ("Right!", "Exactly!", "Wow!")
- Include setup phrases ("So get this", "Here's the thing")
- Add follow-up questions
- Include moments of humor or surprise

---

## Exercise 4: Script Validation (Analyze)

Build a validator that checks podcast scripts for common issues.

**Task**: Write a function that validates:

```python
def validate_podcast_script(script):
    """
    Validate a podcast script and return issues.

    Checks:
    1. Exactly 2 unique speakers
    2. Each line starts with "Speaker: "
    3. Speaking time is roughly balanced (40-60% each)
    4. No special characters that break JSON (smart quotes, etc.)
    5. Script is under 4000 characters (API limit)

    Returns: list of issues (empty if valid)
    """
    pass
```

**Test with these scripts**:
```
# Should pass
Alex: Hello!
Sam: Hi there!

# Should fail (3 speakers)
Alex: Hello!
Sam: Hi!
Jordan: Hey everyone!

# Should fail (unbalanced)
Alex: This is a very long monologue that goes on and on...
Sam: Ok.
```

---

## Exercise 5: Podcast Pipeline (Create)

Build a complete podcast generation pipeline.

**Task**: Create a system that:

1. **Takes a topic as input** (e.g., "The French Revolution")
2. **Generates a script using an LLM** (use Groq from earlier examples)
3. **Validates the script** (from Exercise 4)
4. **Generates audio using Gemini TTS**
5. **Saves with metadata** (topic, duration, voices used)

**Bonus**: Add a "style" parameter:
- `deep_dive`: NotebookLM style (enthusiastic, casual)
- `npr`: More formal, measured
- `debate`: Two opposing viewpoints

---

## Exercise 6: Multi-Language Podcasts (Create)

Gemini TTS supports 24+ languages.

**Task**: Create a bilingual podcast where:
- Host 1 speaks English
- Host 2 speaks Spanish (or another language)
- They discuss the same topic from different cultural perspectives

**Example topic**: "Coffee culture around the world"

```
Alex: Coffee is huge in America. We drink it all day long!
Sam: En Espana, el cafe es diferente. Tomamos espresso, muy fuerte.
Alex: That's so interesting! Is it true you have coffee after lunch?
Sam: Si! Siempre tomamos cafe despues de comer. Es una tradicion.
```

**Challenge**: Does the same voice (e.g., Aoede) sound natural in both languages, or should you use different voices?

---

## Exercise 7: Educational Podcast Series (Create)

Design a podcast series for Victor (14, learning AI).

**Task**: Create episode outlines for a 5-episode series called "AI 101: The Podcast"

| Episode | Topic | Key Concepts |
|---------|-------|--------------|
| 1 | What is AI? | Definition, examples, history |
| 2 | How AI Learns | Training, data, patterns |
| 3 | AI Safety | Bias, hallucinations, guardrails |
| 4 | Building with AI | APIs, prompts, models |
| 5 | AI Future | Jobs, creativity, ethics |

**Requirements**:
- Each episode: 2-minute script (~400 words)
- Include at least one "mind-blowing" fact per episode
- End each with a teaser for the next episode
- Make it engaging for a teenage audience

---

## Real-World Applications

After completing these exercises, you can build:

1. **Automated News Podcasts** - Convert articles to audio with two hosts
2. **Educational Content** - Turn documentation into conversational learning
3. **Audiobook Dialogues** - Voice different characters in stories
4. **Meeting Summaries** - Generate podcast-style recaps of meetings
5. **Language Learning** - Create bilingual conversation practice
6. **Accessibility Tools** - Convert text content to engaging audio

---

## Reflection Questions

1. **Voice vs. Content**: How much does voice selection affect how the content is perceived? Would the Columbus podcast feel different with different voices?

2. **Script Writing**: What makes a good podcast script different from written content? How do you write for the ear instead of the eye?

3. **API Limitations**: The API limits you to 2 speakers. How would you work around this for a panel discussion with 4 people?

4. **Cost Considerations**: How would you estimate costs for generating a 30-minute podcast? What factors affect the cost?

5. **Ethical Considerations**: What are the implications of AI-generated podcasts that sound human? Should they be labeled as AI-generated?

---

## Voice Reference

| Voice | Description |
|-------|-------------|
| Puck | Upbeat, energetic |
| Kore | Firm, confident |
| Charon | Informative, clear |
| Zephyr | Bright, cheerful |
| Fenrir | Excitable, dynamic |
| Leda | Youthful, fresh |
| Aoede | Breezy, warm |
| Sulafat | Warm, friendly |
| Achird | Friendly, approachable |

[Full list of 30 voices at https://demeterics.ai/docs/speech]

---

## Next Steps

Congratulations! You've completed the AI101 exercise series. You now know how to:
- Make basic AI chat requests
- Control AI behavior with system prompts
- Use templates for dynamic prompts
- Analyze images with vision models
- Moderate content for safety
- Detect prompt injection attacks
- Transcribe audio with Whisper
- Search the web with Tavily
- Extract content from web pages
- Build AI agents with tool use
- Use built-in web search
- Execute code safely
- Apply reasoning with prompt caching
- Generate speech with TTS
- **Create multi-speaker podcasts**

You're ready to build real AI applications!

---

[Previous: Text-to-Speech](15_text_to_speech.md) | [Index](README.md)
