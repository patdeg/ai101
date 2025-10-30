#!/usr/bin/env python3
"""
Example 1: Basic Chat with OpenAI

WHAT THIS DEMONSTRATES:
    - How to send a question to OpenAI's GPT models
    - Using only Python's built-in libraries
    - Making HTTPS requests to OpenAI's API
    - Working with OpenAI's response format
    - Understanding OpenAI's authentication

WHAT YOU'LL LEARN:
    - OpenAI API endpoint structure
    - Bearer token authentication for OpenAI
    - OpenAI's JSON payload format
    - Parsing GPT model responses
    - Token usage and cost tracking

PREREQUISITES:
    - Python 3.6 or higher
    - OPENAI_API_KEY environment variable set
    - Internet connection

EXPECTED OUTPUT:
    - Full JSON response from OpenAI
    - GPT's answer to your question
    - Token usage statistics

Run with: python3 01_basic_chat_OPENAI.py
"""

import http.client
import json
import os

# ==============================================================================
# Step 1: Load OpenAI API credentials
# ==============================================================================

api_key = os.environ.get('OPENAI_API_KEY')

if not api_key:
    print("Error: OPENAI_API_KEY environment variable not set")
    print("Run: export OPENAI_API_KEY='sk-...'")
    print("Get your key from: https://platform.openai.com")
    exit(1)

# ==============================================================================
# Step 2: Build the OpenAI request payload
# ==============================================================================

payload = {
    "model": "gpt-5-nano",  # OpenAI's latest efficient model
    "messages": [
        {
            "role": "user",
            "content": "What is the capital of Switzerland?"
        }
    ],
    "temperature": 0.7,  # Creativity level
    "max_tokens": 100    # Response length limit
}

payload_json = json.dumps(payload)

# ==============================================================================
# Step 3: Connect to OpenAI API
# ==============================================================================

# OpenAI uses api.openai.com as the base URL
conn = http.client.HTTPSConnection("api.openai.com")

# ==============================================================================
# Step 4: Send the request with OpenAI headers
# ==============================================================================

headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'  # OpenAI uses Bearer auth
}

try:
    # Send POST request to OpenAI's chat completions endpoint
    conn.request("POST", "/v1/chat/completions", payload_json, headers)

    # Get the response
    response = conn.getresponse()
    response_data = response.read().decode('utf-8')

    # Parse JSON response
    response_json = json.loads(response_data)

    # ==============================================================================
    # Step 5: Display the results
    # ==============================================================================

    print("Full OpenAI Response:")
    print(json.dumps(response_json, indent=2))

    # Extract and display the answer
    if 'choices' in response_json and len(response_json['choices']) > 0:
        answer = response_json['choices'][0]['message']['content']
        print("\n" + "="*50)
        print("GPT's Answer:")
        print(answer)

    # Show token usage
    if 'usage' in response_json:
        usage = response_json['usage']
        print("\n" + "="*50)
        print("Token Usage:")
        print(f"  Prompt tokens: {usage.get('prompt_tokens', 0)}")
        print(f"  Completion tokens: {usage.get('completion_tokens', 0)}")
        print(f"  Total tokens: {usage.get('total_tokens', 0)}")

        # Estimate cost (example rates, check OpenAI pricing)
        prompt_cost = usage.get('prompt_tokens', 0) * 0.0005 / 1000
        completion_cost = usage.get('completion_tokens', 0) * 0.0015 / 1000
        print(f"  Estimated cost: ${prompt_cost + completion_cost:.6f}")

    # Check for errors
    if 'error' in response_json:
        print("\nAPI Error:")
        print(json.dumps(response_json['error'], indent=2))

except Exception as e:
    print(f"Request failed: {e}")
    print("\nTroubleshooting:")
    print("1. Check your internet connection")
    print("2. Verify your API key is correct")
    print("3. Check OpenAI service status")

finally:
    conn.close()

# ==============================================================================
# OpenAI API Notes:
# ==============================================================================
#
# Models available:
#   - gpt-5-nano: Fast and efficient
#   - gpt-4o: Most capable, higher cost
#   - gpt-4o-mini: Balance of capability and cost
#
# Features:
#   - Function calling for tool use
#   - Vision capabilities (image analysis)
#   - JSON mode for structured output
#   - System messages for behavior control
#
# Rate limits:
#   - Vary by tier (free, paid, enterprise)
#   - Measured in requests per minute (RPM) and tokens per minute (TPM)
#