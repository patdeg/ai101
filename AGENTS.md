# AGENTS.md

This file provides guidance to AI coding assistants (GitHub Copilot, Cursor, OpenAI Codex, etc.) when working with code in this repository.

## Project Overview

**ai101** is an educational repository demonstrating how to use the Groq API across 5 different languages/platforms. This is a **training project** designed for beginners (starting with a 14-year-old student named Victor), so code clarity and educational value are the top priorities.

### Primary Goal
Create a "Rosetta Stone" of AI API examples that teach programming fundamentals while demonstrating practical AI integration.

## Repository Structure

```
ai101/
├── curl/          # Bash/cURL examples (no dependencies)
├── nodejs/        # Node.js examples (built-in modules only)
├── python/        # Python examples (standard library only)
├── go/            # Go examples (standard library only)
├── arduino/       # Arduino/ESP32 examples (WiFi-enabled boards)
├── test_image.jpg # Shared test image (300x450, 58KB)
├── README.md      # Main documentation
├── CLAUDE.md      # Guidance for Claude Code
└── AGENTS.md      # This file
```

## The 7 Examples (Repeated Across Languages)

1. **01_basic_chat** - Simple question/answer with AI
2. **02_system_prompt** - Controlling AI behavior with system messages
3. **03_vision** - Image analysis using vision models
4. **04_safety_check** - Content moderation with LlamaGuard (text)
5. **05_image_safety_check** - Image content moderation with LlamaGuard (vision)
6. **06_prompt_guard** - Jailbreak/injection detection
7. **07_whisper** - Audio transcription with Whisper

**Note**: Not all examples exist in all languages yet. Python, Node.js, and Go have examples 1-4, 6-7 (missing 05). Arduino has 01, 02, 03, 04, 06, 07. cURL has all 7 in dual versions (minimal + full).

## Coding Standards

### 1. Educational First, Production Second
- **80% of lines should be code/comments/documentation**
- Prioritize clarity over cleverness
- Explain WHY, not just WHAT
- Use step-by-step numbered comments (Step 1, Step 2, etc.)

