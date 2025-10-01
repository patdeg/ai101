#!/bin/bash

################################################################################
# EXAMPLE 2: SYSTEM PROMPT + USER PROMPT
################################################################################
#
# What this demonstrates:
#   - How to use system prompts to control AI behavior
#   - The difference between system and user messages
#   - How to shape AI responses with instructions
#
# What you'll learn:
#   - System prompts act like "job descriptions" for the AI
#   - Message order matters (system first, then user)
#   - How to control response style, format, and personality
#
# Prerequisites:
#   - GROQ_API_KEY environment variable must be set
#   - Understanding of Example 1 (basic chat)
#
# Expected output:
#   - A response that follows the system prompt rules
#   - In this case: exactly 2 sentences about Roman Empire
#   - The AI will be more focused/factual (temperature 0.5)
#
################################################################################

# Store the API endpoint URL
API_URL="https://api.groq.com/openai/v1/chat/completions"

# Make the API call with BOTH system and user messages
# The system message comes first to set the AI's behavior
curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "system",                         # System message defines HOW to respond
        "content": "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
      },
      {
        "role": "user",                           # User message asks WHAT to respond to
        "content": "Why did the Roman Empire fall?"
      }
    ],
    "temperature": 0.5,                           # Lower = more focused and consistent
    "max_tokens": 150
  }'

# Key differences from Example 1:
#
# "role": "system"
#   This message sets the AI's personality/instructions
#   The AI will follow these rules for all responses
#
# Message order matters:
#   1. System message comes FIRST
#   2. User message comes SECOND
#   3. You can add more user/assistant messages for conversation
#
# "temperature": 0.5
#   Lower than before (0.5 vs 0.7) = more focused, less creative
#   Good for factual questions where you want consistency
#
# Why use system prompts?
#   - Control response length ("be concise" or "explain in detail")
#   - Set expertise level ("explain like I'm 5" or "use technical terms")
#   - Define behavior ("be formal" or "be casual and friendly")
#   - Set output format ("answer in JSON" or "use bullet points")
