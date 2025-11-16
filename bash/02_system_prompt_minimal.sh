#!/bin/sh
# Use system prompt to control AI behavior
# Usage: ./02_system_prompt_minimal.sh

curl -s https://api.demeterics.com/groq/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $DEMETERICS_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {"role": "system", "content": "You are a helpful pirate. Always speak like a pirate!"},
      {"role": "user", "content": "What is the capital of France?"}
    ],
    "temperature": 1.0,
    "max_tokens": 150
  }'

# Parameters explained:
# system role - Instructions that control how the AI behaves
# user role - The actual question or request
# temperature - Higher = more creative/varied responses
