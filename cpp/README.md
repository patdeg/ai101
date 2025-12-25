# C++ Examples for AI101

Learn AI API integration with modern C++! These examples use **libcurl** for HTTP requests and **nlohmann/json** for intuitive JSON handling.

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

### 2. Install nlohmann/json (Required)

**Option A: Download directly (Recommended)**
```bash
cd lib/
curl -O https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
```

**Option B: Package manager**
```bash
# Ubuntu/Debian
sudo apt-get install nlohmann-json3-dev

# macOS
brew install nlohmann-json
```

### 3. Set your API key

```bash
export DEMETERICS_API_KEY="your-api-key-here"
```

## Quick Start

```bash
# 1. Install libcurl (required)
sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
# or: brew install curl                     # macOS

# 2. Install nlohmann/json library
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
make install  # Download nlohmann/json library to lib/
make clean    # Remove all executables
make 01       # Build just example 01
make run-01   # Build and run example 01
make help     # Show all available targets
```

### Manual compilation (if json.hpp is in lib/ folder):
```bash
g++ -std=c++17 -o bin/01_basic_chat 01_basic_chat.cpp -lcurl -I lib/
./bin/01_basic_chat
```

### If nlohmann/json is installed system-wide:
```bash
g++ -std=c++17 -o bin/01_basic_chat 01_basic_chat.cpp -lcurl
./bin/01_basic_chat
```

### Compile all examples manually:
```bash
mkdir -p bin
for f in *.cpp; do
    g++ -std=c++17 -o "bin/${f%.cpp}" "$f" -lcurl -I lib/ 2>/dev/null && echo "Built: ${f%.cpp}"
done
```

## Examples Overview

| # | File | Description |
|---|------|-------------|
| 01 | `01_basic_chat.cpp` | Simple question/answer with AI |
| 02 | `02_system_prompt.cpp` | Controlling AI behavior with system messages |
| 03 | `03_prompt_template.cpp` | Dynamic prompt compilation with templates |
| 04 | `04_vision.cpp` | Image analysis using vision models |
| 05 | `05_safety_check.cpp` | Content moderation with LlamaGuard |
| 06 | `06_image_safety_check.cpp` | Image content moderation |
| 07 | `07_prompt_guard.cpp` | Jailbreak/injection detection |
| 08 | `08_whisper.cpp` | Audio transcription with Whisper |
| 09 | `09_tavily_search.cpp` | Web search with Tavily API |
| 10 | `10_tavily_extract.cpp` | Extract content from web pages |
| 11 | `11_tool_use.cpp` | AI agents with function calling |
| 12 | `12_web_search.cpp` | Built-in web search with groq/compound-mini |
| 13 | `13_code_execution.cpp` | Python code execution |
| 14 | `14_reasoning.cpp` | Step-by-step thinking with prompt caching |
| 15 | `15_text_to_speech.cpp` | Voice synthesis with OpenAI TTS |

## C++ Concepts You'll Learn

- **Modern C++ (C++17)**: Auto, lambdas, range-based for loops
- **Smart pointers**: Automatic memory management (no manual free!)
- **std::string**: Safe string handling (no buffer overflows)
- **Standard containers**: vector, map, optional
- **RAII**: Resource Acquisition Is Initialization
- **Error handling**: Exceptions and std::optional
- **nlohmann/json**: Intuitive JSON that feels like Python

## Why nlohmann/json?

This library makes JSON feel natural in C++:

```cpp
// Creating JSON (feels like Python/JavaScript!)
json j;
j["model"] = "meta-llama/llama-4-scout-17b-16e-instruct";
j["messages"] = {{{"role", "user"}, {"content", "Hello!"}}};
j["temperature"] = 0.7;

// Converting to string
std::string body = j.dump();

// Parsing JSON
json response = json::parse(response_string);
std::string answer = response["choices"][0]["message"]["content"];
```

## Why C++ for AI?

C++ combines performance with modern conveniences:
- **Performance**: Near-C speed when needed
- **Safety**: Smart pointers and RAII prevent memory leaks
- **Libraries**: Rich ecosystem (TensorFlow, PyTorch have C++ APIs)
- **Industry use**: Games, embedded systems, high-frequency trading
- **Modern features**: C++17/20 are quite pleasant to use!

## Troubleshooting

### "curl/curl.h: No such file or directory"
libcurl development headers not installed. Run:
```bash
sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
brew install curl                           # macOS
```

### "json.hpp: No such file or directory"
nlohmann/json header not found. Either:
1. Download it to lib/ folder (see above)
2. Or install via package manager

### "DEMETERICS_API_KEY not set"
Set your environment variable:
```bash
export DEMETERICS_API_KEY="your-key-here"
```

### Compiler version issues
Ensure you have C++17 support:
```bash
g++ --version  # Should be 7+ for C++17
```

## Resources

- [nlohmann/json Documentation](https://json.nlohmann.me/)
- [libcurl C++ Tutorial](https://curl.se/libcurl/c/libcurl-tutorial.html)
- [Learn C++](https://www.learncpp.com/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
