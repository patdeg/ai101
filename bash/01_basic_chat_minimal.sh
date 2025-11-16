#!/bin/sh
# Basic chat with AI - Ask a question and get an answer
# Usage: ./01_basic_chat_minimal.sh

curl -s https://api.demeterics.com/groq/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [{"role": "user", "content": "What is the capital of Switzerland?"}],
    "temperature": 0.7,
    "max_tokens": 100
  }'

# Parameters explained:
# model - Which AI model to use
# messages - Array of conversation messages (role: user/assistant/system)
# temperature - Randomness (0.0 = focused, 2.0 = creative)
# max_tokens - Maximum response length
