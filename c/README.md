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

### 3. Set your API key

```bash
export DEMETERICS_API_KEY="your-api-key-here"
```

## Compiling Examples

### If cJSON is in lib/ folder:
```bash
gcc -o 01_basic_chat 01_basic_chat.c lib/cJSON.c -lcurl -I lib/
./01_basic_chat
```

### If cJSON is installed system-wide:
```bash
gcc -o 01_basic_chat 01_basic_chat.c -lcurl -lcjson
./01_basic_chat
```

### Compile all examples (with lib/):
```bash
for f in *.c; do
    gcc -o "${f%.c}" "$f" lib/cJSON.c -lcurl -I lib/ 2>/dev/null && echo "Built: ${f%.c}"
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
| 12 | `12_web_search.c` | Built-in web search with groq/compound-mini |
| 13 | `13_code_execution.c` | Python code execution |
| 14 | `14_reasoning.c` | Step-by-step thinking with prompt caching |
| 15 | `15_text_to_speech.c` | Voice synthesis with OpenAI TTS |

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

### "DEMETERICS_API_KEY not set"
Set your environment variable:
```bash
export DEMETERICS_API_KEY="your-key-here"
```

## Resources

- [cJSON Documentation](https://github.com/DaveGamble/cJSON)
- [libcurl Tutorial](https://curl.se/libcurl/c/libcurl-tutorial.html)
- [C Programming Tutorial](https://www.learn-c.org/)
