# Bash/cURL Examples for AI APIs

Shell script examples demonstrating multiple AI provider APIs using plain bash and cURL commands. Learn how to interact with OpenRouter, Groq, OpenAI, Anthropic, and SambaNova using the same concepts.

## 📚 Two Versions of Each Example

Each example comes in **two flavors** to suit different needs:

### 🔹 **Minimal Version** (`*_minimal.sh`)
- **Shell**: `/bin/sh` (POSIX-compliant, works everywhere)
- **Dependencies**: Only `curl` (no `jq` required)
- **Output**: Raw JSON response
- **Documentation**: Concise parameter explanations
- **Use case**: Quick testing, scripting, minimal environments

### 🔸 **Full Version** (`*_full.sh`)
- **Shell**: `/bin/bash` (more features)
- **Dependencies**: `curl` + `jq` for JSON parsing
- **Output**: Pretty-printed, parsed, formatted
- **Documentation**: Extensive explanations, examples, best practices
- **Use case**: Learning, debugging, production workflows

## 🚀 Quick Start

### Setting up API Keys

Different providers require different API keys:

```bash
# OpenRouter (default for most examples: chat, vision, safety, tool use,
# web search, whisper/STT, TTS, podcast)
export OPENROUTER_API_KEY="sk-or-..."

# Groq (only for the code-execution and reasoning examples, which use
# Groq's native hosted code-interpreter tool + prompt caching)
export GROQ_API_KEY="gsk_..."

# Tavily (only for the two Tavily search/extract examples)
export TAVILY_API_KEY="tvly-..."

# OpenAI (used by the _OPENAI vendor-comparison variant of example 1)
export OPENAI_API_KEY="sk-..."

# Anthropic (Claude, used by the _ANTHROPIC vendor-comparison variant)
export ANTHROPIC_API_KEY="sk-ant-..."

# SambaNova (used by the _SAMBA vendor-comparison variant)
export SAMBANOVA_API_KEY="..."

# Google Gemini (used by the _GEMINI vendor-comparison variant)
export GEMINI_API_KEY="..."
```

> Most examples only need `OPENROUTER_API_KEY` — get one free at
> https://openrouter.ai/keys. The `GROQ_API_KEY` and `TAVILY_API_KEY` are only
> needed for the handful of examples noted above; the OpenAI/Anthropic/
> SambaNova/Gemini keys are only needed if you run example 1's `_OPENAI`/
> `_ANTHROPIC`/`_SAMBA`/`_GEMINI` variants to compare providers directly.

### Option 1: Minimal (Just Works™)
```bash
# Run any minimal script
./01_basic_chat_minimal.sh           # OpenRouter (default)
./01_basic_chat_OPENAI_minimal.sh    # OpenAI direct
./01_basic_chat_ANTHROPIC_minimal.sh # Anthropic direct
./01_basic_chat_SAMBA_minimal.sh     # SambaNova direct
./01_basic_chat_GEMINI_minimal.sh    # Google Gemini direct
```

### Option 2: Full (Beautiful Output)
```bash
# Install jq first (if needed)
# macOS: brew install jq
# Linux: sudo apt-get install jq
# Windows: choco install jq

# Run any full script
./01_basic_chat_full.sh
```

## 📋 Available Examples

