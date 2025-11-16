#!/usr/bin/env python3
"""
Example 12: Code Execution with openai/gpt-oss-20b

What this demonstrates:
  - A model that can execute Python to solve tasks
  - Inspecting optional reasoning and executed tool details

What you'll learn:
  - Prompting for code execution
  - Retrieving executed code and outputs (when provided)

Prerequisites:
  - DEMETERICS_API_KEY set

Expected output:
  - Final answer content
  - message.reasoning and executed_tools (if present)

Exercises: exercises/12_code_execution.md
"""

import http.client
import json
import os
import sys

api_key = os.environ.get('DEMETERICS_API_KEY')
if not api_key:
    print('Error: DEMETERICS_API_KEY not set', file=sys.stderr)
    print('Run: export DEMETERICS_API_KEY="dmt_your_api_key_here"', file=sys.stderr)
    sys.exit(1)

payload = {
    "model": "openai/gpt-oss-20b",
    "messages": [
        {"role": "system", "content": "You can execute Python to verify results. If code raises an exception, show it and provide a corrected version."},
        {"role": "user",   "content": "1) Compute the 2000th prime number using Python. 2) Execute Python: print(1/0) to demonstrate the exception, then show how to catch it without crashing."}
    ],
    "tools": [
        {"type": "code_interpreter"}
    ],
    "tool_choice": "required",
    "temperature": 0.2,
    "max_tokens": 900
}

try:
    conn = http.client.HTTPSConnection('api.demeterics.com')
    headers = {
        'Authorization': f'Bearer {api_key}',
        'Content-Type': 'application/json'
    }

    body = json.dumps(payload)
    conn.request('POST', '/groq/v1/chat/completions', body, headers)
    res = conn.getresponse()
    data = res.read().decode('utf-8')
    conn.close()

    j = json.loads(data)
    msg = j.get('choices', [{}])[0].get('message', {})

    print('========================================')
    print('Final Answer')
    print('========================================')
    print(msg.get('content', '(no content)'))
    print()

    print('========================================')
    print('Reasoning & Tool Calls (if any)')
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
    # 1) Switch to 'openai/gpt-oss-120b' (if available) and compare.
    # 2) Ask to run code that may raise exceptions (invalid JSON) and show mitigation.
    # 3) Ask for the exact code executed and outputs.

    print('Exercises: exercises/12_code_execution.md')

except Exception as e:
    print(f'Error: {e}', file=sys.stderr)
    print('Raw response:', data if 'data' in locals() else '(none)', file=sys.stderr)
    sys.exit(1)
