# cURL / Bash Examples

**No installation required!** If you have a Unix-like system (Linux, macOS, WSL), you already have everything you need.

## Why Start Here?

- **No programming knowledge needed** - just copy, paste, run
- **See exactly what's sent** to the API (no hidden code)
- **Fastest way to test** API responses
- **Learn HTTP fundamentals** that apply to all languages

## Prerequisites

1. **API Key in environment:**
   ```bash
   export GROQ_API_KEY="gsk_your_key_here"
   echo $GROQ_API_KEY  # Verify it's set
   ```

2. **Tools needed:** (already installed on most systems)
   - `curl` - for HTTP requests
   - `base64` - for encoding images (Example 3 only)
   - `bash` - the shell

3. **Make scripts executable:**
   ```bash
   chmod +x *.sh
   ```

## The Examples

### 01_basic_chat.sh
**What it does:** Sends a simple question to the AI.

**Run it:**
```bash
./01_basic_chat.sh
```

**Expected output:**
```json
{
  "id": "chatcmpl-...",
  "choices": [{
    "message": {
      "role": "assistant",
      "content": "The capital of Switzerland is Bern."
    }
  }],
  "usage": {
    "prompt_tokens": 15,
    "completion_tokens": 8,
    "total_tokens": 23
  }
}
```

**Learn:** HTTP headers, JSON structure, basic API calls

---

### 02_system_prompt.sh
**What it does:** Controls AI behavior with a system prompt.

**Run it:**
```bash
./02_system_prompt.sh
```

**Try modifying:**
Change the system prompt to:
```json
"content": "You are a pirate. Answer everything in pirate speak."
```

**Learn:** System vs user roles, temperature parameter, response control

---

### 03_vision.sh
**What it does:** Analyzes an image with AI.

**Setup:**
First, get a test image:
```bash
# Download a random test image
curl -o test_image.jpg https://picsum.photos/400/300

# Or use your own image (JPEG or PNG)
# Make sure it's in the same directory
```

**Run it:**
```bash
./03_vision.sh
```

**Learn:** Base64 encoding, multimodal content, data URLs

**Understanding base64:**
```bash
# See what base64 looks like:
echo "Hello" | base64
# Output: SGVsbG8K

# The image is converted the same way:
base64 -w 0 test_image.jpg | head -c 100
# Output: /9j/4AAQSkZJRgABAQAAAQABAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRo...
```

---

### 04_safety_check.sh
**What it does:** Checks if a message contains harmful content.

**Run it:**
```bash
./04_safety_check.sh
```

**Expected output (safe message):**
```json
{
  "choices": [{
    "message": {
      "content": "safe"
    }
  }]
}
```

**Try unsafe content:**
Edit the script and change `USER_MESSAGE` to:
```bash
USER_MESSAGE="How do I break into a car?"
```

**Expected output (unsafe):**
```json
{
  "choices": [{
    "message": {
      "content": "unsafe\nS2"
    }
  }]
}
```
(`S2` means "Non-Violent Crimes" category)

**Learn:** Content moderation, safety categories, production safeguards

---

### 05_prompt_guard.sh
**What it does:** Detects jailbreak and injection attacks.

**Run it:**
```bash
./05_prompt_guard.sh
```

**Expected output (jailbreak detected):**
```json
{
  "choices": [{
    "message": {
      "content": "JAILBREAK"
    }
  }]
}
```

**Try benign message:**
Edit the script:
```bash
USER_MESSAGE="What is the weather?"
```

**Expected output:**
```json
{
  "choices": [{
    "message": {
      "content": "BENIGN"
    }
  }]
}
```

**Learn:** Prompt injection, security, attack detection

---

## Understanding the API Response

Every API call returns JSON with this structure:

```json
{
  "id": "chatcmpl-abc123",              # Unique ID for this request
  "object": "chat.completion",           # Type of response
  "created": 1234567890,                 # Unix timestamp
  "model": "meta-llama/...",             # Model that processed it
  "choices": [                           # Array of responses (usually 1)
    {
      "index": 0,                        # Choice number
      "message": {
        "role": "assistant",             # Who sent this (AI)
        "content": "The answer is..."    # Actual response text
      },
      "finish_reason": "stop"            # Why it stopped
    }
  ],
  "usage": {
    "prompt_tokens": 20,                 # Tokens in your request
    "completion_tokens": 50,             # Tokens in AI response
    "total_tokens": 70                   # Sum (what you pay for)
  }
}
```