| Example | Minimal | Full | What it does |
|---------|---------|------|--------------|
| **01 Basic Chat** | `01_basic_chat_minimal.sh` | `01_basic_chat_full.sh` | Ask AI a question (OpenRouter) |
| **01 Basic Chat - OpenAI** | `01_basic_chat_OPENAI_minimal.sh` | - | Ask AI a question (OpenAI) |
| **01 Basic Chat - Anthropic** | `01_basic_chat_ANTHROPIC_minimal.sh` | - | Ask AI a question (Claude) |
| **01 Basic Chat - SambaNova** | `01_basic_chat_SAMBA_minimal.sh` | - | Ask AI a question (SambaNova) |
| **01 Basic Chat - Gemini** | `01_basic_chat_GEMINI_minimal.sh` | - | Ask AI a question (Google Gemini 2.5 Flash) |
| **02 System Prompt** | `02_system_prompt_minimal.sh` | `02_system_prompt_full.sh` | Control AI behavior with instructions |
| **03 Vision** | `03_vision_minimal.sh` | `03_vision_full.sh` | Analyze images with AI |
| **04 Safety Check** | `04_safety_check_minimal.sh` | `04_safety_check_full.sh` | Moderate text content |
| **05 Image Safety** | `05_image_safety_check_minimal.sh` | `05_image_safety_check_full.sh` | Moderate image content |
| **06 Prompt Guard** | `06_prompt_guard_minimal.sh` | `06_prompt_guard_full.sh` | Detect prompt injection attacks |
| **07 Whisper** | `07_whisper_minimal.sh` | `07_whisper_full.sh` | Transcribe audio to text |
| **08 Tavily Search** | `08_tavily_search_minimal.sh` | `08_tavily_search_full.sh` | Web search with AI-powered answers |
| **09 Tavily Extract** | `09_tavily_extract_minimal.sh` | `09_tavily_extract_full.sh` | Extract clean content from web pages |
| **10 Tool Use** | `10_tool_use_minimal.sh` | `10_tool_use_full.sh` | AI agents with function calling |
| **11 Web Search** | `11_web_search_minimal.sh` | `11_web_search_full.sh` | openai/gpt-oss-20b:online with built-in web search |
| **12 Code Execution** | `12_code_execution_minimal.sh` | `12_code_execution_full.sh` | openai/gpt-oss-20b Python execution |
| **13 Reasoning** | `13_reasoning_minimal.sh` | `13_reasoning_full.sh` | Step-by-step thinking with prompt caching |
| **15 Text-to-Speech** | `15_text_to_speech_minimal.sh` | `15_text_to_speech_full.sh` | Fish Audio TTS via OpenRouter (free tier) |
| **15 Podcast** | `16_podcast_minimal.sh` | `16_podcast_full.sh` | Multi-speaker podcast with Gemini TTS |

## 🎯 When to Use Which Version?

### Use **Minimal** when:
- ✅ Quick testing or one-off API calls
- ✅ Embedding in scripts or automation
- ✅ Working in minimal environments (Docker, CI/CD)
- ✅ Don't want to install `jq`
- ✅ Just need the raw JSON

### Use **Full** when:
- 📚 Learning how the APIs work
- 🐛 Debugging API responses
- 💰 Tracking costs and token usage
- 🎨 Want pretty, readable output
- 📊 Need parsed data (just the answer, costs, etc.)

## 🌐 Provider Comparison

### Supported Providers

| Provider | API Endpoint | Key Models | Best For |
|----------|-------------|------------|----------|
| **OpenRouter (Default)** | `openrouter.ai/api/v1` | 400+ models across every major lab, incl. free `:free` tiers | One key, no whitelist, works for anyone |
| **Groq (code exec / reasoning only)** | `api.groq.com/openai/v1` | `openai/gpt-oss-20b` | Native hosted code-interpreter + prompt caching |
| **OpenAI** | `api.openai.com` | GPT-4o, GPT-5 mini | Advanced reasoning, TTS |
| **Anthropic** | `api.anthropic.com` | Claude Haiku/Sonnet/Opus | Long context, policy-aware |
| **SambaNova** | `api.sambanova.ai` | Llama 3.1 variants | Open models, enterprise focus |
| **Google Gemini** | `generativelanguage.googleapis.com` | Gemini 2.0/2.5 Flash & Pro | Classroom-friendly multimodal prompts |

### About OpenRouter

