![AI 101 Banner](ai101.png)

# AI 101: Learn AI APIs the Safe Way

**A hands-on learning path from basic AI to responsible AI development**

Teaching kids AI is cool. Teaching them **safe AI** is cooler. 🛡️

This repository is a complete learning journey for anyone starting with AI APIs—especially young developers making the leap from coding lessons to real computer science. Every example runs in **6 languages** (Bash, Node.js, Python, Go, C, and C++) so you can see how the same concepts translate across different programming environments.

But here's what makes this different: **AI safety is built in from day one.** Alongside chat, vision, and audio examples, you'll learn content moderation with LlamaGuard, prompt injection defense with Prompt Guard, and responsible AI practices throughout.

## What You'll Learn

### Core AI Capabilities
- 💬 **Chat & Reasoning**: Question-answering, step-by-step thinking, prompt caching
- 👁️ **Vision**: Image analysis with multimodal AI models
- 🎙️ **Audio**: Speech-to-text transcription with Whisper
- 🔊 **Voice Synthesis**: Text-to-speech with Fish Audio (free), including two-speaker podcasts
- 🔍 **Web Search**: AI-powered search and content extraction
- 🛠️ **AI Agents**: Building autonomous agents with function calling

### Safety & Security (Built-in)
- 🛡️ **Content Moderation**: LlamaGuard for text and image safety
- 🔒 **Prompt Injection Defense**: Detecting jailbreak attempts with Prompt Guard
- ✅ **Responsible AI Patterns**: Safety checks throughout every workflow

### Programming Fundamentals
- 🌐 HTTP requests and API authentication
- 📋 JSON parsing and data handling
- 🔐 Environment variables and secrets management
- 🎯 Error handling and production patterns

## Why This Repository?

**🔑 A Rosetta Stone for AI APIs**
See the same concepts across Bash, Node.js, Python, and Go—perfect for understanding how languages differ and which one fits your style.

**📚 Bridge from Coding Lessons to Real CS**
Move beyond tutorials into production-ready patterns with real APIs, authentication, error handling, and best practices.

**🛡️ Safety-First Mindset**
Learn that AI development isn't just about what you can build—it's about what you *should* build. Safety examples are integrated, not afterthoughts.

**🎓 Built for Learning**
80% of every file is educational comments explaining WHY, not just WHAT. Perfect for self-paced learning or teaching others.

## Multi-Vendor Support

Example 01 (Basic Chat) demonstrates how to work with **5 different AI providers**:

| Provider | Strengths | Best Use Case |
|----------|-----------|---------------|
| **OpenRouter** | One key, hundreds of models, free tier | Default for this repo — chat, vision, safety, audio, everything |
| **Groq** | Fastest inference, cost-effective | Real-time applications, high volume |
| **OpenAI** | Most advanced models, GPT series | Complex reasoning, function calling |
| **Anthropic** | Claude models, nuanced responses | Long context, careful analysis |
| **SambaNova** | Open models (Llama), enterprise | Privacy-conscious, on-premise |

Each provider has its own example file (e.g., `01_basic_chat_OPENAI.py`) showing provider-specific authentication and API patterns.

## The 16 Examples

1. **Basic Chat** - Single question to the AI model (multi-vendor support!)
2. **System + User Prompts** - Controlling AI behavior with system instructions
3. **Prompt Templates** - Dynamic prompt compilation with variables and conditionals
4. **Vision Analysis** - Analyzing local images with multimodal models
5. **Safety Check (Text)** - Content moderation with LlamaGuard
6. **Safety Check (Image)** - Image content moderation with LlamaGuard Vision
7. **Prompt Guard** - Detecting jailbreak attempts
8. **Whisper Audio** - Transcribing audio to text with Whisper
9. **Tavily Search** - Web search with AI-powered answers
10. **Tavily Extract** - Extract clean content from web pages
11. **Tool Use** - AI agents with function calling (OpenRouter + Tavily)
12. **Web Search** - Built-in search-augmented generation via OpenRouter's `:online` models
13. **Code Execution** - Python code execution with openai/gpt-oss-20b (direct on Groq)
14. **Reasoning** - Step-by-step thinking with openai/gpt-oss-20b + prompt caching (direct on Groq)
15. **Text-to-Speech** - Voice synthesis with Fish Audio S2.1 Pro (via OpenRouter)
16. **Podcast** - Two-speaker podcast generation with Fish Audio S2.1 Pro (via OpenRouter)
18. **Pose Estimation** - Real-time computer vision with webcam skeleton overlay (Python, Go)

