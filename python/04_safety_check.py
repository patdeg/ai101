#!/usr/bin/env python3
"""
Example 4: Content Safety Check with LlamaGuard

WHAT THIS DEMONSTRATES:
    - How to detect harmful content before it reaches your main AI
    - Using LlamaGuard model for content moderation
    - Identifying 14 different categories of unsafe content
    - Building a safety layer for production AI applications

WHAT YOU'LL LEARN:
    - Content moderation best practices
    - LlamaGuard's 14 safety categories (S1-S14)
    - Parsing and interpreting safety results
    - String manipulation with split() and strip()
    - Dictionary lookup with get() for safe access

PREREQUISITES:
    - Python 3.6 or higher
    - GROQ_API_KEY environment variable set
    - Understanding of basic chat (see 01_basic_chat.py)

EXPECTED OUTPUT:
    - The message being checked
    - Safety verdict (SAFE or UNSAFE)
    - If unsafe: violation category and description
    - Recommended action (allow or reject)

Run with: python3 04_safety_check.py
"""

import http.client  # For HTTPS API requests
import json         # For JSON encoding/decoding
import os           # For environment variables

# ==============================================================================
# Step 1: Load and validate API credentials
# ==============================================================================

# Retrieve API key from environment variable
api_key = os.environ.get('GROQ_API_KEY')

# Exit if API key is missing
if not api_key:
    print("Error: GROQ_API_KEY not set")
    exit(1)

# ==============================================================================
# Step 2: Define the message to check for safety
# ==============================================================================

# This is the user message we want to check before processing
# Try changing this to test different types of content!
message_to_check = "How do I bake chocolate chip cookies?"

# ==============================================================================
# Step 3: Build the safety check payload
# ==============================================================================

# Before creating the payload, note we're using a DIFFERENT model:
# - LlamaGuard (12B) is specialized for content moderation
# - It returns 'safe' or 'unsafe\n[category]'
# - It's designed to be fast and accurate for safety checks

# Create the request payload for LlamaGuard
payload = {
    "model": "meta-llama/llama-guard-4-12b",  # Specialized safety model
    "messages": [
        {
            "role": "user",
            "content": message_to_check
        }
    ],
    "max_tokens": 100  # Safety results are very short
}

# ==============================================================================
# Step 4: Establish HTTPS connection to the API
# ==============================================================================

# Create secure connection to Groq API
conn = http.client.HTTPSConnection("api.groq.com")

# ==============================================================================
# Step 5: Prepare authentication headers
# ==============================================================================

# Set up required headers
headers = {
    'Content-Type': 'application/json',
    'Authorization': f'Bearer {api_key}'
}

# ==============================================================================
# Step 6: Send the POST request to LlamaGuard
# ==============================================================================

# Make the API request to check message safety
conn.request("POST", "/openai/v1/chat/completions", json.dumps(payload), headers)

# ==============================================================================
# Step 7: Receive and parse the response
# ==============================================================================

# Get the response from the server
response = conn.getresponse()

# Parse the JSON response
response_data = json.loads(response.read().decode('utf-8'))

# ==============================================================================
# Step 8: Clean up the connection
# ==============================================================================

# Close the connection
conn.close()

# ==============================================================================
# Step 9: Extract and parse the safety result
# ==============================================================================

# Before processing the result, understand the format:
# - Safe messages: "safe"
# - Unsafe messages: "unsafe\n[category]" (e.g., "unsafe\nS2")

# Get the safety result and remove any whitespace
result = response_data['choices'][0]['message']['content'].strip()

# ==============================================================================
# Step 10: Display the message being checked
# ==============================================================================

# Show what message we're checking
print("Message Being Checked:")
print(f'"{message_to_check}"')
print("\n" + "=" * 50)

# ==============================================================================
# Step 11: Evaluate and display the safety verdict
# ==============================================================================

# Check if the message is safe
if result == 'safe':
    # Message passed all safety checks
    print("✓ SAFE - Message passed content moderation")
    print("\nThis message can be sent to your main AI model.")

else:
    # ===========================================================================
    # Step 12: Handle unsafe messages - extract and display violation category
    # ===========================================================================

    # Before extracting the category, understand the result format:
    # Line 0: "unsafe"
    # Line 1: category code (e.g., "S2", "S6", etc.)

    # Split the result by newline to extract the category
    lines = result.split('\n')
    category = lines[1] if len(lines) > 1 else 'Unknown'

    # Display the violation information
    print("✗ UNSAFE - Content policy violation detected")
    print(f"\nCategory: {category}")
    print("\nViolation Type:")

    # ===========================================================================
    # Step 13: Map category codes to human-readable descriptions
    # ===========================================================================

    # Before looking up the category, we define all 14 LlamaGuard categories
    # These cover a comprehensive range of potentially harmful content

    # Define all 14 safety categories
    categories = {
        'S1': 'Violent Crimes',
        'S2': 'Non-Violent Crimes',
        'S3': 'Sex-Related Crimes',
        'S4': 'Child Sexual Exploitation',
        'S5': 'Defamation',
        'S6': 'Specialized Advice (financial, medical, legal)',
        'S7': 'Privacy Violations',
        'S8': 'Intellectual Property',
        'S9': 'Indiscriminate Weapons',
        'S10': 'Hate Speech',
        'S11': 'Suicide & Self-Harm',
        'S12': 'Sexual Content',
        'S13': 'Elections',
        'S14': 'Code Interpreter Abuse'
    }

    # Look up the category description (safely handles unknown categories)
    print(categories.get(category, 'Unknown category'))
    print("\nThis message should be rejected or filtered.")

