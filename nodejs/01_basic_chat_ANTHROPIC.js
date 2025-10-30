/**
 * ============================================================================
 * Example 1: Basic Chat with Anthropic (Claude) - Your First Claude API Call
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to make an Anthropic Claude API call using Node.js
 * - No external dependencies needed!
 * - Understanding Anthropic's unique API format
 * - Working with Claude models
 *
 * WHAT YOU'LL LEARN:
 * - Building HTTPS requests for Anthropic's API
 * - Anthropic-specific headers (x-api-key, anthropic-version)
 * - Claude's message format
 * - Handling Claude responses
 *
 * PREREQUISITES:
 * - Node.js installed (v14 or higher)
 * - ANTHROPIC_API_KEY environment variable set
 *   Run: export ANTHROPIC_API_KEY="sk-ant-..."
 *
 * EXPECTED OUTPUT:
 * - Full JSON response from Claude
 * - The AI's answer to your question
 * - Token usage information
 *
 * RUN THIS:
 * node 01_basic_chat_ANTHROPIC.js
 * ============================================================================
 */

// Step 1: Import the built-in HTTPS module
const https = require('https');

// Step 2: Prepare the Anthropic request payload
// Note: Anthropic has a slightly different format than OpenAI
const data = JSON.stringify({
  model: "claude-haiku-4-5",                    // Claude's fast, efficient model
  max_tokens: 100,                              // REQUIRED for Anthropic (not optional!)
  temperature: 0.7,                             // Creativity level
  messages: [
    {
      role: "user",
      content: "What is the capital of Switzerland?"
    }
  ]
});

// Step 3: Configure the Anthropic HTTP request
// Anthropic uses different headers than OpenAI
const options = {
  hostname: 'api.anthropic.com',               // Anthropic's API server
  port: 443,
  path: '/v1/messages',                        // Anthropic's messages endpoint
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'x-api-key': process.env.ANTHROPIC_API_KEY,        // Anthropic uses x-api-key header
    'anthropic-version': '2023-06-01',                 // Required version header
    'Content-Length': Buffer.byteLength(data)
  }
};

// Step 4: Create and send the request
const req = https.request(options, (res) => {
  let responseData = '';

  // Accumulate response chunks
  res.on('data', (chunk) => {
    responseData += chunk;
  });

  // Process complete response
  res.on('end', () => {
    try {
      const parsed = JSON.parse(responseData);

      // Display full response
      console.log('Full Anthropic Response:');
      console.log(JSON.stringify(parsed, null, 2));

      // Extract the AI's answer
      // Note: Anthropic's response structure is different
      if (parsed.content && parsed.content[0]) {
        console.log('\nClaude\'s Answer:');
        console.log(parsed.content[0].text);
      }

      // Show token usage
      if (parsed.usage) {
        console.log('\nToken Usage:');
        console.log(`Input: ${parsed.usage.input_tokens}`);
        console.log(`Output: ${parsed.usage.output_tokens}`);
      }

      // Check for errors
      if (parsed.error) {
        console.error('API Error:', parsed.error.message);
      }

    } catch (error) {
      console.error('Error parsing response:', error.message);
      console.log('Raw response:', responseData);
    }
  });
});

// Handle request errors
req.on('error', (error) => {
  console.error('Request failed:', error.message);

  if (error.message.includes('ENOTFOUND')) {
    console.log('Check your internet connection');
  }
  if (!process.env.ANTHROPIC_API_KEY) {
    console.log('Make sure ANTHROPIC_API_KEY is set');
  }
});

// Send the request
req.write(data);
req.end();

// Anthropic API Notes:
// - max_tokens is REQUIRED (not optional like OpenAI)
// - Uses x-api-key header instead of Authorization Bearer
// - Requires anthropic-version header
// - Response format differs: content array instead of choices
// - Models: claude-haiku-4-5 (fast), claude-sonnet (balanced), claude-opus (powerful)
// - Excellent for long context and nuanced responses