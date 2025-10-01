#!/usr/bin/env python3
"""
Example 1: Basic Chat with AI

WHAT THIS DEMONSTRATES:
    - How to send a simple question to an AI model
    - Using only Python's built-in libraries (no pip install required!)
    - Making HTTPS API requests with http.client
    - Handling JSON data encoding and decoding
    - Working with environment variables for API keys

WHAT YOU'LL LEARN:
    - Basic HTTP request/response cycle
    - API authentication with Bearer tokens
    - JSON payload structure for chat completions
    - Parsing AI model responses
    - Understanding token usage and costs

PREREQUISITES:
    - Python 3.6 or higher
    - GROQ_API_KEY environment variable set
    - Internet connection

EXPECTED OUTPUT:
    - Full JSON response from the API
    - AI's answer to your question
    - Token usage statistics (prompt, completion, total)

Run with: python3 01_basic_chat.py
"""

import http.client  # For making HTTP/HTTPS requests
import json         # For encoding/decoding JSON data
import os           # For accessing environment variables

# ==============================================================================
# Step 1: Load and validate API credentials
# ==============================================================================

# Get API key from environment variable (more secure than hardcoding)
# This keeps your API key out of your source code
api_key = os.environ.get('GROQ_API_KEY')

# Validate that the API key exists before proceeding
if not api_key:
    print("Error: GROQ_API_KEY environment variable not set")
    print("Run: export GROQ_API_KEY='your_key_here'")
    exit(1)

# ==============================================================================
# Step 2: Build the request payload
# ==============================================================================

# Before creating the payload, we need to understand its structure:
# - model: Which AI model to use
# - messages: Array of conversation messages (user, assistant, system)
# - temperature: Creativity level (0.0 = deterministic, 2.0 = very random)
# - max_tokens: Maximum length of response

# Create the request payload (the data we're sending to the API)
payload = {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
        {
            "role": "user",  # This message is from the user
            "content": "What is the capital of Switzerland?"
        }
    ],
    "temperature": 0.7,  # Balanced creativity (good for factual questions)
    "max_tokens": 100    # Limit response to ~100 tokens (~75 words)
}

# Before sending, we need to convert Python dictionary to JSON string
# APIs expect JSON format, not Python objects
payload_json = json.dumps(payload)

# ==============================================================================
# Step 3: Establish HTTPS connection to the API
# ==============================================================================

# Create HTTPS connection to Groq API server
# HTTPS ensures encrypted communication for security
conn = http.client.HTTPSConnection("api.groq.com")

# ==============================================================================
# Step 4: Prepare HTTP headers for authentication and content type
# ==============================================================================

# Set up headers that the API requires:
# - Content-Type tells the server we're sending JSON
# - Authorization provides our API key for authentication
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'  # Bearer token authentication
}

# ==============================================================================
# Step 5: Send the HTTP POST request to the API
# ==============================================================================

# Make the POST request to create a chat completion
# - Method: POST (we're creating something, not just reading)
# - Path: OpenAI-compatible endpoint
# - Body: Our JSON payload with the question
# - Headers: Authentication and content type
conn.request("POST", "/openai/v1/chat/completions", payload_json, headers)

# ==============================================================================
# Step 6: Receive and read the HTTP response
# ==============================================================================

# Get the response object from the server
response = conn.getresponse()

# Before parsing, we need to read the raw response data
# The response comes as bytes, so we decode it to a UTF-8 string
response_data = response.read().decode('utf-8')

# ==============================================================================
# Step 7: Parse the JSON response into a Python dictionary
# ==============================================================================

# Convert JSON string back to Python dictionary for easy access
response_json = json.loads(response_data)

# ==============================================================================
# Step 8: Clean up the connection
# ==============================================================================

# Close the connection to free up resources
# Good practice: always close connections when done
conn.close()

# ==============================================================================
# Step 9: Display the results to the user
# ==============================================================================

# Display the full response (useful for debugging and understanding the API)
print("Full Response:")
print(json.dumps(response_json, indent=2))

# Extract and display just the AI's answer
# The answer is nested in: choices[0] -> message -> content
print("\nAI Answer:")
print(response_json['choices'][0]['message']['content'])

# Display token usage information
# Tokens are how AI models measure text (1 token ≈ 0.75 words)
# Understanding token usage helps estimate costs
print("\nToken Usage:")
print(f"  Prompt: {response_json['usage']['prompt_tokens']}")
print(f"  Response: {response_json['usage']['completion_tokens']}")
print(f"  Total: {response_json['usage']['total_tokens']}")

# Python concepts explained:
#
# import http.client
#   Built-in module for HTTP/HTTPS requests (no pip install!)
#
# import json
#   Built-in module for JSON encoding/decoding
#
# import os
#   Built-in module for OS operations (like environment variables)
#
# os.environ.get('VAR_NAME')
#   Gets environment variable (like $GROQ_API_KEY in bash)
#   Returns None if not set
#
# json.dumps(dict)
#   Converts Python dictionary to JSON string
#   indent=2 means pretty-print with 2-space indentation
#
# json.loads(string)
#   Converts JSON string to Python dictionary
#
# f'Bearer {api_key}'
#   F-string (formatted string literal)
#   Variables in {curly braces} get replaced with their values
#
# http.client.HTTPSConnection()
#   Creates HTTPS connection (secure HTTP)
#   Use HTTPConnection() for plain HTTP (not recommended)
#
# conn.request(method, path, body, headers)
#   Sends HTTP request
#   method = "GET", "POST", "PUT", etc.
#   path = "/api/endpoint"
#   body = request data (JSON string)
#   headers = dict of HTTP headers
#
# response.read()
#   Returns bytes (binary data)
#   Need to .decode('utf-8') to get string
#
# Dictionary access:
#   response_json['key']           # Crashes if key missing
#   response_json.get('key')       # Returns None if key missing
#   response_json.get('key', 'default')  # Returns 'default' if missing
#
# String methods:
#   .upper()    # "hello" → "HELLO"
#   .lower()    # "HELLO" → "hello"
#   .strip()    # "  hi  " → "hi"
#   .split()    # "a b c" → ['a', 'b', 'c']
