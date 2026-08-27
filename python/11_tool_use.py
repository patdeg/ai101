#!/usr/bin/env python3
"""
Example 10: Tool Use (Function Calling) - AI Agents with OpenRouter + Tavily

Demonstrates:
- OpenRouter function calling
- Tavily Search and Extract as tools
- Multi-step agent workflow

Note: Uses Python standard library only
For production, consider: pip install groq tavily-python
"""

import http.client
import json
import os
import sys

# Check for API keys
if not os.environ.get('OPENROUTER_API_KEY') or not os.environ.get('TAVILY_API_KEY'):
    print('Error: Both OPENROUTER_API_KEY and TAVILY_API_KEY must be set', file=sys.stderr)
    print('Get an OpenRouter key: https://openrouter.ai/keys', file=sys.stderr)
    print('Get a Tavily key: https://tavily.com', file=sys.stderr)
    sys.exit(1)

# Tool functions
def tavily_search(query):
    """Search the web using Tavily API"""
    print(f'[Tool] Executing tavily_search with query: {query}')

    conn = http.client.HTTPSConnection('api.tavily.com')
    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["TAVILY_API_KEY"]}'
    }

    body = json.dumps({
        'query': query,
        'max_results': 5,
        'include_answer': True
    })

    conn.request('POST', '/search', body, headers)
    response = conn.getresponse()
    data = response.read().decode('utf-8')
    conn.close()

    return json.loads(data)

def tavily_extract(url):
    """Extract content from a URL using Tavily API"""
    print(f'[Tool] Executing tavily_extract for URL: {url}')

    conn = http.client.HTTPSConnection('api.tavily.com')
    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["TAVILY_API_KEY"]}'
    }

    body = json.dumps({
        'urls': [url],
        'extract_depth': 'basic'
    })

    conn.request('POST', '/extract', body, headers)
    response = conn.getresponse()
    data = response.read().decode('utf-8')
    conn.close()

    return json.loads(data)

def execute_tool(function_name, arguments):
    """Execute a tool by name"""
    if function_name == 'tavily_search':
        return tavily_search(arguments['query'])
    elif function_name == 'tavily_extract':
        return tavily_extract(arguments['url'])
    else:
        raise ValueError(f'Unknown function: {function_name}')

def call_groq(messages, tools=None, tool_choice=None):
    """Call OpenRouter chat completions"""
    conn = http.client.HTTPSConnection('openrouter.ai')
    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["OPENROUTER_API_KEY"]}'
    }

    request_data = {
        'model': 'openai/gpt-oss-20b',
        'messages': messages
    }

    if tools:
        request_data['tools'] = tools
        request_data['tool_choice'] = 'auto'
    elif tool_choice:
        request_data['tool_choice'] = tool_choice

    body = json.dumps(request_data)

    conn.request('POST', '/api/v1/chat/completions', body, headers)
    response = conn.getresponse()
    data = response.read().decode('utf-8')
    conn.close()

    return json.loads(data)

def main():
    print('========================================')
    print('Groq Tool Use - AI Agent Demo')
    print('========================================\n')

    user_query = 'Search for the latest developments in quantum computing and provide a summary'

    print(f'User Query: {user_query}\n')
    print('Step 1: Sending query to AI with tool definitions...\n')

    # Tool definitions
    tools = [
        {
            'type': 'function',
            'function': {
                'name': 'tavily_search',
                'description': 'Search the web for current information using Tavily API',
                'parameters': {
                    'type': 'object',
                    'properties': {
                        'query': {
                            'type': 'string',
                            'description': 'The search query'
                        }
                    },
                    'required': ['query']
                }
            }
        },
        {
            'type': 'function',
            'function': {
                'name': 'tavily_extract',
                'description': 'Extract content from a specific URL',
                'parameters': {
                    'type': 'object',
                    'properties': {
                        'url': {
                            'type': 'string',
                            'description': 'The URL to extract content from'
                        }
                    },
                    'required': ['url']
                }
            }
        }
    ]

    # Step 1: Initial request
    initial_response = call_groq([
        {'role': 'user', 'content': user_query}
    ], tools)

    # Check for tool calls
    tool_calls = initial_response['choices'][0]['message'].get('tool_calls')

    if not tool_calls:
        print('AI answered without using tools:\n')
        print(initial_response['choices'][0]['message']['content'])
        return

    # Step 2: Execute tool calls
    print('========================================')
    print('AI decided to use tools!')
    print('========================================\n')
    print(f'Number of tool calls: {len(tool_calls)}\n')

    tool_messages = []

    for tool_call in tool_calls:
        print(f'Function: {tool_call["function"]["name"]}')
        print(f'Arguments: {tool_call["function"]["arguments"]}\n')

        args = json.loads(tool_call['function']['arguments'])
        result = execute_tool(tool_call['function']['name'], args)

        tool_messages.append({
            'role': 'tool',
            'tool_call_id': tool_call['id'],
            'content': json.dumps(result)
        })

        print('✓ Tool executed successfully\n')

    # Step 3: Send results back to AI
    print('Step 2: Sending tool results back to AI for final answer...\n')

    # tool_choice='none' tells the model this is the final answer turn, not
    # another chance to call a tool - without it, small models occasionally
    # echo malformed tool-call syntax instead of a plain-text summary. Even
    # with this, budget models can still occasionally return an empty/odd
    # response on a multi-turn tool continuation - if that happens, just
    # re-run; a production system would retry automatically.
    final_response = call_groq([
        {'role': 'user', 'content': user_query},
        initial_response['choices'][0]['message'],
        *tool_messages
    ], tool_choice='none')

    # Display final response
    print('========================================')
    print('Final AI Response')
    print('========================================\n')
    print(final_response['choices'][0]['message']['content'])
    print('')

    # Display token usage
    print('========================================')
    print('Token Usage')
    print('========================================\n')
    usage = final_response['usage']
    print(f'Prompt tokens: {usage["prompt_tokens"]}')
    print(f'Completion tokens: {usage["completion_tokens"]}')
    print(f'Total tokens: {usage["total_tokens"]}')

    cost = (usage['prompt_tokens'] * 0.11 + usage['completion_tokens'] * 0.34) / 1000000
    print(f'Cost: ${cost:.6f}\n')

    print('========================================')
    print('Summary')
    print('========================================\n')
    print('✓ AI agent successfully used tools')
    print(f'✓ Executed {len(tool_calls)} tool call(s)')
    print('✓ Retrieved real-time information')
    print('✓ Generated informed response\n')

if __name__ == '__main__':
    try:
        main()
    except Exception as error:
        print(f'Error: {error}', file=sys.stderr)
        sys.exit(1)
