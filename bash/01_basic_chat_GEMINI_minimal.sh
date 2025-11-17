#!/bin/sh
# Basic chat with Google Gemini - ask a safe question and get a quick answer
# Usage: ./01_basic_chat_GEMINI_minimal.sh
#
# What this demonstrates:
#   - Calling the native Gemini generateContent endpoint with curl
#   - Sending prompts with the contents/parts structure Gemini expects
#
# What you'll learn:
#   - How to authenticate with the x-goog-api-key header
#   - How generationConfig and safetySettings influence the reply
#
# Prerequisites:
#   - export GEMINI_API_KEY="your_google_ai_studio_key"
#   - curl installed (pre-installed on macOS/Linux, in Git Bash on Windows)
#
# Expected output:
#   - JSON response that includes the model's answer plus token usage metadata

# Step 1: Send a POST request directly to Gemini 2.5 Flash
# This is a JSON-native API, so everything (question + settings) lives in the body
curl -s https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent \
  -H "Content-Type: application/json" \
  -H "x-goog-api-key: $GEMINI_API_KEY" \
  -d '{
    "contents": [
      {
        "parts": [
          {
            "text": "Explain how AI works in a few friendly sentences for an 8th grader."
          }
        ]
      }
    ],
    "generationConfig": {
      "temperature": 0.7,
      "maxOutputTokens": 150,
      "topK": 40,
      "topP": 0.95
    },
    "safetySettings": [
      {"category": "HARM_CATEGORY_HATE_SPEECH", "threshold": "BLOCK_MEDIUM_AND_ABOVE"},
      {"category": "HARM_CATEGORY_SEXUAL", "threshold": "BLOCK_MEDIUM_AND_ABOVE"},
      {"category": "HARM_CATEGORY_HARASSMENT", "threshold": "BLOCK_MEDIUM_AND_ABOVE"},
      {"category": "HARM_CATEGORY_DANGEROUS_CONTENT", "threshold": "BLOCK_MEDIUM_AND_ABOVE"}
    ]
  }'

# Request breakdown:
# -H "Content-Type: application/json" tells Gemini to expect JSON input
# -H "x-goog-api-key: $GEMINI_API_KEY" authenticates with your Google AI Studio key
# contents -> parts -> text is where your actual prompt lives
# generationConfig keeps the answer concise (maxOutputTokens) and balanced (temperature/topP)
# safetySettings keep classroom projects safe by blocking disallowed categories
