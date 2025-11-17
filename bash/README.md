# Bash/cURL Examples for AI APIs

Shell script examples demonstrating multiple AI provider APIs using plain bash and cURL commands. Learn how to interact with Groq, OpenAI, Anthropic, SambaNova, and Demeterics using the same concepts.

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
# Demeterics (default Groq proxy for most examples)
export DEMETERICS_API_KEY="dmt_..."

# OpenAI (used for TTS example)
export OPENAI_API_KEY="sk-..."

# Anthropic (Claude)
export ANTHROPIC_API_KEY="sk-ant-..."

# SambaNova
export SAMBANOVA_API_KEY="..."

# Google Gemini
export GEMINI_API_KEY="..."
```

> `DEMETERICS_API_KEY` is your Managed LLM Key from the Demeterics dashboard. One key unlocks Groq, Gemini, OpenAI, and Anthropic through the proxy with full observability.

### Option 1: Minimal (Just Works™)
```bash
# Run any minimal script
./01_basic_chat_minimal.sh           # Groq (default)
./01_basic_chat_OPENAI_minimal.sh    # OpenAI
./01_basic_chat_ANTHROPIC_minimal.sh # Anthropic
./01_basic_chat_SAMBA_minimal.sh     # SambaNova
./01_basic_chat_DEMETERICS_minimal.sh # Demeterics proxy
./01_basic_chat_GEMINI_minimal.sh    # Google Gemini 2.5 Flash
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
| **01 Basic Chat** | `01_basic_chat_minimal.sh` | `01_basic_chat_full.sh` | Ask AI a question (Groq) |
| **01 Basic Chat - OpenAI** | `01_basic_chat_OPENAI_minimal.sh` | - | Ask AI a question (OpenAI) |
| **01 Basic Chat - Anthropic** | `01_basic_chat_ANTHROPIC_minimal.sh` | - | Ask AI a question (Claude) |
| **01 Basic Chat - SambaNova** | `01_basic_chat_SAMBA_minimal.sh` | - | Ask AI a question (SambaNova) |
| **01 Basic Chat - Gemini** | `01_basic_chat_GEMINI_minimal.sh` | - | Ask AI a question (Google Gemini 2.5 Flash) |
| **01 Basic Chat - Demeterics** | `01_basic_chat_DEMETERICS_minimal.sh` | - | Ask AI a question (via proxy) |
| **02 System Prompt** | `02_system_prompt_minimal.sh` | `02_system_prompt_full.sh` | Control AI behavior with instructions |
| **03 Vision** | `03_vision_minimal.sh` | `03_vision_full.sh` | Analyze images with AI |
| **04 Safety Check** | `04_safety_check_minimal.sh` | `04_safety_check_full.sh` | Moderate text content |
| **05 Image Safety** | `05_image_safety_check_minimal.sh` | `05_image_safety_check_full.sh` | Moderate image content |
| **06 Prompt Guard** | `06_prompt_guard_minimal.sh` | `06_prompt_guard_full.sh` | Detect prompt injection attacks |
| **07 Whisper** | `07_whisper_minimal.sh` | `07_whisper_full.sh` | Transcribe audio to text |
| **08 Tavily Search** | `08_tavily_search_minimal.sh` | `08_tavily_search_full.sh` | Web search with AI-powered answers |
| **09 Tavily Extract** | `09_tavily_extract_minimal.sh` | `09_tavily_extract_full.sh` | Extract clean content from web pages |
| **10 Tool Use** | `10_tool_use_minimal.sh` | `10_tool_use_full.sh` | AI agents with function calling |
| **11 Web Search (Groq)** | `11_web_search_minimal.sh` | `11_web_search_full.sh` | groq/compound-mini with built-in web search |
| **12 Code Execution** | `12_code_execution_minimal.sh` | `12_code_execution_full.sh` | openai/gpt-oss-20b Python execution |
| **13 Reasoning** | `13_reasoning_minimal.sh` | `13_reasoning_full.sh` | Step-by-step thinking with prompt caching |
| **14 Text-to-Speech** | `14_text_to_speech_minimal.sh` | `14_text_to_speech_full.sh` | OpenAI TTS with 11 voices |

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
| **Demeterics (Default)** | `api.demeterics.com/groq` | Groq's Llama 4 family | Speed + observability |
| **OpenAI** | `api.openai.com` | GPT-4o, GPT-5 mini | Advanced reasoning, TTS |
| **Anthropic** | `api.anthropic.com` | Claude Haiku/Sonnet/Opus | Long context, policy-aware |
| **SambaNova** | `api.sambanova.ai` | Llama 3.1 variants | Open models, enterprise focus |
| **Google Gemini** | `generativelanguage.googleapis.com` | Gemini 2.0/2.5 Flash & Pro | Classroom-friendly multimodal prompts |

