#!/usr/bin/env python3
"""
Example 08: Tavily Search API - Web Search with AI-Powered Answers

What is Tavily?
---------------
Tavily is a search API designed for AI agents and applications:
- Web search with AI-generated answer synthesis
- Topic-specific search (general, news, finance)
- Image search with AI descriptions
- Domain filtering (include/exclude)
- Time-restricted searches
- Clean, structured data (no ads)

Free tier: 1,000 searches/month
Get your API key at: https://tavily.com

This example:
------------
- Searches for "traditional French bread recipes"
- Requests AI-generated answer from multiple sources
- Returns 5 clean, relevant results
- No external dependencies (uses Python standard library)
"""

import http.client
import json
import os
import sys

# Check for API key
if not os.environ.get('TAVILY_API_KEY'):
    print('Error: TAVILY_API_KEY environment variable not set\n', file=sys.stderr)
    print('Get your free API key:', file=sys.stderr)
    print('  1. Visit https://tavily.com', file=sys.stderr)
    print('  2. Sign up (1,000 free searches/month)', file=sys.stderr)
    print('  3. Copy your API key (starts with "tvly-")', file=sys.stderr)
    print('  4. Set: export TAVILY_API_KEY="tvly-your-key-here"\n', file=sys.stderr)
    sys.exit(1)

# Tavily Search API Request Parameters
#
# REQUIRED:
# - query: Search query string
# - api_key: Your Tavily API key
#
# OPTIONAL:
# - search_depth: "basic" (default) or "advanced"
# - topic: "general" (default), "news", or "finance"
# - days: For news - days back to search (default: 7)
# - max_results: 1-20 results (default: 5)
# - include_answer: True/False/"basic"/"advanced"
# - include_images: Include relevant images
# - include_image_descriptions: AI descriptions of images
# - include_domains: List of domains to include (max 300)
# - exclude_domains: List of domains to exclude (max 150)

request_data = {
    'query': 'traditional French bread recipes',
    'search_depth': 'basic',
    'include_answer': True,
    'max_results': 5,
    'include_images': False
}

print('===========================================')
print('Tavily Search API - French Bread Recipes')
print('===========================================\n')
print('Query: traditional French bread recipes')
print('Options: include_answer=True, max_results=5\n')
print('Sending request to Tavily...\n')

# Make API request
try:
    conn = http.client.HTTPSConnection('api.tavily.com')

    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["TAVILY_API_KEY"]}'
    }

    body = json.dumps(request_data)

    conn.request('POST', '/search', body, headers)

    response = conn.getresponse()
    data = response.read().decode('utf-8')

    conn.close()

    # Parse response
    result = json.loads(data)

    # Check for errors
    if 'error' in result:
        print(f'API Error: {result["error"]}', file=sys.stderr)
        sys.exit(1)

    # Display AI-generated answer
    print('========================================')
    print('AI-Generated Answer (Answer Synthesis)')
    print('========================================\n')
    print(result.get('answer', 'No answer provided'))
    print('')

    # Display search results
    print('========================================')
    print('Search Results (Top 5)')
    print('========================================\n')
    print(f'Found {len(result["results"])} results:\n')

    for i, item in enumerate(result['results']):
        print(f'[{i + 1}] {item["title"]}')
        print(f'URL: {item["url"]}')
        print(f'Relevance Score: {item["score"]}')
        print(f'Summary: {item["content"]}')
        print('')

    # Display metadata
    print('========================================')
    print('Request Metadata')
    print('========================================\n')
    print(f'Response Time: {result["response_time"]}s')
    print(f'Request ID: {result.get("request_id", "N/A")}')
    print('')

    # Display raw JSON
    print('========================================')
    print('Raw API Response (formatted)')
    print('========================================\n')
    print(json.dumps(result, indent=2))
    print('')

    # Summary
    print('===========================================')
    print('Summary')
    print('===========================================\n')
    print('✓ Tavily Search completed successfully')
    print('✓ AI Answer synthesized from multiple sources')
    print(f'✓ Returned {len(result["results"])} relevant results')
    print(f'✓ Response time: {result["response_time"]}s\n')
    print('Try the exercises to explore:')
    print('  - News search with time restrictions')
    print('  - Finance search with domain filtering')
    print('  - Image search with descriptions')
    print('  - Building executive summaries with AI')
    print('  - Sending results via ntfy.sh notifications\n')

except Exception as error:
    print(f'Error: {error}', file=sys.stderr)
    sys.exit(1)
