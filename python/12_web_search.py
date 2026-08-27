#!/usr/bin/env python3
"""
Example 11: Web Search with OpenRouter's :online model suffix

What this demonstrates:
  - Calling a web-search-augmented model via OpenRouter's ":online" suffix
  - OpenAI-compatible chat API with integrated search
  - Inspecting optional reasoning and citation info

What you'll learn:
  - Building HTTPS requests with http.client
  - Parsing optional fields safely in JSON

Prerequisites:
  - OPENROUTER_API_KEY environment variable set
  - Python 3.7+

Expected output:
  - Final answer content, informed by live web search results
  - Optional reasoning and annotation info (if present)

Exercises: exercises/11_web_search.md
"""

import http.client
import json
import os
import sys

# Step 1: API key
api_key = os.environ.get('OPENROUTER_API_KEY')
if not api_key:
    print('Error: OPENROUTER_API_KEY not set', file=sys.stderr)
    print('Run: export OPENROUTER_API_KEY="your_api_key_here"', file=sys.stderr)
    sys.exit(1)

# Step 2: Build payload
# Appending ":online" to any model slug turns on OpenRouter's built-in
# web-search plugin (default engine Exa, ~$0.005/request) for this call.
payload = {
    "model": "openai/gpt-oss-20b:online",
    "messages": [
        {"role": "system", "content": "You are a helpful research assistant. Provide concise answers with links. Use search when needed."},
        {"role": "user",   "content": "What were the top 3 AI model releases last week? Include links and 1-sentence summaries."}
    ],
    "temperature": 0.3,
    "max_tokens": 1200
}

# Optional extension (check docs before adding unsupported fields):
# Some deployments expose web-search configuration (e.g., include/exclude domains, country).
# Only add fields that your account/docs confirm are supported to avoid errors.

try:
    # Step 3: HTTPS connection
    conn = http.client.HTTPSConnection('openrouter.ai')

    headers = {
        'Authorization': f'Bearer {api_key}',
        'Content-Type': 'application/json'
    }

    # Step 4: Send request
    body = json.dumps(payload)
    conn.request('POST', '/api/v1/chat/completions', body, headers)

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
    # 1) Change the model before ":online" to try a different underlying LLM.
    # 2) Try a boolean-style query: '(OpenAI OR Meta) AND release notes last 7 days'.
    # 3) OpenRouter's web plugin supports options like max_results and search_prompt —
    #    see https://openrouter.ai/docs/features/web-search and add a "plugins" block.

    print('Exercises: exercises/11_web_search.md')

except Exception as e:
    print(f'Error: {e}', file=sys.stderr)
    print('Raw response:', data if 'data' in locals() else '(none)', file=sys.stderr)
    sys.exit(1)