## 🎮 Featured Project: Alfred the Minecraft AI Counselor

**Want to see all these concepts come together in one epic project?**

Check out **[minecraft/mcchatbot](minecraft/mcchatbot/)** - an AI-powered Minecraft assistant that demonstrates:

- **🎭 System Prompts**: A 96-line personality that makes Alfred kind, helpful, and fun
- **🛠️ Tool Use**: 12 functions the AI can call (teleport, weather, Easter eggs like floating cats!)
- **⚡ AI Safety with Flair**: When kids use bad language, they get a friendly message... AND a dramatic lightning bolt strikes nearby!

```
Player: "You're so stupid Alfred!"
[💥 LIGHTNING BOLT strikes 3 blocks ahead 💥]
Alfred: Let's keep the chat kind. Adventures are better when everyone feels welcome.
```

**Why it's perfect for learning**:
- Combines chat, tool calling, and moderation in one project
- Visual feedback makes AI concepts concrete (watching AI teleport players is magic!)
- Safety moderation that's memorable (lightning strikes!) but educational
- Full audit logs for parent/educator review
- Kids can modify Alfred's personality by editing the system prompt

**[→ Check out the full Alfred project](minecraft/mcchatbot/)**

## Repository Structure

```
ai101/
├── bash/          # Bash/cURL examples (minimal + full versions)
├── nodejs/        # Node.js examples (built-in modules only)
├── python/        # Python examples (standard library only)
├── go/            # Go examples (standard library only)
├── c/             # C examples (libcurl + cJSON)
├── cpp/           # C++ examples (libcurl + nlohmann/json)
├── arduino/       # Arduino/ESP32 examples
├── exercises/     # Hands-on practice exercises
└── README.md      # This file
```

Each language folder contains the same 16 examples implemented in that language (except Arduino has only examples 1-8), plus a detailed README explaining the code.

**Note on C/C++**: These examples use minimal external dependencies: **libcurl** (HTTP requests) and a JSON library (**cJSON** for C, **nlohmann/json** for C++). Both JSON libraries can be bundled directly in the project - see their respective READMEs for setup.

## Prerequisites

### Primary Provider API Keys

Example 01 (Basic Chat) supports multiple AI providers, but **everything in this repo defaults to OpenRouter** — a direct connection, no proxy, no whitelist, free to sign up.

**OpenRouter (Default - powers nearly every example):**
1. Go to https://openrouter.ai
2. Sign up and open the dashboard
3. Click **Keys** → **Create Key**
4. Copy the key (looks like `sk-or-v1-xxx...`)
5. Save/export as `OPENROUTER_API_KEY`
6. Browse free and paid models at https://openrouter.ai/models

**Groq (used directly for examples 13-14, code execution + reasoning):**
1. Go to https://console.groq.com
2. Sign up for a free account
3. Create a new API key
4. Save as `GROQ_API_KEY`

**OpenAI (optional, used by the `_OPENAI` provider-comparison variant of example 01):**
1. Go to https://platform.openai.com
2. Sign up for an account
3. Create a new API key
4. Save as `OPENAI_API_KEY`

**Anthropic (Claude, optional advanced examples):**
1. Go to https://console.anthropic.com
2. Sign up for an account
3. Navigate to API Keys
4. Create a new API key
5. Save as `ANTHROPIC_API_KEY`

**SambaNova (optional open-model provider):**
1. Go to https://sambanova.ai
2. Sign up for an account
3. Get your API key
4. Save as `SAMBANOVA_API_KEY`

### Additional API Keys

**Tavily (for examples 08-10):**
1. Go to https://tavily.com
2. Sign up for a free account
3. Get your API key from the dashboard
4. Add it to your environment: `export TAVILY_API_KEY="tvly-your-key"`

