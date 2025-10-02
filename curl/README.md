# cURL Examples for Groq AI API

Shell script examples demonstrating Groq's AI APIs using plain cURL commands.

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

### Option 1: Minimal (Just Works™)
```bash
# Set your API key
export GROQ_API_KEY="gsk_..."

# Run any minimal script
./01_basic_chat_minimal.sh
```

### Option 2: Full (Beautiful Output)
```bash
# Install jq first (if needed)
# macOS: brew install jq
# Linux: sudo apt-get install jq
# Windows: choco install jq

# Set your API key
export GROQ_API_KEY="gsk_..."

# Run any full script
./01_basic_chat_full.sh
```

## 📋 Available Examples

| Example | Minimal | Full | What it does |
|---------|---------|------|--------------|
| **01 Basic Chat** | `01_basic_chat_minimal.sh` | `01_basic_chat_full.sh` | Ask AI a question |
| **02 System Prompt** | `02_system_prompt_minimal.sh` | `02_system_prompt_full.sh` | Control AI behavior with instructions |
| **03 Vision** | `03_vision_minimal.sh` | `03_vision_full.sh` | Analyze images with AI |
| **04 Safety Check** | `04_safety_check_minimal.sh` | `04_safety_check_full.sh` | Moderate text content |
| **05 Image Safety** | `05_image_safety_check_minimal.sh` | `05_image_safety_check_full.sh` | Moderate image content |
| **06 Prompt Guard** | `06_prompt_guard_minimal.sh` | `06_prompt_guard_full.sh` | Detect prompt injection attacks |
| **07 Whisper** | `07_whisper_minimal.sh` | `07_whisper_full.sh` | Transcribe audio to text |

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
- `GROQ_API_KEY` environment variable

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

1. Go to [console.groq.com](https://console.groq.com)
2. Sign up (free!)
3. Navigate to API Keys
4. Create a new key
5. Export it:
   ```bash
   export GROQ_API_KEY="gsk_your_key_here"
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
| Llama 4 Scout 17B | $0.11/1M | $0.34/1M | General chat |
| LlamaGuard 3 8B | $0.20/1M | $0.20/1M | Content moderation |
| Prompt Guard 86M | $0.20/1M | $0.20/1M | Injection detection |
| LLaVA 1.5 7B | $0.20/1M | $0.20/1M | Vision analysis |
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
- Your `GROQ_API_KEY` is not set or invalid
- Solution: `export GROQ_API_KEY="gsk_..."`

### "curl: command not found"
- Rare, but curl might not be installed
- Solution: Install curl via your package manager

### Raw JSON is hard to read
- You're using a minimal script
- Solution: Use the full version with `jq`

## 📚 Additional Resources

- [Groq API Documentation](https://console.groq.com/docs)
- [jq Manual](https://jqlang.github.io/jq/manual/)
- [cURL Documentation](https://curl.se/docs/)

## 🤝 Contributing

Found a bug? Have an improvement? PRs welcome!

## 📄 License

MIT - Do whatever you want with these scripts!

---

**Happy scripting! 🚀**
