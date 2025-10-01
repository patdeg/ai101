#!/bin/bash

# Example 2: System + User Prompt
# System prompts let you control HOW the AI behaves.
# Think of it like giving the AI a job description before asking questions.

API_URL="https://api.groq.com/openai/v1/chat/completions"

curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "system",
        "content": "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
      },
      {
        "role": "user",
        "content": "Why did the Roman Empire fall?"
      }
    ],
    "temperature": 0.5,
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
