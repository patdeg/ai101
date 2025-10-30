/**
 * ============================================================================
 * Example 1: Basic Chat with OpenAI - Your First OpenAI API Call
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to make an OpenAI API call using only Node.js built-in modules
 * - No external dependencies needed!
 * - Understanding the OpenAI API format
 * - Parsing and displaying AI responses
 *
 * WHAT YOU'LL LEARN:
 * - Building HTTPS POST requests for OpenAI
 * - Working with OpenAI's JSON payload format
 * - Handling streaming responses
 * - Using OpenAI API keys
 *
 * PREREQUISITES:
 * - Node.js installed (v14 or higher)
 * - OPENAI_API_KEY environment variable set
 *   Run: export OPENAI_API_KEY="sk-..."
 *
 * EXPECTED OUTPUT:
 * - Full JSON response from OpenAI
 * - The AI's answer to your question
 * - Token usage statistics
 *
 * RUN THIS:
 * node 01_basic_chat_OPENAI.js
 * ============================================================================
 */

// Step 1: Import the built-in HTTPS module
const https = require('https');

// Step 2: Prepare the OpenAI request payload
const data = JSON.stringify({
  model: "gpt-5-nano",                          // OpenAI's latest efficient model
  messages: [
    {
      role: "user",                              // Who is speaking
      content: "What is the capital of Switzerland?"
    }
  ],
  temperature: 0.7,   // Creativity level (0.0 = focused, 2.0 = creative)
  max_tokens: 100     // Maximum response length
});

// Step 3: Configure the OpenAI HTTP request
const options = {
  hostname: 'api.openai.com',                  // OpenAI's API server
  port: 443,                                    // HTTPS port
  path: '/v1/chat/completions',               // OpenAI chat endpoint
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.OPENAI_API_KEY}`,  // OpenAI API key
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
      console.log('Full OpenAI Response:');
      console.log(JSON.stringify(parsed, null, 2));

      // Extract the AI's answer
      if (parsed.choices && parsed.choices[0]) {
        console.log('\nAI Answer:');
        console.log(parsed.choices[0].message.content);
      }

      // Show token usage
      if (parsed.usage) {
        console.log('\nToken Usage:');
        console.log(`Prompt: ${parsed.usage.prompt_tokens}`);
        console.log(`Response: ${parsed.usage.completion_tokens}`);
        console.log(`Total: ${parsed.usage.total_tokens}`);
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
  if (!process.env.OPENAI_API_KEY) {
    console.log('Make sure OPENAI_API_KEY is set');
  }
});

// Send the request
req.write(data);
req.end();

// OpenAI API Notes:
// - Uses standard OpenAI chat completions format
// - Supports system messages for behavior control
// - Includes advanced features like function calling
// - Model options: gpt-5-nano (fast), gpt-4o (powerful)
// - Rate limits apply based on your tier