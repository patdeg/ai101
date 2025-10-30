#!/bin/sh
# Basic chat with Anthropic - Ask a question and get an answer
# Usage: ./01_basic_chat_ANTHROPIC_minimal.sh

curl -s https://api.anthropic.com/v1/messages \
  -H "Content-Type: application/json" \
  -H "x-api-key: $ANTHROPIC_API_KEY" \
  -H "anthropic-version: 2023-06-01" \
  -d '{
    "model": "claude-haiku-4-5",
    "max_tokens": 100,
    "temperature": 0.7,
    "messages": [
      {"role": "user", "content": "What is the capital of Switzerland?"}
    ]
  }'

# Parameters explained:
# model - Which Anthropic model to use
# messages - Array of conversation messages (role: user/assistant/system)
# temperature - Randomness (0.0 = focused, 2.0 = creative)
# max_tokens - Maximum response length

