#!/bin/bash

# Example 1: Basic Chat
# This sends a simple question to the AI model and gets a response.

# The API endpoint - this is where we send our request
API_URL="https://api.groq.com/openai/v1/chat/completions"

# Make the API call using curl
curl "$API_URL" \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $GROQ_API_KEY" \
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "user",
        "content": "What is the capital of Switzerland?"
      }
    ],
    "temperature": 0.7,
    "max_tokens": 100
  }'

# What each part means:
#
# -H "Content-Type: application/json"
#   Tells the server we're sending JSON data
#
# -H "Authorization: Bearer $GROQ_API_KEY"
#   Proves we have permission to use the API
#   $GROQ_API_KEY is read from your environment variable
#
# -d '{...}'
#   The data we're sending (the -d flag means "data")
#
# "model": "meta-llama/llama-4-scout-17b-16e-instruct"
#   Which AI model to use (this one is good for chat + vision)
#
# "messages": [...]
#   The conversation history (just one message here)
#
# "role": "user"
#   Who sent this message (you, the user)
#
# "content": "What is..."
#   The actual question/message
#
# "temperature": 0.7
#   Controls randomness (0 = very focused, 2 = very creative)
#   0.7 is a good middle ground
#
# "max_tokens": 100
#   Maximum length of the response (one token ≈ 3/4 of a word)
