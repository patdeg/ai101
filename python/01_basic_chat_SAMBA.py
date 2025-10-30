#!/usr/bin/env python3
"""
Example 1: Basic Chat with SambaNova

WHAT THIS DEMONSTRATES:
    - How to use SambaNova's enterprise AI platform
    - OpenAI-compatible API format with SambaNova
    - Using open-source Llama models via SambaNova
    - Enterprise-grade inference infrastructure
    - Standard library HTTP requests

WHAT YOU'LL LEARN:
    - SambaNova API endpoints
    - Using open models in production
    - OpenAI-compatible request format
    - Enterprise AI deployment patterns
    - Token usage with Llama models

PREREQUISITES:
    - Python 3.6 or higher
    - SAMBANOVA_API_KEY environment variable set
    - Internet connection

EXPECTED OUTPUT:
    - Full JSON response from SambaNova
    - Llama model's answer
    - Token usage statistics

Run with: python3 01_basic_chat_SAMBA.py
"""

import http.client
import json
import os

# ==============================================================================
# Step 1: Load SambaNova API credentials
# ==============================================================================

api_key = os.environ.get('SAMBANOVA_API_KEY')

if not api_key:
    print("Error: SAMBANOVA_API_KEY environment variable not set")
    print("Run: export SAMBANOVA_API_KEY='...'")
    print("Get your key from: https://sambanova.ai")
    exit(1)

# ==============================================================================
# Step 2: Build the request payload (OpenAI-compatible)
# ==============================================================================

payload = {
    "model": "Meta-Llama-3.1-8B-Instruct",  # Open-source Llama model
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
# Step 3: Connect to SambaNova API
# ==============================================================================

# SambaNova provides OpenAI-compatible endpoints
conn = http.client.HTTPSConnection("api.sambanova.ai")

# ==============================================================================
# Step 4: Send request with SambaNova headers
# ==============================================================================

# SambaNova uses OpenAI-style Bearer authentication
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

try:
    # Use OpenAI-compatible endpoint
    conn.request("POST", "/v1/chat/completions", payload_json, headers)

    # Get the response
    response = conn.getresponse()
    response_data = response.read().decode('utf-8')

    # Parse JSON response
    response_json = json.loads(response_data)

    # ==============================================================================
    # Step 5: Display the results
    # ==============================================================================

    print("Full SambaNova Response:")
    print(json.dumps(response_json, indent=2))

    # Extract and display the answer (OpenAI-compatible format)
    if 'choices' in response_json and len(response_json['choices']) > 0:
        answer = response_json['choices'][0]['message']['content']
        print("\n" + "="*50)
        print("Llama's Answer (via SambaNova):")
        print(answer)

    # Show token usage
    if 'usage' in response_json:
        usage = response_json['usage']
        print("\n" + "="*50)
        print("Token Usage:")
        print(f"  Prompt tokens: {usage.get('prompt_tokens', 0)}")
        print(f"  Completion tokens: {usage.get('completion_tokens', 0)}")
        print(f"  Total tokens: {usage.get('total_tokens', 0)}")

    # Display model info
    if 'model' in response_json:
        print(f"\nModel used: {response_json['model']}")
        print("(Open-source Llama model via SambaNova)")

    # Check for errors
    if 'error' in response_json:
        print("\nAPI Error:")
        print(json.dumps(response_json['error'], indent=2))

except Exception as e:
    print(f"Request failed: {e}")
    print("\nTroubleshooting:")
    print("1. Check your internet connection")
    print("2. Verify your SambaNova API key")
    print("3. Check SambaNova service status")

finally:
    conn.close()

# ==============================================================================
# SambaNova API Notes:
# ==============================================================================
#
# Key features:
#   - OpenAI-compatible format (easy migration)
#   - Focus on open-source models (Llama, Mistral)
#   - Enterprise-grade infrastructure
#   - High-performance custom hardware
#   - Privacy-focused deployments
#
# Available models:
#   - Meta-Llama-3.1-8B-Instruct: Fast, efficient
#   - Meta-Llama-3.1-70B-Instruct: More capable
#   - Meta-Llama-3.1-405B-Instruct: State-of-the-art
#
# Use cases:
#   - Enterprise deployments requiring open models
#   - Privacy-sensitive applications
#   - High-throughput production workloads
#   - Cost-effective scaling
#
# Advantages:
#   - No vendor lock-in (open models)
#   - Predictable performance
#   - Enterprise support available
#