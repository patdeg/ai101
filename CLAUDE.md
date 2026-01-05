# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

---

## Blue Fermion Brand Alignment

This project is part of the **Blue Fermion Labs** portfolio. As such, it follows the universal prime objectives that govern all Blue Fermion properties.

### Universal Prime Objectives

| # | Objective | Rationale |
|---|-----------|-----------|
| **U1** | **Never deploy to production without explicit user permission** | Prevents accidental releases; ensures human-in-the-loop for all production changes |
| **U2** | **Never destroy user work with git checkout/reset** | Learned from production incident; always preserve uncommitted changes |
| **U3** | **Documentation in `/docs/` only** | Root folder contains only README.md, CLAUDE.md, AGENTS.md; keeps projects clean |
| **U4** | **Use the shared common library** | `github.com/patdeg/common` provides logging, utilities, and LLM integration (when applicable) |
| **U5** | **Use Demeterics for LLM observability** | All LLM calls should be tagged with APP, FLOW, ENV, USER for cost tracking |

### Blue Fermion Labs Objectives (Cross-Product)

| # | Objective | Context |
|---|-----------|---------|
| **L1** | **Simplicity First** | Vanilla Bootstrap 5, simple HTMX, basic Go handlers |
| **L2** | **No Hallucination** | Temperature 0.0 for factual extraction; strict prompts |
| **L3** | **Fail Gracefully** | Fallback chains (Groq -> OpenAI -> Error message) |
| **L4** | **Cache Smart** | 15min PDF cache, 30min sessions, use memcache |
| **L5** | **Security Always** | No PII in keys/logs; validate all inputs |

### Brand Context