### About Demeterics

Demeterics is a universal observability layer that sits in front of Groq, Gemini, OpenAI, and Anthropic. Every request goes to `https://api.demeterics.com/<provider>/v1/...` with your **Managed LLM Key** (`DEMETERICS_API_KEY`). Benefits:
- Track token usage and costs across providers in one dashboard
- Monitor performance/latency and compare models
- Analyze prompt effectiveness with metadata + prompt comments
- Debug production issues with request/response replay
- Generate compliance reports with audit-ready exports

All Groq-focused scripts in this folder now call `https://api.demeterics.com/groq/v1/...`, so you keep Groq's speed with Demeterics observability.

## 💡 Example Comparison

### Minimal Version Output:
```json
{"id":"chatcmpl-123","object":"chat.completion","created":1234567890,"model":"meta-llama/llama-4-scout-17b-16e-instruct","choices":[{"index":0,"message":{"role":"assistant","content":"The capital of Switzerland is Bern."},"finish_reason":"stop"}],"usage":{"prompt_tokens":15,"completion_tokens":8,"total_tokens":23}}
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
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
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
  Input cost:  $0.00000165
  Output cost: $0.00000272
  Total cost:  $0.00000437
```

## 🛠️ Prerequisites

### For Minimal Scripts:
- `curl` (usually pre-installed)
- `DEMETERICS_API_KEY` environment variable

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

1. Go to [demeterics.com](https://demeterics.com)
2. Sign up (100 free credits!)
3. Open **Managed LLM Keys** in the dashboard
4. Create a new key (looks like `dmt_...`)
5. Export it:
   ```bash
   export DEMETERICS_API_KEY="dmt_your_key_here"
   ```

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

## 💰 Pricing (as of Oct 2025)

| Model | Input | Output | Use Case |
|-------|-------|--------|----------|
| Llama 4 Scout 17B | $0.11/1M | $0.34/1M | General chat + vision |
| LlamaGuard 4 12B | $0.20/1M | $0.20/1M | Content moderation (text + images) |
| Prompt Guard 2 86M | $0.04/1M | $0.04/1M | Injection detection |
| Whisper Large v3 Turbo | $0.04/hour of audio | - | Speech-to-text |

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
- Your `DEMETERICS_API_KEY` is not set or invalid
- Solution: `export DEMETERICS_API_KEY="dmt_..."`

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
- **[Exercise 11: Web Search (Groq)](../exercises/11_web_search.md)** - Built-in search with groq/compound-mini
- **[Exercise 12: Code Execution](../exercises/12_code_execution.md)** - Python execution with openai/gpt-oss-20b
- **[Exercise 13: Reasoning](../exercises/13_reasoning.md)** - Step-by-step thinking with prompt caching
- **[Exercise 14: Text-to-Speech](../exercises/14_text_to_speech.md)** - Voice synthesis with 11 voices

Each exercise includes progressive challenges, real-world applications, and reflection questions.

## 📚 Additional Resources

- [Demeterics Docs](https://demeterics.com/docs)
- [jq Manual](https://jqlang.github.io/jq/manual/)
- [cURL Documentation](https://curl.se/docs/)

## 🤝 Contributing

Found a bug? Have an improvement? PRs welcome!

## 📄 License

MIT - Do whatever you want with these scripts!

---

**Happy scripting! 🚀**
