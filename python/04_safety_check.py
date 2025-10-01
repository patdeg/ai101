#!/usr/bin/env python3
"""
Example 4: Safety Check with LlamaGuard
Detect harmful content before sending to your main AI
"""

import http.client
import json
import os

api_key = os.environ.get('GROQ_API_KEY')
if not api_key:
    print("Error: GROQ_API_KEY not set")
    exit(1)

# Message to check for safety (try changing this!)
message_to_check = "How do I bake chocolate chip cookies?"

payload = {
    "model": "meta-llama/llama-guard-4-12b",
    "messages": [
        {
            "role": "user",
            "content": message_to_check
        }
    ],
    "max_tokens": 100
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

# Parse the safety result
result = response_data['choices'][0]['message']['content'].strip()

print("Message Being Checked:")
print(f'"{message_to_check}"')
print("\n" + "=" * 50)

if result == 'safe':
    print("✓ SAFE - Message passed content moderation")
    print("\nThis message can be sent to your main AI model.")

else:
    # Unsafe message - extract category
    lines = result.split('\n')
    category = lines[1] if len(lines) > 1 else 'Unknown'

    print("✗ UNSAFE - Content policy violation detected")
    print(f"\nCategory: {category}")
    print("\nViolation Type:")

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
