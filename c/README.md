# C Examples for AI101

Learn AI API integration with pure C! These examples use **libcurl** for HTTP requests and **cJSON** for JSON handling - both lightweight and widely available.

## Prerequisites

### 1. Install libcurl (Required)

**Ubuntu/Debian:**
```bash
sudo apt-get install libcurl4-openssl-dev
```

**macOS (Homebrew):**
```bash
brew install curl
```

**Windows (vcpkg):**
```bash
vcpkg install curl
```

### 2. Install cJSON (Required)

**Option A: Download directly (Recommended)**
```bash
cd lib/
curl -O https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.h
curl -O https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.c
```

**Option B: Package manager**
```bash
# Ubuntu/Debian
sudo apt-get install libcjson-dev

# macOS
brew install cjson
```

### 3. Set your API keys

Most examples call [OpenRouter](https://openrouter.ai) directly (one free-signup
key covers chat, vision, safety, prompt-guard, whisper/STT, TTS, and podcast):
```bash
export OPENROUTER_API_KEY="your-openrouter-key-here"
```

Two examples (13 code_execution, 14 reasoning) use Groq's native hosted
code-interpreter + prompt-caching, so they need a separate Groq key instead:
```bash
export GROQ_API_KEY="your-groq-key-here"
```

Examples 09/10 (Tavily search/extract) need `TAVILY_API_KEY` from
[tavily.com](https://tavily.com). Example 16 (podcast) also needs two Fish
Audio voice IDs (free account at [fish.audio](https://fish.audio)):
```bash
export FISH_VOICE_1="<voice-id-for-speaker-0>"
export FISH_VOICE_2="<voice-id-for-speaker-1>"
```

## Quick Start

```bash
# 1. Install libcurl (required)
sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
# or: brew install curl                     # macOS

# 2. Install cJSON library
make install

# 3. Build all examples
make

# 4. Run an example
./bin/01_basic_chat
```

## Compiling Examples

Executables are placed in the `bin/` folder (which is gitignored).

### Using Make (Recommended):
```bash
make          # Build all examples
make install  # Download cJSON library to lib/
make clean    # Remove all executables
make 01       # Build just example 01
make run-01   # Build and run example 01
make help     # Show all available targets
```

### Manual compilation (if cJSON is in lib/ folder):
```bash
gcc -o bin/01_basic_chat 01_basic_chat.c lib/cJSON.c -lcurl -I lib/
./bin/01_basic_chat
```

### If cJSON is installed system-wide:
```bash
gcc -o bin/01_basic_chat 01_basic_chat.c -lcurl -lcjson
./bin/01_basic_chat
```

### Compile all examples manually:
```bash
mkdir -p bin
for f in *.c; do
    gcc -o "bin/${f%.c}" "$f" lib/cJSON.c -lcurl -I lib/ 2>/dev/null && echo "Built: ${f%.c}"
done
```

## Examples Overview

| # | File | Description |
|---|------|-------------|
| 01 | `01_basic_chat.c` | Simple question/answer with AI |
| 02 | `02_system_prompt.c` | Controlling AI behavior with system messages |
| 03 | `03_prompt_template.c` | Dynamic prompt compilation with templates |
| 04 | `04_vision.c` | Image analysis using vision models |
| 05 | `05_safety_check.c` | Content moderation with LlamaGuard |
| 06 | `06_image_safety_check.c` | Image content moderation |
| 07 | `07_prompt_guard.c` | Jailbreak/injection detection |
| 08 | `08_whisper.c` | Audio transcription with Whisper |
| 09 | `09_tavily_search.c` | Web search with Tavily API |
| 10 | `10_tavily_extract.c` | Extract content from web pages |
| 11 | `11_tool_use.c` | AI agents with function calling |
| 12 | `12_web_search.c` | Built-in web search with OpenRouter's `:online` models |
| 13 | `13_code_execution.c` | Python code execution (direct on Groq) |
| 14 | `14_reasoning.c` | Step-by-step thinking with prompt caching (direct on Groq) |
| 15 | `15_text_to_speech.c` | Voice synthesis with Fish Audio (via OpenRouter) |
| 16 | `16_podcast.c` | Two-speaker podcast generation with Fish Audio (via OpenRouter) |

## C Concepts You'll Learn

- **Memory management**: malloc/free, avoiding memory leaks
- **String handling**: strcpy, strcat, sprintf, string formatting
- **Pointers**: Understanding *, &, and pointer arithmetic
- **Structs**: Custom data types for organizing data
- **Error handling**: Return codes, NULL checks
- **File I/O**: Reading files for base64 encoding
- **HTTP basics**: Request/response cycle with libcurl
- **JSON handling**: Parsing and creating JSON with cJSON

## Why C for AI?

While C isn't the typical choice for AI applications, learning it provides:
- Deep understanding of memory and performance
- Foundation for embedded AI (like our Arduino examples)
- Skills that transfer to C++, Rust, and system programming
- Appreciation for what higher-level languages do for you!

## Troubleshooting

### "curl/curl.h: No such file or directory"
libcurl development headers not installed. Run:
```bash
sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
brew install curl                           # macOS
```

### "undefined reference to cJSON_*"
cJSON not linked. Either:
1. Add `lib/cJSON.c` to your compile command
2. Or add `-lcjson` if installed system-wide

### "OPENROUTER_API_KEY not set" (or GROQ_API_KEY / TAVILY_API_KEY)
Set the environment variable the example needs (see "Set your API keys" above):
```bash
export OPENROUTER_API_KEY="your-key-here"
```

## Resources

- [cJSON Documentation](https://github.com/DaveGamble/cJSON)
- [libcurl Tutorial](https://curl.se/libcurl/c/libcurl-tutorial.html)
- [C Programming Tutorial](https://www.learn-c.org/)
