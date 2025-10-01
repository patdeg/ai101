#!/usr/bin/env python3
"""
Example 1: Basic Chat
This sends a simple question to the AI model using Python's built-in libraries.
No external dependencies needed (no pip install)!
"""

import http.client
import json
import os

# Get API key from environment variable
api_key = os.environ.get('GROQ_API_KEY')

if not api_key:
    print("Error: GROQ_API_KEY environment variable not set")
    print("Run: export GROQ_API_KEY='your_key_here'")
    exit(1)

# Create the request payload (the data we're sending)
payload = {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
        {
            "role": "user",
            "content": "What is the capital of Switzerland?"
        }
    ],
    "temperature": 0.7,
    "max_tokens": 100
}

# Convert Python dict to JSON string
payload_json = json.dumps(payload)

# Create HTTPS connection
conn = http.client.HTTPSConnection("api.groq.com")

# Set up headers
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

# Make the request
conn.request("POST", "/openai/v1/chat/completions", payload_json, headers)

# Get the response
response = conn.getresponse()

# Read and parse the response
response_data = response.read().decode('utf-8')
response_json = json.loads(response_data)

# Close the connection
conn.close()

# Display the results
print("Full Response:")
print(json.dumps(response_json, indent=2))

print("\nAI Answer:")
print(response_json['choices'][0]['message']['content'])

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