OpenRouter is a single API that routes to hundreds of models from every major
lab (OpenAI, Anthropic, Google, Meta, Mistral, and more) — including many
`:free` tier models with no cost. Every request goes to
`https://openrouter.ai/api/v1/...` with one key (`OPENROUTER_API_KEY`), no
whitelist or account approval required. This repo uses it as the default for
chat, vision, safety, tool use, web search, transcription, and text-to-speech.
Two examples (code execution, reasoning) call Groq directly instead, since
they rely on Groq's native hosted code-interpreter tool and prompt caching,
which OpenRouter doesn't expose the same way.

## 💡 Example Comparison

### Minimal Version Output:
```json
{"id":"chatcmpl-123","object":"chat.completion","created":1234567890,"model":"openai/gpt-oss-20b","choices":[{"index":0,"message":{"role":"assistant","content":"The capital of Switzerland is Bern."},"finish_reason":"stop"}],"usage":{"prompt_tokens":15,"completion_tokens":8,"total_tokens":23}}
```

### Full Version Output:
```
Sending request to AI...

========================================
Raw API Response (pretty-printed):
========================================
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
        "content": "The capital of Switzerland is Bern."
      },
      "finish_reason": "stop"
    }
  ],
  "usage": {
    "prompt_tokens": 15,
    "completion_tokens": 8,
    "total_tokens": 23
  }
}

========================================
AI's Answer:
========================================
The capital of Switzerland is Bern.

========================================
Token Usage:
========================================
  Prompt tokens:     15
  Completion tokens: 8
  Total tokens:      23

========================================
Cost Breakdown:
========================================
  This is openai/gpt-oss-20b — a fraction of a cent per call, and reliably answers within the token budget below.
  (Paid rate for reference: $0.08/1M input, $0.20/1M output tokens.)
```

## 🛠️ Prerequisites

### For Minimal Scripts:
- `curl` (usually pre-installed)
- `OPENROUTER_API_KEY` environment variable (see per-example notes above for
  when you also need `GROQ_API_KEY` or `TAVILY_API_KEY`)

### For Full Scripts (additional):
- `jq` for JSON parsing
- `bc` for cost calculations (optional, falls back to `awk`)

### Installing jq:
```bash
# macOS
brew install jq

# Ubuntu/Debian
sudo apt-get install jq

# Fedora/RHEL
sudo dnf install jq

# Windows (with Chocolatey)
choco install jq

# Or download from: https://jqlang.github.io/jq/download/
```

## 🔑 Getting Your API Key

