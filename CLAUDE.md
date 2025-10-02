# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is `ai101` - an educational repository demonstrating Groq API usage across 5 languages/platforms. This is a **training project** for beginners (starting with a 14-year-old student named Victor), where **code clarity and educational value are the top priorities**.

### Primary Goal
Create a "Rosetta Stone" of AI API examples that teach programming fundamentals while demonstrating practical AI integration.

## Directory Structure

- `curl/` - Bash/cURL examples (14 files: 7 examples × 2 versions each)
- `nodejs/` - Node.js examples (6 examples, built-in modules only)
- `python/` - Python examples (6 examples, standard library only)
- `go/` - Go examples (6 examples, standard library only)
- `arduino/` - Arduino/ESP32 examples (for XIAO ESP32-S3 Sense board)

## The 7 Examples (Repeated Across Languages)

1. **01_basic_chat** - Simple question/answer with AI
2. **02_system_prompt** - Controlling AI behavior with system messages
3. **03_vision** - Image analysis using vision models
4. **04_safety_check** - Content moderation with LlamaGuard (text)
5. **05_image_safety_check** - Image content moderation (curl only currently)
6. **06_prompt_guard** - Jailbreak/injection detection
7. **07_whisper** - Audio transcription with Whisper

## cURL Directory: Dual Version Strategy

Each cURL example has **two versions**:

### Minimal Version (`*_minimal.sh`)
- **Shell**: `/bin/sh` (POSIX-compliant, works everywhere)
- **Dependencies**: Only `curl` (no `jq` required)
- **Output**: Raw JSON response
- **Documentation**: Concise parameter explanations
- **Use case**: Quick testing, scripting, CI/CD, minimal environments

### Full Version (`*_full.sh`)
- **Shell**: `/bin/bash` (more features)
- **Dependencies**: `curl` + `jq` for JSON parsing, `bc` for calculations
- **Output**: Pretty-printed, parsed, formatted with cost breakdown
- **Documentation**: Extensive explanations, examples, best practices
- **Use case**: Learning, debugging, production workflows, tracking costs

## Development Approach

### 1. Educational First, Production Second
- **80% of lines should be code/comments/documentation**
- Prioritize clarity over cleverness
- Explain WHY, not just WHAT
- Use step-by-step numbered comments

