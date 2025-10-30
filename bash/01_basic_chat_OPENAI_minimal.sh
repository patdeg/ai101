#!/bin/sh
# Basic chat with OpenAI - Ask a question and get an answer
# Usage: ./01_basic_chat_OPENAI_minimal.sh

curl -s https://api.openai.com/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $OPENAI_API_KEY" \
  -d '{
    "model": "gpt-5-nano",
    "messages": [{"role": "user", "content": "What is the capital of Switzerland?"}]
  }'

# Parameters explained:
# model - Which AI model to use
# messages - Array of conversation messages (role: user/assistant/system)
# temperature - Randomness (0.0 = focused, 2.0 = creative)
# max_tokens - Maximum response length

