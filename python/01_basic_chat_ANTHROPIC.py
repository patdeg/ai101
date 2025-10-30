#!/usr/bin/env python3
"""
Example 1: Basic Chat with Anthropic (Claude)

WHAT THIS DEMONSTRATES:
    - How to send a question to Anthropic's Claude models
    - Using only Python's built-in libraries
    - Understanding Anthropic's unique API format
    - Working with Claude's response structure
    - Anthropic-specific headers and authentication

WHAT YOU'LL LEARN:
    - Anthropic API endpoint structure
    - x-api-key authentication header
    - Required anthropic-version header
    - Claude's message format differences
    - Token usage tracking for Claude

PREREQUISITES:
    - Python 3.6 or higher
    - ANTHROPIC_API_KEY environment variable set
    - Internet connection

EXPECTED OUTPUT:
    - Full JSON response from Anthropic
    - Claude's answer to your question
    - Input/output token statistics

Run with: python3 01_basic_chat_ANTHROPIC.py
"""

import http.client
import json
import os

# ==============================================================================
# Step 1: Load Anthropic API credentials
# ==============================================================================

api_key = os.environ.get('ANTHROPIC_API_KEY')

if not api_key:
    print("Error: ANTHROPIC_API_KEY environment variable not set")
    print("Run: export ANTHROPIC_API_KEY='sk-ant-...'")
    print("Get your key from: https://console.anthropic.com")
    exit(1)

# ==============================================================================
# Step 2: Build the Anthropic request payload
# ==============================================================================

# Note: Anthropic REQUIRES max_tokens (unlike OpenAI where it's optional)
payload = {
    "model": "claude-haiku-4-5",  # Claude's fast, efficient model
    "max_tokens": 100,             # REQUIRED for Anthropic
    "temperature": 0.7,            # Creativity level
    "messages": [
        {
            "role": "user",
            "content": "What is the capital of Switzerland?"
        }
    ]
}

payload_json = json.dumps(payload)

# ==============================================================================
# Step 3: Connect to Anthropic API
# ==============================================================================

# Anthropic uses api.anthropic.com
conn = http.client.HTTPSConnection("api.anthropic.com")

# ==============================================================================
# Step 4: Send request with Anthropic-specific headers
# ==============================================================================

# Anthropic uses different headers than OpenAI
headers = {
    'Content-Type': 'application/json',
    'x-api-key': api_key,              # Note: x-api-key, not Authorization Bearer
    'anthropic-version': '2023-06-01'  # Required version header
}

try:
    # Send to Anthropic's messages endpoint (different from OpenAI)
    conn.request("POST", "/v1/messages", payload_json, headers)

    # Get the response
    response = conn.getresponse()
    response_data = response.read().decode('utf-8')

    # Parse JSON response
    response_json = json.loads(response_data)

    # ==============================================================================
    # Step 5: Display the results
    # ==============================================================================

    print("Full Anthropic Response:")
    print(json.dumps(response_json, indent=2))

    # Extract and display the answer
    # Note: Anthropic's response structure is different from OpenAI
    if 'content' in response_json and len(response_json['content']) > 0:
        # Claude returns content as an array of text blocks
        answer = response_json['content'][0]['text']
        print("\n" + "="*50)
        print("Claude's Answer:")
        print(answer)

    # Show token usage (different field names)
    if 'usage' in response_json:
        usage = response_json['usage']
        print("\n" + "="*50)
        print("Token Usage:")
        print(f"  Input tokens: {usage.get('input_tokens', 0)}")
        print(f"  Output tokens: {usage.get('output_tokens', 0)}")

        # Estimate cost (Claude Haiku pricing as example)
        input_cost = usage.get('input_tokens', 0) * 0.00025 / 1000
        output_cost = usage.get('output_tokens', 0) * 0.00125 / 1000
        print(f"  Estimated cost: ${input_cost + output_cost:.6f}")

    # Display model info
    if 'model' in response_json:
        print(f"\nModel used: {response_json['model']}")

    # Check for errors
    if 'error' in response_json:
        print("\nAPI Error:")
        print(f"  Type: {response_json['error'].get('type')}")
        print(f"  Message: {response_json['error'].get('message')}")

except Exception as e:
    print(f"Request failed: {e}")
    print("\nTroubleshooting:")
    print("1. Check your internet connection")
    print("2. Verify your API key starts with 'sk-ant-'")
    print("3. Ensure max_tokens is set (required for Anthropic)")
    print("4. Check Anthropic service status")

finally:
    conn.close()

# ==============================================================================
# Anthropic API Notes:
# ==============================================================================
#
# Key differences from OpenAI:
#   - max_tokens is REQUIRED (not optional)
#   - Uses x-api-key header instead of Authorization Bearer
#   - Requires anthropic-version header
#   - Response has 'content' array instead of 'choices'
#   - Token usage fields: input_tokens/output_tokens instead of prompt/completion
#
# Models available:
#   - claude-haiku-4-5: Fast and efficient
#   - claude-sonnet: Balanced performance
#   - claude-opus: Most capable, highest quality
#
# Strengths:
#   - Excellent at following complex instructions
#   - Strong reasoning and analysis
#   - Large context window (up to 200K tokens)
#   - Nuanced, thoughtful responses
#