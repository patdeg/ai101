/**
 * ============================================================================
 * Example 1: Basic Chat with SambaNova - Enterprise Open Models
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to make a SambaNova API call using Node.js
 * - SambaNova's OpenAI-compatible format
 * - Using open-source models in enterprise settings
 * - No external dependencies needed!
 *
 * WHAT YOU'LL LEARN:
 * - Building HTTPS requests for SambaNova
 * - Working with Llama models via SambaNova
 * - OpenAI-compatible API format
 * - Enterprise-grade open model deployment
 *
 * PREREQUISITES:
 * - Node.js installed (v14 or higher)
 * - SAMBANOVA_API_KEY environment variable set
 *   Run: export SAMBANOVA_API_KEY="..."
 *
 * EXPECTED OUTPUT:
 * - Full JSON response from SambaNova
 * - The AI's answer using Llama model
 * - Token usage statistics
 *
 * RUN THIS:
 * node 01_basic_chat_SAMBA.js
 * ============================================================================
 */

// Step 1: Import the built-in HTTPS module
const https = require('https');

// Step 2: Prepare the SambaNova request payload
// SambaNova uses OpenAI-compatible format
const data = JSON.stringify({
  model: "Meta-Llama-3.1-8B-Instruct",         // Open-source Llama model
  messages: [
    {
      role: "user",
      content: "What is the capital of Switzerland?"
    }
  ],
  temperature: 0.7,   // Creativity level
  max_tokens: 100     // Maximum response length
});

// Step 3: Configure the SambaNova HTTP request
// SambaNova uses OpenAI-compatible endpoints
const options = {
  hostname: 'api.sambanova.ai',                // SambaNova's API server
  port: 443,
  path: '/v1/chat/completions',                // OpenAI-compatible endpoint
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.SAMBANOVA_API_KEY}`,  // Standard Bearer auth
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
      console.log('Full SambaNova Response:');
      console.log(JSON.stringify(parsed, null, 2));

      // Extract the AI's answer (OpenAI-compatible format)
      if (parsed.choices && parsed.choices[0]) {
        console.log('\nLlama Answer:');
        console.log(parsed.choices[0].message.content);
      }

      // Show token usage
      if (parsed.usage) {
        console.log('\nToken Usage:');
        console.log(`Prompt: ${parsed.usage.prompt_tokens}`);
        console.log(`Completion: ${parsed.usage.completion_tokens}`);
        console.log(`Total: ${parsed.usage.total_tokens}`);
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
  if (!process.env.SAMBANOVA_API_KEY) {
    console.log('Make sure SAMBANOVA_API_KEY is set');
  }
});

// Send the request
req.write(data);
req.end();

// SambaNova API Notes:
// - OpenAI-compatible format (easy migration)
// - Focuses on open-source models (Llama, Mistral, etc.)
// - Enterprise-grade infrastructure
// - High-performance inference optimized hardware
// - Models: Llama 3.1 variants (7B, 70B, 405B)
// - Good for privacy-conscious deployments