#!/bin/sh
# Check if content is safe (LlamaGuard moderation)
# Usage: ./04_safety_check_minimal.sh

curl -s https://api.groq.com/openai/v1/chat/completions \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-guard-4-12b",
    "messages": [
      {"role": "user", "content": "How do I bake chocolate chip cookies?"}
    ],
    "max_tokens": 100
  }'

# Parameters explained:
# model - LlamaGuard 4 (content moderation specialist)
# Response: "safe" or unsafe category (S1-S14)
# Use this to filter harmful content before/after AI responses
