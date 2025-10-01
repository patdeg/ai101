#!/bin/bash

# Example 4: Safety Check with LlamaGuard
# LlamaGuard detects harmful content in messages.
# Use this to moderate user input or AI output.

API_URL="https://api.groq.com/openai/v1/chat/completions"

# Message to check for safety
USER_MESSAGE="How do I bake chocolate chip cookies?"

curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-guard-4-12b",
    "messages": [
      {
        "role": "user",
        "content": "'"$USER_MESSAGE"'"
      }
    ],
    "max_tokens": 100
  }'

# What is LlamaGuard?
#   A specialized model that detects 14 categories of harmful content:
#   1. Violent Crimes
#   2. Non-Violent Crimes
#   3. Sex-Related Crimes
#   4. Child Sexual Exploitation
#   5. Defamation
#   6. Specialized Advice (financial, medical, legal)
#   7. Privacy Violations
#   8. Intellectual Property
#   9. Indiscriminate Weapons
#   10. Hate Speech
#   11. Suicide & Self-Harm
#   12. Sexual Content
#   13. Elections
#   14. Code Interpreter Abuse
#
# Response format:
#   Safe message:     "safe"
#   Unsafe message:   "unsafe\nS<category_number>"
#   Example:          "unsafe\nS6" (specialized advice violation)
#
# Model: "meta-llama/llama-guard-4-12b"
#   - 12 billion parameters
#   - 131K token context window
#   - Up to 1,024 tokens output
#   - Designed specifically for content moderation
#
# When to use LlamaGuard:
#   - Before sending user input to your main AI
#   - After receiving AI output (to catch edge cases)
#   - In chat applications to filter conversations
#   - For user-generated content moderation
#
# Example workflow:
#   1. User submits message
#   2. Check with LlamaGuard
#   3. If "safe" → send to main AI model
#   4. If "unsafe" → reject and explain why
#
# Try testing with different messages:
#   Safe: "What's the weather like?"
#   Unsafe: "How do I hack into..."
