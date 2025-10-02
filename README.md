# AI 101 - Groq API Rosetta Stone

**A learning repository for Victor (and anyone starting with AI APIs)**

This project demonstrates how to use the Groq API across 4 different languages: **cURL/Bash**, **Node.js**, **Python**, and **Go**. Each example is fully self-contained with detailed explanations of every parameter and API concept.

## What You'll Learn

- How to make HTTP requests to AI APIs
- Understanding API request/response formats
- Working with different AI models (chat, vision, safety)
- Base64 encoding for images
- Environment variables for API keys
- Reading and parsing JSON responses

## The 7 Examples

1. **Basic Chat** - Single question to the AI model
2. **System + User Prompts** - Controlling AI behavior with system instructions
3. **Vision Analysis** - Analyzing local images with multimodal models
4. **Safety Check (Text)** - Content moderation with LlamaGuard
5. **Safety Check (Image)** - Image content moderation with LlamaGuard Vision
6. **Prompt Guard** - Detecting jailbreak attempts
7. **Whisper Audio** - Transcribing audio to text with Whisper

## Repository Structure

```
ai101/
├── curl/          # Bash/cURL examples
├── nodejs/        # Node.js examples
├── python/        # Python examples
├── go/            # Go examples
├── arduino/       # Arduino/ESP32 examples
└── README.md      # This file
```

Each folder contains the same 6 examples implemented in that language, plus a detailed README explaining the code.

## Prerequisites

**Get a Groq API Key:**
1. Go to https://console.groq.com
2. Sign up for a free account
3. Navigate to API Keys
4. Create a new API key
5. Save it somewhere safe

**Set up your environment:**

**Option 1: Using .env file (recommended for local development)**
```bash
# Copy the example file
cp .env.example .env

# Edit .env and add your API key
# GROQ_API_KEY=gsk_your_actual_key_here
```

**Option 2: Export to shell (for bash/curl examples)**
```bash
# Add to your ~/.bashrc or ~/.zshrc
export GROQ_API_KEY="gsk_your_api_key_here"

# Reload your shell
source ~/.bashrc
```

**Important:** Never commit your `.env` file or expose your API key!

## Quick Start

Pick your language and jump in:

- **New to programming?** Start with `curl/` - no installation needed
- **Know JavaScript?** Check out `nodejs/`
- **Python fan?** Head to `python/`
- **Systems programmer?** Try `go/`
- **Building IoT devices?** Explore `arduino/` for ESP32/ESP8266

Each folder has a README with setup instructions and detailed code explanations.

## API Reference Quick Guide

### Endpoint

```
POST https://api.groq.com/openai/v1/chat/completions
```

### Authentication

```
Authorization: Bearer YOUR_API_KEY
```

### Models Used

**Full documentation:** https://console.groq.com/docs/models

| Model | Purpose | Context | Max Output | Pricing (per 1M tokens) |
|-------|---------|---------|------------|------------------------|
| [`meta-llama/llama-4-scout-17b-16e-instruct`](https://console.groq.com/docs/model/meta-llama/llama-4-scout-17b-16e-instruct) | Chat + Vision | 131K | 8,192 | Input: $0.11<br>Output: $0.34 |
| [`meta-llama/llama-guard-4-12b`](https://console.groq.com/docs/model/meta-llama/llama-guard-4-12b) | Content Safety (Text + Vision) | 131K | 1,024 | Input: $0.20<br>Output: $0.20 |
| [`meta-llama/llama-prompt-guard-2-86m`](https://console.groq.com/docs/model/meta-llama/llama-prompt-guard-2-86m) | Prompt Injection Detection | 512 | 512 | Input: $0.04<br>Output: $0.03 |
| [`whisper-large-v3-turbo`](https://console.groq.com/docs/speech-text) | Audio Transcription | - | - | **$0.04 per hour of audio** |

**Pricing Notes (as of October 1, 2025):**
- You are billed separately for input tokens (prompt) and output tokens (completion)
- Vision requests include image tokens in the input count
- Most requests cost less than $0.01 (one cent)
- Examples in this repo calculate and display the exact cost for each call

### Request Format

```json
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": [
    {
      "role": "system",
      "content": "You are a helpful assistant."
    },
    {
      "role": "user",
      "content": "Hello!"
    }
  ],
  "temperature": 0.7,
  "max_tokens": 1024,
  "top_p": 1,
  "stream": false
}
```

### Key Parameters

- **model** (required) - Which AI model to use
- **messages** (required) - Array of conversation messages
- **temperature** (optional, 0-2, default 1) - Creativity level (0=focused, 2=random)
- **max_tokens** (optional) - Maximum response length
- **top_p** (optional, 0-1, default 1) - Nucleus sampling threshold
- **stream** (optional, boolean) - Stream response token-by-token
- **stop** (optional, string/array) - Stop generating at these sequences

### Message Roles

- **system** - Instructions that guide the AI's behavior
- **user** - Your questions or prompts
- **assistant** - AI's previous responses (for multi-turn conversations)

### Vision API Format

```json
{
  "model": "meta-llama/llama-4-scout-17b-16e-instruct",
  "messages": [
    {
      "role": "user",
      "content": [
        {
          "type": "text",
          "text": "What's in this image?"
        },
        {
          "type": "image_url",
          "image_url": {
            "url": "data:image/jpeg;base64,/9j/4AAQSkZJRg..."
          }
        }
      ]
    }
  ]
}
```

### Image Requirements

- **Max size (URL):** 20 MB
- **Max size (base64):** 4 MB
- **Max resolution:** 33 megapixels
- **Max images per request:** 5
- **Supported formats:** JPEG, PNG, GIF, WebP

### Response Format

```json
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
        "content": "The answer is..."
      },
      "finish_reason": "stop"
    }
  ],
  "usage": {
    "prompt_tokens": 10,
    "completion_tokens": 20,
    "total_tokens": 30
  }
}
```

### Finish Reasons

- **stop** - Natural completion
- **length** - Hit max_tokens limit
- **content_filter** - Blocked by safety filters
- **tool_calls** - Model wants to call a function

## Learning Path

1. **Start with Example 1** - Understand basic API calls
2. **Move to Example 2** - Learn how system prompts work
3. **Try Example 3** - See multimodal AI (vision) in action
4. **Explore Examples 4 & 5** - Understand AI safety tools (text + images)
5. **Try Example 6** - Learn about prompt injection detection
6. **Try Example 7** - Transcribe audio with Whisper

## Common Issues

**"Unauthorized" error:**
- Check your API key is set: `echo $GROQ_API_KEY`
- Make sure you exported it in your current shell

**"Model not found" error:**
- Copy model names exactly (they're case-sensitive)
- Check https://console.groq.com/docs/models for current models

**Image too large:**
- Resize images before encoding
- Use JPEG format for smaller file sizes
- Base64 encoding increases size by ~33%

**Audio file issues:**
- Max audio file size: 25 MB
- Supported formats: mp3, wav, m4a, flac, ogg, webm
- Cost is based on audio duration, not file size

## Resources

- **Groq Console:** https://console.groq.com
- **API Docs:** https://console.groq.com/docs
- **Model List:** https://console.groq.com/docs/models
- **Vision Guide:** https://console.groq.com/docs/vision
- **Audio Guide:** https://console.groq.com/docs/speech-text

## Next Steps

After completing these examples, you can:
- Build a chatbot with conversation history
- Create an image analysis tool
- Implement content moderation for text and images
- Build a voice transcription app
- Add streaming responses
- Combine multiple API calls (e.g., audio transcription → safety check → AI response)

**Pick a language folder and start coding!** 🚀