### 2. No External Dependencies (Except Arduino)
- **curl/**: Pure bash and curl (minimal), or bash + jq + bc (full)
- **nodejs/**: Only built-in modules (`https`, `fs`, `path`, `os`)
- **python/**: Only standard library (`http.client`, `json`, `os`, `base64`)
- **go/**: Only standard library packages
- **arduino/**: ArduinoJson, WiFiClientSecure, ESP32 camera/audio libraries

### 3. JSON Handling in Shell Scripts

**Use Heredoc Pattern** (eliminates `\"` escaping):
```bash
curl -d @- <<EOF
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": [{
    "role": "user",
    "content": [
      {"type": "text", "text": "What's in this image?"},
      {"type": "image_url", "image_url": {"url": "data:image/jpeg;base64,$IMAGE_BASE64"}}
    ]
  }]
}
EOF
```

**Why**: Zero escaping needed, perfect JSON formatting, variables expand naturally.

### 4. Hardware Platform: XIAO ESP32-S3 Sense

**Arduino examples target**: [Seeed XIAO ESP32-S3 Sense](https://www.amazon.com/Seeed-Studio-XIAO-ESP32-Sense/dp/B0C69FFVHH) ($14)

**Built-in Components**:
- PDM microphone (for audio transcription)
- OV2640 camera (for vision analysis)
- WiFi (for API calls)
- 8MB PSRAM (for image/audio buffers)

**No external components required** - everything is on-board.

**Examples**: 03_vision (camera), 07_whisper (microphone)

### 5. API Models and Pricing (October 2025)

**Chat/Vision**: `meta-llama/llama-4-scout-17b-16e-instruct`
- Input: $0.11/1M tokens
- Output: $0.34/1M tokens

**Safety (Text + Vision)**: `meta-llama/llama-guard-4-12b`
- Input: $0.20/1M tokens
- Output: $0.20/1M tokens

**Prompt Guard**: `meta-llama/llama-prompt-guard-2-86m`
- Input: $0.04/1M tokens
- Output: $0.03/1M tokens

**Whisper Audio**: `whisper-large-v3-turbo`
- **$0.04 per hour of audio**

### 6. When Adding New Examples

1. Create files in all language directories (or document which are missing)
2. Use consistent numbering (01-07)
3. For cURL: Create both `*_minimal.sh` and `*_full.sh`
4. Follow existing pattern for headers and comments
5. Use heredoc for complex JSON in shell scripts
6. Update README.md files
7. Test in each environment

### 7. When Modifying Examples

1. **Read the file first** - understand the pattern
2. **Maintain consistency** - match existing style
3. **Preserve educational comments** - don't remove for brevity
4. **Use heredoc** for JSON in shell scripts (avoid `\"`)
5. **Test changes** before committing
6. **Consider all languages** - apply changes across implementations when applicable

## File Naming Conventions

- Examples: `01_basic_chat.{sh,js,py,go,ino}`
- cURL minimal: `01_basic_chat_minimal.sh` (and 02-07)
- cURL full: `01_basic_chat_full.sh` (and 02-07)
- Arduino: `01_basic_chat/01_basic_chat.ino` (in subdirectories)
- README files: `README.md` (one per language directory)
- Test resources: `test_image.jpg`, `gettysburg.mp3` (root level)
- Documentation: `*.md` (root level)

## Common Pitfalls to Avoid

1. ❌ **Don't use external libraries** (except Arduino necessities)
2. ❌ **Don't hardcode API keys** (always use environment variables)
3. ❌ **Don't skip error handling** (educational opportunity!)
4. ❌ **Don't use `\"` escaping in JSON** (use heredoc instead)
5. ❌ **Don't write production code** (optimize for learning)
6. ❌ **Don't create files unless necessary** (modify existing when possible)
7. ❌ **Don't add emojis** unless explicitly requested
8. ❌ **Don't create documentation files proactively** (only when needed)

## What to Prioritize

1. ✅ **Code clarity and readability**
2. ✅ **Educational value of comments** (80% of content)
3. ✅ **Consistency across languages**
4. ✅ **Heredoc pattern for JSON** (no escaping)
5. ✅ **Error messages that teach**
6. ✅ **Step-by-step execution flow**
7. ✅ **Security awareness** (examples 4, 5, 6)

## Educational Philosophy

**Target Audience**: 14-year-old Victor (first exposure to AI APIs) and anyone learning AI API integration.

**Learning Progression**:
1. HTTP requests, JSON, API basics
2. Message roles, AI behavior control
3. Multimodal AI, file handling, encoding
4. Content safety, string parsing
5. Image safety, vision moderation
6. Security layers, attack detection
7. Audio transcription, speech-to-text

**Key Principles**: Show don't tell, explain WHY before HOW, one concept at a time, build on previous examples, real-world use cases.

## Resources

- **Groq Console:** https://console.groq.com
- **API Documentation:** https://console.groq.com/docs
- **Model Information:** https://console.groq.com/docs/models
- **Vision API Guide:** https://console.groq.com/docs/vision
- **XIAO ESP32-S3 Sense:** https://www.seeedstudio.com/XIAO-ESP32S3-Sense-p-5639.html

## Summary

When working on this repository:
1. Educational value > Production quality
2. 80% code/comments > Minimal comments
3. Standard library > External dependencies
4. Heredoc > Escaped JSON
5. Clarity > Cleverness
6. Consistency > Innovation
7. Dual versions for cURL (minimal + full)

**Remember**: Victor is 14 and learning. Make every line count as a teaching opportunity.
