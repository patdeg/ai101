#!/usr/bin/env python3
"""
Example 2: System + User Prompt
Control AI behavior with system instructions
"""

import http.client
import json
import os

api_key = os.environ.get('GROQ_API_KEY')
if not api_key:
    print("Error: GROQ_API_KEY not set")
    exit(1)

payload = {
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
}

conn = http.client.HTTPSConnection("api.groq.com")
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

conn.request("POST", "/openai/v1/chat/completions", json.dumps(payload), headers)
response = conn.getresponse()
response_data = json.loads(response.read().decode('utf-8'))
conn.close()

print("System Prompt Used:")
print(payload['messages'][0]['content'])

print("\nUser Question:")
print(payload['messages'][1]['content'])

print("\nAI Response (following system rules):")
print(response_data['choices'][0]['message']['content'])

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