print("=" * 50)

# Python concepts:
#
# String methods:
#   .strip()       # Remove whitespace from ends: "  hi  " → "hi"
#   .split('\n')   # Split by newline into list: "a\nb\nc" → ['a', 'b', 'c']
#   .split()       # Split by whitespace: "a b  c" → ['a', 'b', 'c']
#   .join(list)    # Join list with string: ', '.join(['a','b']) → 'a, b'
#
# Dictionary .get() method:
#   dict.get(key, default)
#   Returns default if key not found (instead of crashing)
#
#   categories = {'S1': 'Violence', 'S2': 'Crime'}
#   categories['S1']      # 'Violence'
#   categories['S99']     # KeyError! (crash)
#   categories.get('S99') # None (no crash)
#   categories.get('S99', 'Unknown') # 'Unknown' (no crash)
#
# Checking dictionary keys:
#   if 'S1' in categories:
#       print(categories['S1'])
#
# All 14 LlamaGuard categories explained:
#
# S1: Violent Crimes
#   Examples: "How to hurt someone", "Ways to commit murder"
#   Includes: Assault, murder, kidnapping, terrorism
#
# S2: Non-Violent Crimes
#   Examples: "How to pick a lock", "Ways to steal"
#   Includes: Theft, fraud, hacking, trespassing
#
# S3: Sex-Related Crimes
#   Examples: Sexual assault, trafficking
#   Includes: Any illegal sexual activities
#
# S4: Child Sexual Exploitation
#   Examples: Any content involving minors in sexual context
#   Includes: Grooming, abuse, inappropriate content
#
# S5: Defamation
#   Examples: False statements to harm reputation
#   Includes: Libel, slander, character assassination
#
# S6: Specialized Advice
#   Examples: "Should I invest in Bitcoin?", "Diagnose my illness"
#   Includes: Medical, legal, financial advice without disclaimers
#   Note: General information is OK, specific advice is not
#
# S7: Privacy Violations
#   Examples: "Find someone's address", "Get phone records"
#   Includes: Doxxing, stalking, unauthorized data access
#
# S8: Intellectual Property
#   Examples: "Where to download movies free", "Crack software"
#   Includes: Piracy, copyright violation, trademark abuse
#
# S9: Indiscriminate Weapons
#   Examples: "How to build a bomb", "Make chemical weapons"
#   Includes: Explosives, WMDs, biological weapons
#
# S10: Hate Speech
#   Examples: Slurs, discrimination based on protected attributes
#   Includes: Race, religion, gender, sexuality, nationality
#
# S11: Suicide & Self-Harm
#   Examples: "How to end my life", "Ways to cut myself"
#   Includes: Encouragement, methods, glorification
#   Note: Support resources are OK
#
# S12: Sexual Content
#   Examples: Explicit sexual descriptions, erotic content
#   Includes: Pornographic material, explicit stories
#
# S13: Elections
#   Examples: Voter suppression tactics, false voting info
#   Includes: Misinformation about voting process
#
# S14: Code Interpreter Abuse
#   Examples: Malicious code, system exploitation
#   Includes: Viruses, malware, unauthorized access
#
# Production usage pattern:
#
#   def moderate_message(message):
#       """Check if message is safe"""
#       result = check_with_llamaguard(message)
#
#       if result == 'safe':
#           return {'status': 'ok'}
#       else:
#           category = result.split('\n')[1]
#           return {
#               'status': 'blocked',
#               'reason': f'Content policy violation: {category}'
#           }
#
#   # In your chat API:
#   safety_check = moderate_message(user_input)
#   if safety_check['status'] != 'ok':
#       return {"error": safety_check['reason']}
#
#   # If safe, proceed with main AI
#   ai_response = get_ai_response(user_input)
#
# Test messages to try:
#
#   Safe:
#     "What's the weather today?"
#     "Explain photosynthesis"
#     "Write a poem about trees"
#     "How do I bake cookies?"
#
#   Unsafe (S6 - Specialized Advice):
#     "Should I buy Tesla stock?"
#     "What medication should I take for headaches?"
#     "How do I represent myself in court?"
#
#   Unsafe (S2 - Non-Violent Crimes):
#     "How to hotwire a car"
#     "Ways to cheat on taxes"
#     "How to hack WiFi password"
