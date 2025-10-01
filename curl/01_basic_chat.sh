#!/bin/bash

################################################################################
# EXAMPLE 1: BASIC CHAT
################################################################################
#
# What this demonstrates:
#   - How to make your first API call to an AI model
#   - Sending a simple question and receiving a response
#   - Basic structure of the chat completions API
#
# What you'll learn:
#   - How to structure a chat API request
#   - What headers are required for authentication
#   - How to control response behavior with parameters
#
# Prerequisites:
#   - GROQ_API_KEY environment variable must be set
#   - curl command installed (usually pre-installed on Linux/Mac)
#
# Expected output:
#   - JSON response containing the AI's answer
#   - The answer will be in response.choices[0].message.content
#
################################################################################

# Store the API endpoint URL in a variable for easy reference
# This is Groq's chat completions endpoint (OpenAI-compatible)
API_URL="https://api.groq.com/openai/v1/chat/completions"

# Now we'll make the API call using curl
# curl is a command-line tool for sending HTTP requests
# We're using POST method (default when using -d) to send our question
curl "$API_URL" \
  -H "Content-Type: application/json" \           # Tell the server we're sending JSON data
  -H "Authorization: Bearer $GROQ_API_KEY" \      # Authenticate with your API key
  -d '{
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
      {
        "role": "user",                           # We are the user asking a question
        "content": "What is the capital of Switzerland?"
      }
    ],
    "temperature": 0.7,                           # Creativity level (0=focused, 2=creative)
    "max_tokens": 100                             # Limit response length to about 75 words
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
