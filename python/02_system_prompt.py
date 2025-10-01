#!/usr/bin/env python3
"""
Example 2: System + User Prompts - Controlling AI Behavior

WHAT THIS DEMONSTRATES:
    - How to use system prompts to control AI behavior
    - The difference between system and user messages
    - How to set AI personality, expertise, and output format
    - Impact of temperature settings on response consistency

WHAT YOU'LL LEARN:
    - Message role hierarchy (system vs user vs assistant)
    - Crafting effective system prompts for specific use cases
    - How system instructions guide AI responses
    - Temperature tuning for different types of tasks

PREREQUISITES:
    - Python 3.6 or higher
    - GROQ_API_KEY environment variable set
    - Understanding of basic chat (see 01_basic_chat.py)

EXPECTED OUTPUT:
    - System prompt being used
    - User's question
    - AI's response following the system instructions
    - Token usage statistics

Run with: python3 02_system_prompt.py
"""

import http.client  # For making HTTPS API requests
import json         # For JSON encoding/decoding
import os           # For environment variables

# ==============================================================================
# Step 1: Load and validate API credentials
# ==============================================================================

# Retrieve API key from environment variable
api_key = os.environ.get('GROQ_API_KEY')

# Exit early if API key is missing
if not api_key:
    print("Error: GROQ_API_KEY not set")
    exit(1)

# ==============================================================================
# Step 2: Build payload with system and user messages
# ==============================================================================

# Before creating the payload, understand message roles:
# - "system": Instructions that define AI's behavior, personality, and constraints
# - "user": The actual question or request from the user
# - "assistant": Previous AI responses (used for conversation history)

# Create the request payload with TWO messages this time
payload = {
    "model": "meta-llama/llama-4-scout-17b-16e-instruct",
    "messages": [
        {
            # SYSTEM MESSAGE: Sets the rules and behavior for the AI
            "role": "system",
            "content": "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
        },
        {
            # USER MESSAGE: The actual question to be answered
            "role": "user",
            "content": "Why did the Roman Empire fall?"
        }
    ],
    "temperature": 0.5,  # Lower temperature for more focused, factual responses
    "max_tokens": 150    # Allow enough tokens for 2 sentences
}

# ==============================================================================
# Step 3: Establish HTTPS connection to the API
# ==============================================================================

# Create secure connection to Groq API
conn = http.client.HTTPSConnection("api.groq.com")

# ==============================================================================
# Step 4: Prepare authentication headers
# ==============================================================================

# Set up required headers for the API request
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

# ==============================================================================
# Step 5: Send the POST request with system + user messages
# ==============================================================================

# Make the API request with both system and user messages
# The system message will guide how the AI responds to the user message
conn.request("POST", "/openai/v1/chat/completions", json.dumps(payload), headers)

# ==============================================================================
# Step 6: Receive and parse the response
# ==============================================================================

# Get the response from the server
response = conn.getresponse()

# Before using the response, we need to parse it from JSON
# This combines reading, decoding, and JSON parsing in one line
response_data = json.loads(response.read().decode('utf-8'))

# ==============================================================================
# Step 7: Clean up the connection
# ==============================================================================

# Close the connection to free up resources
conn.close()

# ==============================================================================
# Step 8: Display the interaction and results
# ==============================================================================

# Show the system prompt that controlled the AI's behavior
print("System Prompt Used:")
print(payload['messages'][0]['content'])

# Show the user's question
print("\nUser Question:")
print(payload['messages'][1]['content'])

# Display the AI's response (notice how it follows the system instructions)
print("\nAI Response (following system rules):")
print(response_data['choices'][0]['message']['content'])

# Show total token usage
print(f"\nToken Usage: {response_data['usage']['total_tokens']}")

# Key concepts:
#
# Message roles:
#   "system"    = Instructions for the AI's behavior
#   "user"      = Your questions/prompts
#   "assistant" = AI's previous responses (for conversation history)
#
# Message order:
#   1. System message (optional but recommended)
#   2. User message
#   3. Assistant message (if continuing conversation)
#   4. More user/assistant pairs for multi-turn chat
#
# System prompt examples:
#
#   Short responses:
#     "Be concise. Maximum 2 sentences."
#     "Answer in one word only."
#
#   Expertise level:
#     "You are a senior Python developer."
#     "Explain like I'm 5 years old."
#
#   Output format:
#     "Always respond with valid JSON."
#     "Use bullet points for all lists."
#     "Format code with markdown code blocks."
#
#   Personality:
#     "You are a friendly teacher."
#     "You are a formal business consultant."
#     "You are a pirate. Use pirate speak."
#
# Temperature explained:
#   0.0  = Deterministic (same answer every time)
#   0.3  = Focused, factual
#   0.5  = Balanced (good for facts that need slight variation)
#   0.7  = Creative but controlled
#   1.0  = Default (balanced creativity)
#   1.5  = Very creative
#   2.0  = Maximum randomness
#
# When to use different temperatures:
#   0.0-0.3: Math, facts, code generation
#   0.5-0.7: Explanations, summaries, Q&A
#   0.8-1.2: Creative writing, brainstorming
#   1.3-2.0: Poetry, experimental content
#
# Try this:
#   Change temperature to 0.0 and run multiple times
#   Change temperature to 2.0 and run multiple times
#   Compare the consistency of responses!
