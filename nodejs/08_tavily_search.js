#!/usr/bin/env node
/**
 * Example 08: Tavily Search API - Web Search with AI-Powered Answers
 *
 * What is Tavily?
 * ---------------
 * Tavily is a search API designed for AI agents and applications:
 * - Web search with AI-generated answer synthesis
 * - Topic-specific search (general, news, finance)
 * - Image search with AI descriptions
 * - Domain filtering (include/exclude)
 * - Time-restricted searches
 * - Clean, structured data (no ads)
 *
 * Free tier: 1,000 searches/month
 * Get your API key at: https://tavily.com
 *
 * This example:
 * ------------
 * - Searches for "traditional French bread recipes"
 * - Requests AI-generated answer from multiple sources
 * - Returns 5 clean, relevant results
 * - No external dependencies (uses Node.js built-in https module)
 */

const https = require('https');

// Check for API key
if (!process.env.TAVILY_API_KEY) {
  console.error('Error: TAVILY_API_KEY environment variable not set\n');
  console.error('Get your free API key:');
  console.error('  1. Visit https://tavily.com');
  console.error('  2. Sign up (1,000 free searches/month)');
  console.error('  3. Copy your API key (starts with "tvly-")');
  console.error('  4. Set: export TAVILY_API_KEY="tvly-your-key-here"\n');
  process.exit(1);
}

/**
 * Tavily Search API Request Parameters
 *
 * REQUIRED:
 * - query: Search query string
 * - api_key: Your Tavily API key
 *
 * OPTIONAL:
 * - search_depth: "basic" (default) or "advanced"
 * - topic: "general" (default), "news", or "finance"
 * - days: For news - days back to search (default: 7)
 * - max_results: 1-20 results (default: 5)
 * - include_answer: true/false/"basic"/"advanced"
 * - include_images: Include relevant images
 * - include_image_descriptions: AI descriptions of images
 * - include_domains: Array of domains to include (max 300)
 * - exclude_domains: Array of domains to exclude (max 150)
 */

const requestData = JSON.stringify({
  query: 'traditional French bread recipes',
  search_depth: 'basic',
  include_answer: true,
  max_results: 5,
  include_images: false
});

const options = {
  hostname: 'api.tavily.com',
  port: 443,
  path: '/search',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.TAVILY_API_KEY}`,
    'Content-Length': Buffer.byteLength(requestData)
  }
};

console.log('===========================================');
console.log('Tavily Search API - French Bread Recipes');
console.log('===========================================\n');
console.log('Query: traditional French bread recipes');
console.log('Options: include_answer=true, max_results=5\n');
console.log('Sending request to Tavily...\n');

const req = https.request(options, (res) => {
  let data = '';

  res.on('data', (chunk) => {
    data += chunk;
  });

  res.on('end', () => {
    try {
      const response = JSON.parse(data);

      // Check for errors
      if (response.error) {
        console.error('API Error:', response.error);
        process.exit(1);
      }

      // Display AI-generated answer
      console.log('========================================');
      console.log('AI-Generated Answer (Answer Synthesis)');
      console.log('========================================\n');
      console.log(response.answer || 'No answer provided');
      console.log('');

      // Display search results
      console.log('========================================');
      console.log('Search Results (Top 5)');
      console.log('========================================\n');
      console.log(`Found ${response.results.length} results:\n`);

      response.results.forEach((result, index) => {
        console.log(`[${index + 1}] ${result.title}`);
        console.log(`URL: ${result.url}`);
        console.log(`Relevance Score: ${result.score}`);
        console.log(`Summary: ${result.content}`);
        console.log('');
      });

      // Display metadata
      console.log('========================================');
      console.log('Request Metadata');
      console.log('========================================\n');
      console.log(`Response Time: ${response.response_time}s`);
      console.log(`Request ID: ${response.request_id || 'N/A'}`);
      console.log('');

      // Display raw JSON
      console.log('========================================');
      console.log('Raw API Response (formatted)');
      console.log('========================================\n');
      console.log(JSON.stringify(response, null, 2));
      console.log('');

      // Summary
      console.log('===========================================');
      console.log('Summary');
      console.log('===========================================\n');
      console.log('✓ Tavily Search completed successfully');
      console.log('✓ AI Answer synthesized from multiple sources');
      console.log(`✓ Returned ${response.results.length} relevant results`);
      console.log(`✓ Response time: ${response.response_time}s\n`);
      console.log('Try the exercises to explore:');
      console.log('  - News search with time restrictions');
      console.log('  - Finance search with domain filtering');
      console.log('  - Image search with descriptions');
      console.log('  - Building executive summaries with AI');
      console.log('  - Sending results via ntfy.sh notifications\n');

    } catch (error) {
      console.error('Error parsing response:', error.message);
      console.error('Raw response:', data);
      process.exit(1);
    }
  });
});

req.on('error', (error) => {
  console.error('Request error:', error.message);
  process.exit(1);
});

req.write(requestData);
req.end();
