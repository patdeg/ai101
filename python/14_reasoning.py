#!/usr/bin/env python3
"""
Example 13: Reasoning with openai/gpt-oss-20b

Demonstrates:
- Reasoning models that show step-by-step thinking
- Three reasoning formats: raw, parsed, hidden
- Three reasoning effort levels: low, medium, high
- Cache optimization for cost savings

What you'll learn:
- How reasoning models differ from chat models
- When to use different reasoning formats
- How to optimize message order for cache hits
- Token usage and cost tracking

Note: Uses Python standard library only
For production, consider: pip install groq
"""

import http.client
import json
import os
import sys

# Check for API key
if not os.environ.get('DEMETERICS_API_KEY'):
    print('Error: DEMETERICS_API_KEY environment variable not set', file=sys.stderr)
    print('Get your Managed LLM Key from: https://demeterics.com', file=sys.stderr)
    sys.exit(1)

def call_reasoning(messages, format_type=None, effort='medium'):
    """Call Demeterics Groq proxy with reasoning model"""
    conn = http.client.HTTPSConnection('api.demeterics.com')

    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["DEMETERICS_API_KEY"]}'
    }

    request_data = {
        'model': 'openai/gpt-oss-20b',
        'messages': messages,
        'temperature': 0.6,
        'max_completion_tokens': 1024,
        'reasoning_effort': effort
    }

    if format_type:
        request_data['reasoning_format'] = format_type

    body = json.dumps(request_data)

    conn.request('POST', '/groq/v1/chat/completions', body, headers)
    response = conn.getresponse()
    data = response.read().decode('utf-8')
    conn.close()

    return json.loads(data)

def main():
    print('========================================')
    print('Reasoning with openai/gpt-oss-20b')
    print('========================================\n')

    query = "How many 'r' letters are in the word 'strawberry'? Think through this step-by-step."
    messages = [{'role': 'user', 'content': query}]

    print(f'Query: {query}\n')

    # Demo 1: Raw format (default)
    print('========================================')
    print('Demo 1: Raw Format (reasoning in <think> tags)')
    print('========================================\n')

    raw_response = call_reasoning(messages)
    print('Response (raw format):')
    print(raw_response['choices'][0]['message']['content'])
    print('')

    print('Token Usage (Demo 1):')
    print(f'  Prompt tokens: {raw_response["usage"]["prompt_tokens"]}')
    print(f'  Completion tokens: {raw_response["usage"]["completion_tokens"]}')
    print(f'  Total tokens: {raw_response["usage"]["total_tokens"]}\n')

    # Demo 2: Parsed format
    print('========================================')
    print('Demo 2: Parsed Format (separate reasoning field)')
    print('========================================\n')

    parsed_response = call_reasoning(messages, 'parsed')

    if 'reasoning' in parsed_response['choices'][0]['message']:
        print('Reasoning process:')
        print(parsed_response['choices'][0]['message']['reasoning'])
        print('')

    print('Final answer:')
    print(parsed_response['choices'][0]['message']['content'])
    print('')

    # Demo 3: Hidden format
    print('========================================')
    print('Demo 3: Hidden Format (only final answer)')
    print('========================================\n')

    hidden_response = call_reasoning(messages, 'hidden')
    print('Response (hidden format):')
    print(hidden_response['choices'][0]['message']['content'])
    print('')

    # Cost calculation
    print('========================================')
    print('Cost Analysis')
    print('========================================\n')

    input_cost_per_1m = 0.10
    cached_input_cost_per_1m = 0.05
    output_cost_per_1m = 0.50

    prompt_tokens = raw_response['usage']['prompt_tokens']
    completion_tokens = raw_response['usage']['completion_tokens']

    input_cost = (prompt_tokens * input_cost_per_1m) / 1000000
    output_cost = (completion_tokens * output_cost_per_1m) / 1000000
    total_cost = input_cost + output_cost

    print('Pricing (openai/gpt-oss-20b):')
    print('  Input tokens: $0.10 per 1M')
    print('  Cached input tokens: $0.05 per 1M (50% discount!)')
    print('  Output tokens: $0.50 per 1M\n')

    print('This request:')
    print(f'  Input cost: ${input_cost:.6f} ({prompt_tokens} tokens)')
    print(f'  Output cost: ${output_cost:.6f} ({completion_tokens} tokens)')
    print(f'  Total cost: ${total_cost:.6f}\n')

    # Cache optimization explanation
    print('========================================')
    print('Cache Optimization Strategy')
    print('========================================\n')

    print('What is cached input?')
    print('Groq caches parts of your prompt to speed up responses and reduce costs.')
    print('Cached tokens are 50% cheaper ($0.05/1M vs $0.10/1M for this model).\n')

    print('How to maximize cache hits:')
    print('Order messages from most constant to most variable:\n')
    print('  1. System prompts (always the same across requests)')
    print('  2. Context/documents (same during a conversation)')
    print('  3. User questions (changes with each request)\n')

    print('Example: agentresume.ai recruiter chatbot\n')
    print('Optimal message order:')
    print('''[
  {
    "role": "system",
    "content": "You are a recruiter assistant. Analyze resumes..."
  },                                                    ← Always cached
  {
    "role": "user",
    "content": "Candidate Resume:\\nJohn Doe\\nSkills: Python, Go..."
  },                                                    ← Cached per conversation
  {
    "role": "user",
    "content": "Does this candidate have Python experience?"
  }                                                     ← Not cached (changes each Q)
]
''')

    print('Cache benefit calculation:')
    print('If 1000 tokens are always cached (system + resume):')
    print('  Normal cost: 1000 × $0.10 / 1M = $0.0001')
    print('  Cached cost: 1000 × $0.05 / 1M = $0.00005')
    print('  Savings: 50% per request!\n')

    print('========================================')
    print('Reasoning Effort Comparison')
    print('========================================\n')

    print('Reasoning effort levels:')
    print('  low    - Quick analysis, fewer reasoning tokens')
    print('  medium - Balanced reasoning (default)')
    print('  high   - Deep analysis, more reasoning tokens\n')
    print("Use 'low' for simple questions, 'high' for complex reasoning tasks.\n")

    print('========================================')
    print('Summary')
    print('========================================\n')
    print('✓ Demonstrated three reasoning formats')
    print('✓ Showed token usage and costs')
    print('✓ Explained cache optimization strategy')
    print('✓ Model excels at step-by-step logical reasoning\n')

if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        print(f'Error: {error}', file=sys.stderr)
        sys.exit(1)
