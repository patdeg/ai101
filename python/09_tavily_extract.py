#!/usr/bin/env python3
"""
Example 09: Tavily Extract API - Extract Clean Content from Web Pages

What is Tavily Extract?
----------------------
Converts web pages into clean, structured markdown:
- Removes ads, navigation, and clutter
- Extracts main content in markdown format
- Optionally extracts images
- Perfect for feeding content to LLMs

This example:
------------
- Extracts content from a Medium article about California AI rules
- Uses "advanced" extraction depth
- Includes images from the article
- Saves clean markdown output to file
"""

import http.client
import json
import os
import sys

# Check for API key
if not os.environ.get('TAVILY_API_KEY'):
    print('Error: TAVILY_API_KEY environment variable not set\n', file=sys.stderr)
    print('Get your free API key at: https://tavily.com', file=sys.stderr)
    print('Then: export TAVILY_API_KEY="tvly-your-key-here"\n', file=sys.stderr)
    sys.exit(1)

article_url = 'https://medium.com/@pdeglon/california-ai-rules-explained-in-everyday-english-fea55637cb96'

request_data = {
    'urls': [article_url],
    'include_images': True,
    'extract_depth': 'advanced'
}

print('===========================================')
print('Tavily Extract API - Medium Article')
print('===========================================\n')
print(f'URL: {article_url}')
print('Options: extract_depth=advanced, include_images=True\n')
print('Sending request to Tavily...\n')

# Make API request
try:
    conn = http.client.HTTPSConnection('api.tavily.com')

    headers = {
        'Content-Type': 'application/json',
        'Authorization': f'Bearer {os.environ["TAVILY_API_KEY"]}'
    }

    body = json.dumps(request_data)

    conn.request('POST', '/extract', body, headers)

    response = conn.getresponse()
    data = response.read().decode('utf-8')

    conn.close()

    # Parse response
    result = json.loads(data)

    # Check for errors
    if 'error' in result:
        print(f'API Error: {result["error"]}', file=sys.stderr)
        sys.exit(1)

    # Check extraction results
    result_count = len(result['results'])
    failed_count = len(result.get('failed_results', []))

    if result_count == 0:
        print('Extraction failed\n', file=sys.stderr)
        print(f'Failed URLs: {result.get("failed_results", [])}', file=sys.stderr)
        sys.exit(1)

    print('========================================')
    print('Extraction Summary')
    print('========================================\n')
    print(f'Successful: {result_count}')
    print(f'Failed: {failed_count}\n')

    # Extract content
    content = result['results'][0]['raw_content']
    content_length = len(content)

    print('========================================')
    print('Extracted Content (Markdown)')
    print('========================================\n')
    print(f'Content length: {content_length} characters\n')
    print('First 1000 characters:\n')
    print(content[:1000])
    print('\n... (truncated, see full output below) ...\n')

    # Extract images
    images = result['results'][0].get('images', [])
    image_count = len(images)

    print('========================================')
    print('Extracted Images')
    print('========================================\n')
    print(f'Found {image_count} images:\n')

    for i, image_url in enumerate(images):
        print(f'[{i + 1}] {image_url}')
    print('')

    # Display metadata
    print('========================================')
    print('Request Metadata')
    print('========================================\n')
    print(f'Response Time: {result["response_time"]}s')
    print(f'Content Size: {content_length} characters')
    print(f'Images Extracted: {image_count}\n')

    # Save full content to file
    output_file = 'extracted_content.md'
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content)

    print('========================================')
    print('Full Content Saved')
    print('========================================\n')
    print(f'Saved to: {output_file}\n')
    print(f'View with: cat {output_file}')
    print('Or open with your editor\n')

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
    print('✓ Content extracted successfully')
    print(f'✓ Extracted {content_length} characters of clean markdown')
    print(f'✓ Found {image_count} images')
    print(f'✓ Saved to {output_file}')
    print(f'✓ Response time: {result["response_time"]}s\n')
    print('Try the exercises to:')
    print('  - Extract content from multiple URLs at once')
    print('  - Compare basic vs. advanced extraction depth')
    print('  - Feed extracted content to AI for summarization')
    print('  - Build a web clipper / read-later service\n')

except Exception as error:
    print(f'Error: {error}', file=sys.stderr)
    sys.exit(1)