1. Go to [openrouter.ai](https://openrouter.ai)
2. Sign up (no whitelist, no approval — anyone can create a key)
3. Open **Keys** in the dashboard
4. Create a new key (looks like `sk-or-...`)
5. Export it:
   ```bash
   export OPENROUTER_API_KEY="sk-or-your_key_here"
   ```

For the two examples that need Groq directly (code execution, reasoning),
get a free key at [console.groq.com](https://console.groq.com) and export
`GROQ_API_KEY` the same way.

## 📖 Learning Path

**Beginners** - Start with minimal versions:
1. `01_basic_chat_minimal.sh` - Learn API basics
2. `02_system_prompt_minimal.sh` - Control AI behavior
3. Read the parameter explanations at the bottom of each script

**Intermediate** - Upgrade to full versions:
1. Install `jq`
2. Run the full versions to see parsed output
3. Read the extensive documentation in the scripts

**Advanced** - Customize:
1. Modify the scripts for your use case
2. Combine multiple examples (e.g., Prompt Guard → Safety Check → Chat)
3. Build production workflows

## 💰 Pricing (as of Aug 2026, via OpenRouter unless noted)

| Model | Input | Output | Use Case |
|-------|-------|--------|----------|
| `openai/gpt-oss-20b` | $0.03/1M | $0.13/1M | General chat, tool use |
| `google/gemma-4-31b-it` | $0.09/1M | $0.34/1M | Vision |
| `meta-llama/llama-guard-4-12b` | $0.18/1M | $0.18/1M | Content moderation (text + images) |
| `openai/gpt-oss-safeguard-20b` | $0.075/1M | $0.30/1M | Prompt-injection / jailbreak detection |
| `openai/whisper-1` | $0.006/minute of audio | - | Speech-to-text |
| `fish-audio/s2.1-pro-free:free` | $0.00 | $0.00 | Text-to-speech, podcast |
| `openai/gpt-oss-20b` (direct on Groq) | $0.10/1M ($0.05/1M cached) | $0.50/1M | Code execution, reasoning |

Web search adds ~$0.005 per request (the `:online` plugin) on top of the
underlying model's own per-token cost.

## 🔒 Security Best Practices

1. **Never hardcode API keys** - Use environment variables
2. **Use Prompt Guard** - Filter malicious inputs
3. **Use LlamaGuard** - Moderate content
4. **Validate inputs** - Check file sizes, formats
5. **Rate limit** - Don't spam the API

## 🐛 Troubleshooting

### "Command not found: jq"
- You're running a `*_full.sh` script without `jq` installed
- Solution: Install `jq` or use the `*_minimal.sh` version

### "Unauthorized" or 401 error
- Your `OPENROUTER_API_KEY` (or, for examples 13/14, `GROQ_API_KEY`) is not set or invalid
- Solution: `export OPENROUTER_API_KEY="sk-or-..."`

### "curl: command not found"
- Rare, but curl might not be installed
- Solution: Install curl via your package manager

### Raw JSON is hard to read
- You're using a minimal script
- Solution: Use the full version with `jq`

## 🎯 Exercises

Now that you've mastered the basics, take your learning further with hands-on exercises!

## 🎓 Practice Exercises

Ready to deepen your understanding? Check out the **[exercises/](../exercises/)** directory for hands-on challenges:

- **[Exercise 1: Basic Chat](../exercises/01_basic_chat.md)** - Temperature, tokens, cost tracking
- **[Exercise 2: System Prompt](../exercises/02_system_prompt.md)** - Personas, JSON mode, constraints
- **[Exercise 3: Vision](../exercises/03_vision.md)** - Resolution, OCR, multi-image analysis
- **[Exercise 4: Safety Text](../exercises/04_safety_text.md)** - Content moderation, validators
- **[Exercise 5: Safety Image](../exercises/05_safety_image.md)** - Vision moderation, context
- **[Exercise 6: Prompt Guard](../exercises/06_prompt_guard.md)** - Jailbreak detection, security
- **[Exercise 7: Whisper](../exercises/07_whisper.md)** - Quality tests, languages, noise
- **[Exercise 8: Tavily Search](../exercises/08_tavily_search.md)** - Web search, time filters, domain control
- **[Exercise 9: Tavily Extract](../exercises/09_tavily_extract.md)** - Content extraction, article analysis
- **[Exercise 10: Tool Use](../exercises/10_tool_use.md)** - AI agents, function calling, autonomous workflows
- **[Exercise 11: Web Search](../exercises/11_web_search.md)** - Built-in search with openai/gpt-oss-20b:online
- **[Exercise 12: Code Execution](../exercises/12_code_execution.md)** - Python execution with openai/gpt-oss-20b
- **[Exercise 13: Reasoning](../exercises/13_reasoning.md)** - Step-by-step thinking with prompt caching
- **[Exercise 14: Text-to-Speech](../exercises/14_text_to_speech.md)** - Voice synthesis with 11 voices
- **[Exercise 15: Podcast](../exercises/16_podcast.md)** - Multi-speaker podcast generation with Gemini

Each exercise includes progressive challenges, real-world applications, and reflection questions.

## 📚 Additional Resources

- [OpenRouter Docs](https://openrouter.ai/docs)
- [OpenRouter Models](https://openrouter.ai/models)
- [jq Manual](https://jqlang.github.io/jq/manual/)
- [cURL Documentation](https://curl.se/docs/)

## 🤝 Contributing

Found a bug? Have an improvement? PRs welcome!

## 📄 License

MIT - Do whatever you want with these scripts!

---

**Happy scripting! 🚀**