### 2. No External Dependencies (Except Arduino)
- **curl/**: Pure bash and curl
- **nodejs/**: Only `https`, `fs`, `path`, `os` (built-in modules)
- **python/**: Only `http.client`, `json`, `os`, `base64` (standard library)
- **go/**: Only standard library packages
- **arduino/**: ArduinoJson, WiFiClientSecure (necessary for ESP32/ESP8266)

### 3. Comment Style Requirements

**Header Block** (every file):
```
What this demonstrates:
  - Feature 1
  - Feature 2

What you'll learn:
  - Concept 1
  - Concept 2

Prerequisites:
  - Requirement 1
  - Requirement 2

Expected output:
  - What the user will see
```

**Inline Comments** (before critical operations):
```javascript
// Step 1: Get API key from environment variable
// The API key is sensitive, so we use environment variables
const apiKey = process.env.GROQ_API_KEY;

// Step 2: Create the request payload
// This defines what we're sending to the API
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",
  messages: [...]
});
```

### 4. Language-Specific Guidelines

**Bash/cURL:**
- **Dual versions**: Create both `*_minimal.sh` (with `/bin/sh`, no jq) and `*_full.sh` (with `/bin/bash`, jq, bc)
- **Minimal**: Raw JSON output, POSIX-compliant, curl only, for scripting/CI/CD
- **Full**: Pretty-printed output, cost calculations, extensive documentation, for learning
- **Use heredoc for complex JSON** (eliminates `\"` escaping):
  ```bash
  curl -d @- <<EOF
  {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [{"role": "user", "content": "Hello"}]
  }
  EOF
  ```
- Explain every curl flag inline
- Show environment variable usage

**Node.js:**
- Use `require()` not `import` (CommonJS for simplicity)
- Explain event-driven programming (on('data'), on('end'))
- Show callback patterns clearly
- Handle errors explicitly

**Python:**
- Use `#!/usr/bin/env python3` shebang
- Include comprehensive docstrings
- Use f-strings for formatting
- Explain context managers (`with` statement)

**Go:**
- Define all structs with JSON tags
- Explain struct field capitalization (exported vs unexported)
- Show error handling after every operation
- Comment on pointers and memory

**Arduino:**
- **Target hardware**: Seeed XIAO ESP32-S3 Sense ($23.99 on Amazon)
- **Built-in components**: PDM microphone, OV2640 camera, WiFi, 8MB PSRAM
- **No external components required** - everything is on-board
- All logic in `setup()`, leave `loop()` empty
- Print everything to Serial (115200 baud)
- Monitor memory usage (heap)
- Handle WiFi connection failures gracefully
- Examples 03 (vision) and 07 (whisper) use built-in camera and microphone

### 5. API Usage

**Models:**
- Chat/Vision: `meta-llama/llama-4-scout-17b-16e-instruct`
- Safety (Text + Vision): `meta-llama/llama-guard-4-12b`
- Prompt Guard: `meta-llama/llama-prompt-guard-2-86m`
- Whisper Audio: `whisper-large-v3-turbo`

**Pricing (October 2025):**
- Llama 4 Scout 17B: Input $0.11/1M, Output $0.34/1M
- LlamaGuard 4 12B: Input $0.20/1M, Output $0.20/1M
- Prompt Guard 2 86M: Input $0.04/1M, Output $0.04/1M
- Whisper Turbo: **$0.04 per hour of audio**

**Endpoints:**
```
POST https://api.groq.com/openai/v1/chat/completions (chat, vision, safety, prompt guard)
POST https://api.groq.com/openai/v1/audio/transcriptions (whisper)
```

**Authentication:**
```
Authorization: Bearer $GROQ_API_KEY
```

**Common Parameters:**
- `temperature`: 0.0-2.0 (0.7 is balanced, 0.3 for factual, 0.5 for system prompts)
- `max_tokens`: 100-500 typically
- `messages`: Array with roles: "system", "user", "assistant"

### 6. Vision API Specifics

**Image Path:**
- All examples use `../test_image.jpg` (300x450, 58KB)
- Base64 encode before sending
- Use data URL format: `data:image/jpeg;base64,<encoded_data>`

**Content Structure:**
```json
{
  "content": [
    {"type": "text", "text": "What's in this image?"},
    {"type": "image_url", "image_url": {"url": "data:image/jpeg;base64,..."}}
  ]
}
```

**Size Limits:**
- Base64 embedded: 4 MB max
- URL (remote): 20 MB max
- Resolution: 33 megapixels max

### 7. Safety and Security Examples

**LlamaGuard Response Format:**
- Safe: `"safe"`
- Unsafe: `"unsafe\nS<number>"` (e.g., `"unsafe\nS6"`)

**14 Safety Categories:**
```
S1:  Violent Crimes
S2:  Non-Violent Crimes
S3:  Sex-Related Crimes
S4:  Child Sexual Exploitation
S5:  Defamation
S6:  Specialized Advice (financial, medical, legal)
S7:  Privacy Violations
S8:  Intellectual Property
S9:  Indiscriminate Weapons
S10: Hate Speech
S11: Suicide & Self-Harm
S12: Sexual Content
S13: Elections
S14: Code Interpreter Abuse
```

**Prompt Guard Response Format:**
- `"BENIGN"` - Safe message
- `"JAILBREAK"` - Trying to bypass rules
- `"INJECTION"` - Trying to inject commands

## Common Tasks

### Adding a New Example
1. Create files in all 5 language directories
2. Use consistent numbering (06_*, 07_*, etc.)
3. Follow the existing pattern for headers and comments
4. Update each language's README.md
5. Test in each environment

### Modifying Existing Examples
1. Read the file first to understand the flow
2. Maintain the step-by-step comment structure
3. Keep educational comments at ~80% of content
4. Preserve the "educational first" philosophy
5. Test changes before committing

### Creating Documentation
- Use clear, simple language (14-year-old reading level)
- Include code examples for every concept
- Show expected output
- Provide troubleshooting tips
- Link to relevant Groq documentation

## Educational Philosophy

### Target Audience
- **Primary:** 14-year-old Victor (first exposure to AI APIs)
- **Secondary:** Anyone learning AI API integration
- **Assumption:** Basic familiarity with one programming language

### Learning Progression
1. **Example 1:** HTTP requests, JSON, API basics
2. **Example 2:** Message roles, AI behavior control
3. **Example 3:** Multimodal AI, image file handling, base64 encoding
4. **Example 4:** Content safety (text), string parsing, production concerns
5. **Example 5:** Content safety (images), vision moderation
6. **Example 6:** Security layers, attack detection, defense-in-depth
7. **Example 7:** Audio transcription, multipart form data, speech-to-text

### Key Teaching Principles
- Show, don't just tell
- Explain WHY before HOW
- One concept at a time
- Build on previous examples
- Real-world use cases
- Error handling as learning opportunities

## File Naming Conventions

- Examples: `01_basic_chat.{sh,js,py,go,ino}`
- cURL minimal: `01_basic_chat_minimal.sh` (and 02-07)
- cURL full: `01_basic_chat_full.sh` (and 02-07)
- Arduino: `01_basic_chat/01_basic_chat.ino` (in subdirectories)
- README files: `README.md` (one per language directory)
- Test resources: `test_image.jpg`, `gettysburg.mp3` (root level)
- Documentation: `*.md` (root level)

## Testing Requirements

Before committing changes to examples:

1. **Bash/cURL:** Run script, verify output
2. **Node.js:** Run with `node filename.js`, no errors
3. **Python:** Run with `python3 filename.py`, no errors
4. **Go:** Compile with `go run filename.go`, verify output
5. **Arduino:** Verify compilation (no need to upload)

## Common Pitfalls to Avoid

1. ❌ **Don't use external libraries** (except Arduino necessities)
2. ❌ **Don't hardcode API keys** (always use environment variables)
3. ❌ **Don't skip error handling** (educational opportunity!)
4. ❌ **Don't use `\"` escaping in JSON** (use heredoc instead)
5. ❌ **Don't use jargon without explanation**
6. ❌ **Don't write production code** (optimize for learning)
7. ❌ **Don't create files unless necessary** (modify existing when possible)
8. ❌ **Don't add emojis** unless explicitly requested
9. ❌ **Don't create documentation files proactively** (README.md only when needed)

## What to Prioritize

1. ✅ **Code clarity and readability**
2. ✅ **Educational value of comments** (80% of content)
3. ✅ **Consistency across languages**
4. ✅ **Heredoc pattern for JSON** (no escaping)
5. ✅ **Error messages that teach**
6. ✅ **Step-by-step execution flow**
7. ✅ **Real-world applicability**
8. ✅ **Security awareness** (examples 4, 5, 6)

## Environment Variables

All examples expect:
```bash
export GROQ_API_KEY="gsk_your_api_key_here"
```

Arduino examples use constants instead:
```cpp
const char* GROQ_API_KEY = "your_key_here";
```

## Response Format Examples

### Chat Completion
```json
{
  "id": "chatcmpl-123",
  "choices": [{
    "message": {
      "role": "assistant",
      "content": "The capital of Switzerland is Bern."
    },
    "finish_reason": "stop"
  }],
  "usage": {
    "prompt_tokens": 15,
    "completion_tokens": 8,
    "total_tokens": 23
  }
}
```

### Vision Response
Same structure, but content describes the image.

### Safety Check
```json
{
  "choices": [{
    "message": {
      "content": "safe"
    }
  }]
}
```

or

```json
{
  "choices": [{
    "message": {
      "content": "unsafe\nS6"
    }
  }]
}
```

### Prompt Guard
```json
{
  "choices": [{
    "message": {
      "content": "BENIGN"
    }
  }]
}
```

## When Making Changes

1. **Read the existing code first** - understand the pattern
2. **Maintain consistency** - match the style of existing files
3. **Test your changes** - verify they work
4. **Update documentation** - if you change behavior
5. **Consider all languages** - changes should apply across implementations
6. **Think educational** - will a beginner understand this?

## Resources

- **Groq Console:** https://console.groq.com
- **API Documentation:** https://console.groq.com/docs
- **Model Information:** https://console.groq.com/docs/models
- **Vision API Guide:** https://console.groq.com/docs/vision

## Notes for AI Assistants

- This is a **learning repository**, not production code
- Prioritize **clarity** over efficiency
- Explain **every concept** as if to a beginner
- Use **consistent patterns** across all examples
- **Test before suggesting** - verify code actually works
- **Preserve educational comments** - don't remove them for brevity
- **Match the existing style** - this repository has a specific voice

## Summary

When working on this repository:
1. Educational value > Production quality
2. 80% code/comments > Minimal comments
3. Standard library > External dependencies
4. Heredoc > Escaped JSON
5. Clarity > Cleverness
6. Consistency > Innovation
7. Dual versions for cURL (minimal + full)
8. Safety awareness > Feature completeness

**Remember:** Victor is 14 and learning. Make every line count as a teaching opportunity.
