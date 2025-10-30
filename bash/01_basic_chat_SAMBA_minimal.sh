#!/bin/sh
# Basic chat with SambaNova (OpenAI-compatible) - Ask a question and get an answer
# Usage: ./01_basic_chat_SAMBA_minimal.sh

curl -s https://api.sambanova.ai/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $SAMBANOVA_API_KEY" \
  -d '{
    "model": "Meta-Llama-3.1-8B-Instruct",
    "messages": [{"role": "user", "content": "What is the capital of Switzerland?"}],
    "temperature": 0.7,
    "max_tokens": 100
  }'

# Parameters explained:
# model - Which AI model to use (SambaNova OpenAI-compatible)
# messages - Array of conversation messages (role: user/assistant/system)
# temperature - Randomness (0.0 = focused, 2.0 = creative)
# max_tokens - Maximum response length