**Set up your environment:**

**Option 1: Using .env file (recommended for local development)**
```bash
# Copy the example file
cp .env.example .env

# Edit .env and add your API key
# OPENROUTER_API_KEY=sk-or-v1-your_actual_key_here
```

**Option 2: Export to shell (for bash/curl examples)**
```bash
# Add to your ~/.bashrc or ~/.zshrc
export OPENROUTER_API_KEY="sk-or-v1-your_api_key_here"

# Reload your shell
source ~/.bashrc
```

**Important:** Never commit your `.env` file or expose your API key!

## Quick Start

Pick your language and jump in:

- **New to programming?** Start with `bash/` - no installation needed
- **Know JavaScript?** Check out `nodejs/`
- **Python fan?** Head to `python/`
- **Systems programmer?** Try `go/`
- **Learning C/C++?** Explore `c/` or `cpp/` for low-level understanding
- **Building IoT devices?** Explore `arduino/` for ESP32/ESP8266

Each folder has a README with setup instructions and detailed code explanations.

After running the basic examples, head to [**exercises/**](exercises/) for hands-on practice!

## API Reference Quick Guide

### Endpoint

```
POST https://openrouter.ai/api/v1/chat/completions
```

Two examples (13 code execution, 14 reasoning) call Groq directly instead, to use
its native code-interpreter tool and prompt caching:

```
POST https://api.groq.com/openai/v1/chat/completions
```

### Authentication

```
Authorization: Bearer $OPENROUTER_API_KEY   # openrouter.ai — most examples
Authorization: Bearer $GROQ_API_KEY         # console.groq.com — examples 13, 14 only
```

### Models Used

OpenRouter exposes an OpenAI-compatible catalog spanning hundreds of models from
dozens of providers — model names, capabilities, and pricing are set by each
underlying provider and mirrored by OpenRouter. Browse the live catalog and free-tier
availability at https://openrouter.ai/models (free-tier models rotate over time).

**Full documentation:** https://openrouter.ai/docs

### OpenRouter Models

| Model | Purpose | Pricing (per 1M tokens) |
|-------|---------|------------------------|
| [`openai/gpt-oss-20b`](https://openrouter.ai/openai/gpt-oss-20b) | Chat, System Prompt, Templates, Tool Use | Input: $0.03/1M<br>Output: $0.13/1M |
| [`google/gemma-4-31b-it`](https://openrouter.ai/google/gemma-4-31b-it) | Vision | Input: $0.09/1M<br>Output: $0.34/1M |
| [`meta-llama/llama-guard-4-12b`](https://openrouter.ai/meta-llama/llama-guard-4-12b) | Content Safety (Text + Vision) | Input: $0.18/1M<br>Output: $0.18/1M |
| [`openai/gpt-oss-safeguard-20b`](https://openrouter.ai/openai/gpt-oss-safeguard-20b) | Prompt Injection Detection | Input: $0.075/1M<br>Output: $0.30/1M |
| [`openai/whisper-1`](https://openrouter.ai/openai/whisper-1) | Audio Transcription (`/audio/transcriptions`) | **$0.006 per minute of audio** |
| [`fish-audio/s2.1-pro-free:free`](https://openrouter.ai/fish-audio/s2.1-pro-free:free) | Text-to-Speech, 2-speaker Podcast (`/audio/speech`) | Free |
| any model + `:online` suffix | Web Search (search-augmented generation) | Model's own cost + ~$0.005/request (search) |

Both `openai/gpt-oss-20b` and `google/gemma-4-31b-it` have genuinely free
`:free` variants on OpenRouter, but this repo uses the small-paid versions
instead — the free tiers proved unreliable during testing (verbose
chain-of-thought reasoning that burns through `max_tokens` before reaching an
answer, occasional upstream rate-limiting, and flaky multi-turn tool-call
continuations). The paid versions cost a fraction of a cent per call and
answer reliably.

### Groq Models (direct, examples 13-14 only)

| Model | Purpose | Context | Max Output | Pricing (per 1M tokens) |
|-------|---------|---------|------------|------------------------|
| [`openai/gpt-oss-20b`](https://console.groq.com/docs/model/openai/gpt-oss-20b) | Reasoning + Code Execution | 131K | 8,192 | Input: $0.10/1M ($0.05/1M cached)<br>Output: $0.50/1M |

**Pricing Notes:**
- You are billed separately for input tokens (prompt) and output tokens (completion)
- Vision requests include image tokens in the input count
- Most requests cost less than $0.01 (one cent), and several examples are entirely free on OpenRouter's free tier
- Examples in this repo calculate and display the exact cost for each call

### Request Format

```json
{
  "model": "openai/gpt-oss-20b",
  "messages": [
    {
      "role": "system",
      "content": "You are a helpful assistant."
    },
    {
      "role": "user",
      "content": "Hello!"
    }
  ],
  "temperature": 0.7,
  "max_tokens": 1024,
  "top_p": 1,
  "stream": false
}
```

### Key Parameters

- **model** (required) - Which AI model to use
- **messages** (required) - Array of conversation messages
- **temperature** (optional, 0-2, default 1) - Creativity level (0=focused, 2=random)
- **max_tokens** (optional) - Maximum response length
- **top_p** (optional, 0-1, default 1) - Nucleus sampling threshold
- **stream** (optional, boolean) - Stream response token-by-token
- **stop** (optional, string/array) - Stop generating at these sequences

### Message Roles

- **system** - Instructions that guide the AI's behavior
- **user** - Your questions or prompts
- **assistant** - AI's previous responses (for multi-turn conversations)

### Vision API Format

```json
{
  "model": "google/gemma-4-31b-it",
  "messages": [
    {
      "role": "user",
      "content": [
        {
          "type": "text",
          "text": "What's in this image?"
        },
        {
          "type": "image_url",
          "image_url": {
            "url": "data:image/jpeg;base64,/9j/4AAQSkZJRg..."
          }
        }
      ]
    }
  ]
}
```

### Image Requirements

- **Max size (URL):** 20 MB
- **Max size (base64):** 4 MB
- **Max resolution:** 33 megapixels
- **Max images per request:** 5
- **Supported formats:** JPEG, PNG, GIF, WebP

### Response Format

```json
{
  "id": "chatcmpl-123",
  "object": "chat.completion",
  "created": 1234567890,
  "model": "openai/gpt-oss-20b",
  "choices": [
    {
      "index": 0,
      "message": {
        "role": "assistant",
        "content": "The answer is..."
      },
      "finish_reason": "stop"
    }
  ],
  "usage": {
    "prompt_tokens": 10,
    "completion_tokens": 20,
    "total_tokens": 30
  }
}
```

### Finish Reasons

- **stop** - Natural completion
- **length** - Hit max_tokens limit
- **content_filter** - Blocked by safety filters
- **tool_calls** - Model wants to call a function

## Learning Path

1. **Start with Example 1** - Understand basic API calls
2. **Move to Example 2** - Learn how system prompts work
3. **Try Example 3** - See multimodal AI (vision) in action
4. **Explore Examples 4 & 5** - Understand AI safety tools (text + images)
5. **Try Example 6** - Learn about prompt injection detection
6. **Try Example 7** - Transcribe audio with Whisper

## Common Issues

**"Unauthorized" error:**
- Check your API key is set: `echo $OPENROUTER_API_KEY` (or `$GROQ_API_KEY` for examples 13-14)
- Make sure you exported it in your current shell

**"Model not found" error:**
- Copy model names exactly (they're case-sensitive)
- Check https://openrouter.ai/models for current models — free-tier models rotate over time

**Image too large:**
- Resize images before encoding
- Use JPEG format for smaller file sizes
- Base64 encoding increases size by ~33%

**Audio file issues:**
- Max audio file size: 25 MB
- Supported formats: mp3, wav, m4a, flac, ogg, webm
- Cost is based on audio duration, not file size

## Resources

- **OpenRouter Docs:** https://openrouter.ai/docs
- **OpenRouter Model Catalog:** https://openrouter.ai/models
- **Groq Model Catalog:** https://console.groq.com/docs/models
- **Groq Vision Guide:** https://console.groq.com/docs/vision
- **Groq Audio Guide:** https://console.groq.com/docs/speech-text

## Hands-On Practice

Ready to experiment and build? Check out the [**exercises/**](exercises/) directory for progressive hands-on challenges:

- **[Exercise 1: Basic Chat](exercises/01_basic_chat.md)** - Temperature, tokens, cost tracking
- **[Exercise 2: System Prompt](exercises/02_system_prompt.md)** - Personas, JSON mode, constraints
- **[Exercise 3: Vision](exercises/03_vision.md)** - Resolution, OCR, multi-image analysis
- **[Exercise 4: Safety Check (Text)](exercises/04_safety_text.md)** - Content moderation, validators
- **[Exercise 5: Safety Check (Image)](exercises/05_safety_image.md)** - Vision moderation, context
- **[Exercise 6: Prompt Guard](exercises/06_prompt_guard.md)** - Jailbreak detection, security
- **[Exercise 7: Whisper Audio](exercises/07_whisper.md)** - Quality tests, languages, noise
- **[Exercise 8: Tavily Search](exercises/08_tavily_search.md)** - Web search, time filters, domain control
- **[Exercise 9: Tavily Extract](exercises/09_tavily_extract.md)** - Content extraction, article analysis
- **[Exercise 10: Tool Use](exercises/10_tool_use.md)** - AI agents, function calling, autonomous workflows
- **[Exercise 11: Web Search (OpenRouter)](exercises/11_web_search.md)** - Built-in search with the `:online` model suffix
- **[Exercise 12: Code Execution](exercises/12_code_execution.md)** - Python execution with openai/gpt-oss-20b
- **[Exercise 13: Reasoning](exercises/13_reasoning.md)** - Step-by-step thinking with prompt caching
- **[Exercise 14: Text-to-Speech](exercises/14_text_to_speech.md)** - Voice synthesis with Fish Audio (free)
- **[Exercise 15: Podcast](exercises/16_podcast.md)** - Multi-speaker podcast generation with Fish Audio
- **[Exercise 18: Pose Estimation](exercises/18_pose_estimation.md)** - Computer vision, blue ball → shapes → ArUco → skeleton

Each exercise file includes:
- Progressive difficulty levels
- Real-world applications
- Reflection questions
- Cost calculations

**Philosophy:** These exercises follow Bloom's Taxonomy (understand → apply → analyze → create) with scaffolding for independent learning.

## Learning Path

1. **Start with Example 1** - Understand basic API calls
2. **Move to Example 2** - Learn how system prompts work
3. **Try Example 3** - See multimodal AI (vision) in action
4. **Explore Examples 4 & 5** - Understand AI safety tools (text + images)
5. **Try Example 6** - Learn about prompt injection detection
6. **Try Example 7** - Transcribe audio with Whisper
7. **Try Examples 8 & 9** - Web search and content extraction with Tavily
8. **Try Example 10** - Build AI agents with function calling
9. **Practice with [exercises/](exercises/)** - Deepen your understanding

## Next Steps

After completing these examples and exercises, you can:
- Build a chatbot with conversation history
- Create an image analysis tool
- Implement content moderation for text and images
- Build a voice transcription app
- Add streaming responses
- Combine multiple API calls (e.g., audio transcription → safety check → AI response)

**Ready for a full project?** Check out **[minecraft/mcchatbot](minecraft/mcchatbot/)** - an AI-powered Minecraft assistant that demonstrates system prompts, tool calling, and AI safety with dramatic lightning strikes for moderation! It's the most fun way to see everything you've learned come together.

**Pick a language folder and start coding!**

## License

- **Code** (everything under the language folders) is under the [MIT License](LICENSE).
- **Lessons, text and diagrams** are under
  [Creative Commons Attribution 4.0](LICENSE-CONTENT) (CC BY 4.0). Use and
  adapt them freely, including for teaching, and credit "Patrick Deglon" with
  a link to this repository.
- Third-party media files (sample audio and images) keep their original terms.
