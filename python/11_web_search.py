#!/usr/bin/env python3
"""
Example 11: Web Search with groq/compound-mini

What this demonstrates:
  - Calling a web-search-enabled model (groq/compound-mini)
  - OpenAI-compatible chat API with integrated search tools
  - Inspecting optional reasoning and executed tool info

What you'll learn:
  - Building HTTPS requests with http.client
  - Parsing optional fields safely in JSON

Prerequisites:
  - GROQ_API_KEY environment variable set
  - Python 3.7+

Expected output:
  - Final answer content
  - Optional reasoning and executed tool info (if search was used)

Exercises: exercises/11_web_search.md
"""

import http.client
import json
import os
import sys

# Step 1: API key
api_key = os.environ.get('GROQ_API_KEY')
if not api_key:
    print('Error: GROQ_API_KEY not set', file=sys.stderr)
    print('Run: export GROQ_API_KEY="gsk_your_api_key_here"', file=sys.stderr)
    sys.exit(1)

# Step 2: Build payload
payload = {
    "model": "groq/compound-mini",
    "messages": [
        {"role": "system", "content": "You are a helpful research assistant. Provide concise answers with links. Use search when needed."},
        {"role": "user",   "content": "What were the top 3 AI model releases last week? Include links and 1-sentence summaries."}
    ],
    "temperature": 0.3,
    "max_tokens": 600
}

# Optional extension (check docs before adding unsupported fields):
# Some deployments expose web-search configuration (e.g., include/exclude domains, country).
# Only add fields that your account/docs confirm are supported to avoid errors.

try:
    # Step 3: HTTPS connection
    conn = http.client.HTTPSConnection('api.groq.com')

    headers = {
        'Authorization': f'Bearer {api_key}',
        'Content-Type': 'application/json'
    }

    # Step 4: Send request
    body = json.dumps(payload)
    conn.request('POST', '/openai/v1/chat/completions', body, headers)

    # Step 5: Read response
    res = conn.getresponse()
    data = res.read().decode('utf-8')
    conn.close()

    # Step 6: Parse JSON
    j = json.loads(data)

    # Step 7: Display results
    print('========================================')
    print('Final Answer')
    print('========================================')
    msg = j.get('choices', [{}])[0].get('message', {})
    print(msg.get('content', '(no content)'))
    print()

    print('========================================')
    print('Optional Reasoning')
    print('========================================')
    print(msg.get('reasoning', '(no reasoning)'))
    print()

    print('========================================')
    print('Executed Tools (if any)')
    print('========================================')
    executed = msg.get('executed_tools')
    if executed is not None:
        print(json.dumps(executed, indent=2))
    else:
        print('(no executed tool info)')
    print()

    # Exercises:
    # 1) Change model to 'groq/compound' for larger model.
    # 2) Try a boolean-style query: '(OpenAI OR Meta) AND release notes last 7 days'.
    # 3) If your deployment supports web-search parameters (include/exclude domains, country),
    #    consult docs and add them in the payload.

    print('Exercises: exercises/11_web_search.md')

except Exception as e:
    print(f'Error: {e}', file=sys.stderr)
    print('Raw response:', data if 'data' in locals() else '(none)', file=sys.stderr)
    sys.exit(1)