- **Parent Entity:** Blue Fermion, LLC
- **Business Line:** Blue Fermion Labs (SaaS/Educational Products)
- **Brand Promise:** "Deploy AI with the Rigor of CERN and the Speed of Google"
- **Primary Color:** Teal (#1DA7A0) with Navy (#123C6B) secondary
- **Voice:** Rigorous, Direct, Confident, Accessible, Human

---

## Repository Overview

This is `ai101` - an educational repository teaching **responsible AI development** across 7 languages/platforms. This is a **training project** for beginners (starting with a 14-year-old student named Victor), where **code clarity, educational value, and AI safety are the top priorities**.

### Primary Goal
Create a "Rosetta Stone" of AI API examples that teach programming fundamentals while demonstrating practical AI integration **with safety built in from day one**. Every learner should understand not just what they *can* build, but what they *should* build responsibly.

## Directory Structure

- `bash/` - Bash/cURL examples (32 files: 16 examples × 2 versions each)
- `nodejs/` - Node.js examples (16 examples, built-in modules only)
- `python/` - Python examples (16 examples, standard library only)
- `go/` - Go examples (16 examples, standard library only)
- `c/` - C examples (16 examples, libcurl + cJSON)
- `cpp/` - C++ examples (16 examples, libcurl + nlohmann/json)
- `arduino/` - Arduino/ESP32 examples (for XIAO ESP32-S3 Sense board, examples 1-8 only)
- `exercises/` - Hands-on practice exercises (16 files, one per example)

## The 16 Examples (Repeated Across Languages)

1. **01_basic_chat** - Simple question/answer with AI
2. **02_system_prompt** - Controlling AI behavior with system messages
3. **03_prompt_template** - Dynamic prompt compilation with templates
4. **04_vision** - Image analysis using vision models
5. **05_safety_check** - Content moderation with LlamaGuard (text)
6. **06_image_safety_check** - Image content moderation
7. **07_prompt_guard** - Jailbreak/injection detection
8. **08_whisper** - Audio transcription with Whisper
9. **09_tavily_search** - Web search with AI-powered answers (Tavily API)
10. **10_tavily_extract** - Extract clean content from web pages (Tavily API)
11. **11_tool_use** - AI agents with function calling (Groq + Tavily)
12. **12_web_search** - Built-in web search with groq/compound-mini
13. **13_code_execution** - Python code execution with openai/gpt-oss-20b
14. **14_reasoning** - Step-by-step thinking with openai/gpt-oss-20b + prompt caching
15. **15_text_to_speech** - Voice synthesis with OpenAI gpt-4o-mini-tts (11 voices)
16. **16_podcast** - Multi-speaker podcast generation with Google Gemini TTS (30 voices)

## Bash Directory: Dual Version Strategy

Each bash example has **two versions**:

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

### 2. No External Dependencies (Except Arduino, C, C++)
- **bash/**: Pure bash and curl (minimal), or bash + jq + bc (full)
- **nodejs/**: Only built-in modules (`https`, `fs`, `path`, `os`)
- **python/**: Only standard library (`http.client`, `json`, `os`, `base64`)
- **go/**: Only standard library packages
- **c/**: libcurl (HTTP) + cJSON (JSON) - both widely available, cJSON can be bundled
- **cpp/**: libcurl (HTTP) + nlohmann/json (JSON) - header-only JSON lib can be bundled
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

**Arduino examples target**: [Seeed XIAO ESP32-S3 Sense](https://www.amazon.com/Seeed-Studio-XIAO-ESP32-Sense/dp/B0C69FFVHH) ($23.99)

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
- Output: $0.04/1M tokens

**Whisper Audio**: `whisper-large-v3-turbo`
- **$0.04 per hour of audio**

**Reasoning + Code Execution**: `openai/gpt-oss-20b`
- Input: $0.10/1M tokens ($0.05/1M cached - 50% discount)
- Output: $0.50/1M tokens

**Text-to-Speech** (OpenAI): `gpt-4o-mini-tts`
- Input: $0.60/1M tokens (≈ characters)
- Output: $12/1M tokens (audio duration-based)
- 11 voices: alloy, ash, ballad, coral, echo, fable, onyx, nova, sage, shimmer, verse

**Multi-Speaker TTS** (Demeterics): `POST /tts/v1/generate`
- Single API call generates audio with multiple distinct speakers
- 30 voices: Puck (upbeat), Kore (firm), Charon (informative), Zephyr (bright),
  Fenrir (excitable), Leda (youthful), Aoede (breezy), Sulafat (warm), Achird (friendly), etc.
- Max 2 speakers per request
- Uses DEMETERICS_API_KEY (same key as other examples)
- Documentation: https://demeterics.ai/docs/speech

**Tavily API** (for examples 08-09):
- Web search and content extraction
- Authentication: `Authorization: Bearer <TAVILY_API_KEY>` header
- Endpoints: `/search`, `/extract`
- Free tier available at https://tavily.com

### 6. When Adding New Examples

1. Create files in all language directories (or document which are missing)
2. Use consistent numbering (01-14, or next available)
3. For bash: Create both `*_minimal.sh` and `*_full.sh`
4. Follow existing pattern for headers and comments
5. Use heredoc for complex JSON in shell scripts
6. Update all README.md files (main + all language directories)
7. Add corresponding exercise file in `exercises/` directory
8. Update `exercises/README.md` index table
9. For tool use examples: Define tools with OpenAI-compatible schema
10. Test in each environment

### 7. When Modifying Examples

1. **Read the file first** - understand the pattern
2. **Maintain consistency** - match existing style
3. **Preserve educational comments** - don't remove for brevity
4. **Use heredoc** for JSON in shell scripts (avoid `\"`)
5. **Test changes** before committing
6. **Consider all languages** - apply changes across implementations when applicable

## File Naming Conventions

- Examples: `01_basic_chat.{sh,js,py,go,c,cpp,ino}`
- Bash minimal: `01_basic_chat_minimal.sh` (and 02-16)
- Bash full: `01_basic_chat_full.sh` (and 02-16)
- C examples: `01_basic_chat.c` (and 02-16)
- C++ examples: `01_basic_chat.cpp` (and 02-16)
- Arduino: `01_basic_chat/01_basic_chat.ino` (in subdirectories, examples 01-07 only)
- README files: `README.md` (one per language directory, plus `exercises/README.md`)
- Exercise files: `exercises/01_basic_chat.md` (and 02-16)
- Test resources: `test_image.jpg`, `gettysburg.mp3` (root level)
- Documentation: `*.md` (root level)

## Common Pitfalls to Avoid

1. ❌ **Don't use external libraries** (except Arduino, C, C++ necessities)
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
7. ✅ **AI safety and responsible development** (integrated throughout, emphasized in examples 4-6)

## Educational Philosophy

**Target Audience**: 14-year-old Victor (first exposure to AI APIs) and anyone learning AI API integration.

**Learning Progression**:
1. HTTP requests, JSON, API basics
2. Message roles, AI behavior control
3. Template systems, dynamic prompt generation
4. Multimodal AI, file handling, encoding
5. **Content safety** (text) - LlamaGuard moderation (taught early!)
6. **Image safety** (vision) - Vision moderation (taught early!)
7. **Security layers** - Prompt injection defense (taught early!)
8. Audio transcription, speech-to-text
9. Web search APIs, Tavily integration
10. Content extraction from web pages
11. AI agents with function calling, tool use
12. Web search with groq/compound-mini
13. Code execution with sandboxing
14. Reasoning with prompt caching optimization
15. Text-to-speech with voice synthesis
16. Multi-speaker podcast generation with Gemini TTS

**Key Principles**:
- **Safety-first mindset** - AI safety taught in examples 5-7, before advanced features
- Show don't tell, explain WHY before HOW
- One concept at a time, build on previous examples
- Real-world use cases with responsible AI patterns

## Resources

- **Demeterics Docs:** https://demeterics.com/docs
- **Demeterics Prompt Guide:** https://demeterics.com/docs/prompt
- **Groq Model Information:** https://console.groq.com/docs/models
- **Groq Vision API Guide:** https://console.groq.com/docs/vision
- **Groq Tool Use Guide:** https://console.groq.com/docs/tool-use
- **Tavily API:** https://tavily.com
- **Tavily Documentation:** https://docs.tavily.com
- **XIAO ESP32-S3 Sense:** https://www.seeedstudio.com/XIAO-ESP32S3-Sense-p-5639.html

## Exercises Directory

The `exercises/` directory contains hands-on practice challenges designed using modern educational principles:

- **Progressive difficulty**: Understand → Apply → Analyze → Create
- **16 exercise files**: One per example (01-16)
- **Safety integration**: Exercises 5-7 focus on content moderation and security
- **Navigation**: Each file links to previous/next exercise and back to main index
- **Cross-references**: Exercise files reference related code examples in language directories
- **Real-world applications**: Build validators, pipelines, multi-step workflows, AI agents
- **Reflection questions**: Encourage deeper understanding

**Structure**:
- `exercises/README.md` - Index of all exercises with learning philosophy
- `exercises/01_basic_chat.md` - Temperature, tokens, cost tracking
- `exercises/02_system_prompt.md` - Personas, JSON mode, constraints
- `exercises/03_prompt_template.md` - Template systems, variables, conditionals
- ... (through 11 for Groq + Tavily examples)
- `exercises/12_web_search.md` - Groq built-in web search
- `exercises/13_code_execution.md` - Python code execution
- `exercises/14_reasoning.md` - Step-by-step thinking with caching
- `exercises/15_text_to_speech.md` - Voice synthesis with 11 voices
- `exercises/16_podcast.md` - Multi-speaker podcast generation

Each exercise file includes:
- What the user learned from the basic example
- 5-7 progressive exercises
- Real-world application ideas
- Reflection questions
- Links to related code examples

## Summary

When working on this repository:
1. **Safety-first mindset** > Feature completeness
2. Educational value > Production quality
3. 80% code/comments > Minimal comments
4. Standard library > External dependencies (except C/C++ which need libcurl + JSON lib)
5. Heredoc > Escaped JSON
6. Clarity > Cleverness
7. Consistency > Innovation
8. Dual versions for bash (minimal + full)
9. 16 examples + 16 exercises for complete learning path
10. 7 languages: bash, nodejs, python, go, c, cpp, arduino

**Remember**: Victor is 14 and learning AI for the first time. Make every line count as a teaching opportunity. Teach not just what AI *can* do, but what it *should* do responsibly.