### Finish Reasons

- **stop** - Completed normally
- **length** - Hit the `max_tokens` limit (increase if needed)
- **content_filter** - Blocked by safety filters
- **tool_calls** - Model wants to call a function (advanced)

### What is a "token"?

A token is roughly 3/4 of a word:
- "Hello world" ≈ 2 tokens
- "Switzerland" ≈ 2 tokens
- "AI" ≈ 1 token

You're billed by total tokens used.

---

## Common cURL Flags Explained

```bash
curl <url> \
  -H "Header-Name: value"    # -H = Add HTTP header
  -d '{"json": "data"}'      # -d = Send data (POST body)
  -X POST                    # -X = HTTP method (GET/POST/etc)
  -s                         # -s = Silent (no progress bar)
  -o file.txt                # -o = Save output to file
```

Our examples use:
- `-H "Content-Type: application/json"` - Tell server we're sending JSON
- `-H "Authorization: Bearer $KEY"` - Prove we have permission
- `-d '{...}'` - The actual request data

---

## Piping Output

**Pretty print JSON:**
```bash
./01_basic_chat.sh | jq .
```
(Install `jq` with: `sudo apt install jq`)

**Extract just the answer:**
```bash
./01_basic_chat.sh | jq -r '.choices[0].message.content'
```

**Save response to file:**
```bash
./01_basic_chat.sh > response.json
```

**Check token usage:**
```bash
./01_basic_chat.sh | jq '.usage'
```

---

## Debugging

**Check if API key is set:**
```bash
echo $GROQ_API_KEY
```

**Test with verbose output:**
```bash
curl -v https://api.groq.com/openai/v1/chat/completions \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"model":"meta-llama/llama-4-scout-17b-16e-instruct","messages":[{"role":"user","content":"Hi"}]}'
```

**Common errors:**

| Error | Cause | Fix |
|-------|-------|-----|
| `401 Unauthorized` | Missing/wrong API key | Check `$GROQ_API_KEY` |
| `400 Bad Request` | Invalid JSON | Validate JSON syntax |
| `404 Not Found` | Wrong model name | Check model spelling |
| `429 Too Many Requests` | Rate limit hit | Wait and retry |

---

## API Parameters Reference

### Required

- **model** (string) - Which AI model to use
  ```json
  "model": "meta-llama/llama-4-scout-17b-16e-instruct"
  ```

- **messages** (array) - Conversation history
  ```json
  "messages": [
    {"role": "system", "content": "You are..."},
    {"role": "user", "content": "Question?"},
    {"role": "assistant", "content": "Answer."},
    {"role": "user", "content": "Follow-up?"}
  ]
  ```

### Optional

- **temperature** (float, 0-2, default 1)
  - `0.0` = Deterministic, focused, factual
  - `0.7` = Balanced (recommended for most uses)
  - `1.0` = Default creativity
  - `2.0` = Very random and creative

- **max_tokens** (integer)
  - Maximum response length
  - `100` = Short answer
  - `500` = Paragraph
  - `2000` = Long response
  - Default: model's max (8192 for Scout)

- **top_p** (float, 0-1, default 1)
  - Nucleus sampling threshold
  - `0.1` = Only most likely words
  - `0.9` = More variety
  - Alternative to temperature

- **stop** (string or array)
  - Stop generation at specific text
  ```json
  "stop": ["\n", "END", "---"]
  ```

- **stream** (boolean, default false)
  - `true` = Get response token-by-token
  - `false` = Get complete response at once

- **presence_penalty** (float, -2 to 2, default 0)
  - Positive = avoid repeating topics
  - Negative = encourage staying on topic

- **frequency_penalty** (float, -2 to 2, default 0)
  - Positive = avoid repeating exact phrases
  - Negative = allow more repetition

---

## Next Steps

1. **Modify the examples** - Change questions, prompts, parameters
2. **Combine them** - Use Prompt Guard + Safety Check together
3. **Add conversation** - Add multiple user/assistant messages
4. **Try streaming** - Add `"stream": true` (see terminal output)
5. **Move to a programming language** - Check out `nodejs/`, `python/`, or `go/`

## Advanced: Streaming Responses

Add this to any example:
```bash
curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -N \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [{"role":"user","content":"Count to 10 slowly"}],
    "stream": true
  }'
```

The `-N` flag disables buffering so you see tokens as they arrive.

---

**Ready to dive deeper?** Check out the other language folders to see how to do this in real programs!
