#!/usr/bin/env node
/**
 * Example 11: Web Search with OpenRouter's :online suffix
 *
 * What this demonstrates:
 *  - Calling a web-search-augmented model (append :online to any model slug)
 *  - OpenAI-compatible chat API usage with a built-in search plugin
 *  - Inspecting optional reasoning / executed tools
 *
 * What you'll learn:
 *  - Structuring chat requests in Node.js (built-in https)
 *  - Parsing JSON and safely accessing optional fields
 *
 * Prerequisites:
 *  - OPENROUTER_API_KEY environment variable set
 *  - Node.js v14+
 *
 * Expected output:
  *  - Final answer content
  *  - Optional reasoning and executed tool info (if search was used)
  *
  * Exercises: exercises/11_web_search.md
  */

const https = require('https');

if (!process.env.OPENROUTER_API_KEY) {
  console.error('Error: OPENROUTER_API_KEY not set');
  console.error('Run: export OPENROUTER_API_KEY="your_api_key_here"');
  process.exit(1);
}

// Step 1: Build payload
// The ":online" suffix turns on OpenRouter's web-search plugin (default
// engine Exa, ~$0.005 per request for up to 10 results) for any model.
const payload = JSON.stringify({
  model: 'google/gemini-3.8-flash:online',
  messages: [
    { role: 'system', content: 'You are a helpful research assistant. Provide concise answers with source links. Use search when needed.' },
    { role: 'user', content: 'What were the top 3 AI model releases last week? Include links and 1-sentence summaries.' }
  ],
  temperature: 0.3,
  max_tokens: 1200
});

// Step 2: Configure HTTPS request
const options = {
  hostname: 'openrouter.ai',
  path: '/api/v1/chat/completions',
  method: 'POST',
  headers: {
    'Authorization': `Bearer ${process.env.OPENROUTER_API_KEY}`,
    'Content-Type': 'application/json',
    'Content-Length': Buffer.byteLength(payload)
  }
};

console.log('========================================');
console.log('Web Search: google/gemini-3.8-flash:online');
console.log('========================================\n');

// Step 3: Send request
 const req = https.request(options, (res) => {
  let body = '';
  res.on('data', (chunk) => (body += chunk));
  res.on('end', () => {
    try {
      const json = JSON.parse(body);

      // Raw (optional):
      // console.log(JSON.stringify(json, null, 2));

      const msg = json?.choices?.[0]?.message || {};

      console.log('Final Answer');
      console.log('----------------------------------------');
      console.log(msg.content || '(no content)');
      console.log('');

      console.log('Optional Reasoning');
      console.log('----------------------------------------');
      console.log(msg.reasoning || '(no reasoning)');
      console.log('');

      console.log('Executed Tools (if any)');
      console.log('----------------------------------------');
      if (msg.executed_tools) {
        console.log(JSON.stringify(msg.executed_tools, null, 2));
      } else {
        console.log('(no executed tool info)');
      }
      console.log('');

      // Exercises:
      // 1) Try a different base model with :online, e.g. 'z-ai/glm-5.2:online'.
      // 2) Ask a boolean-style query like: '(OpenAI OR Meta) AND release notes last 7 days'.
      // 3) OpenRouter's web plugin supports a "plugins" array with max_results / search_prompt
      //    overrides - see https://openrouter.ai/docs/guides/features/web-search for the schema.

      console.log('Exercises: exercises/11_web_search.md');

    } catch (err) {
      console.error('Parse error:', err.message);
      console.error('Raw response:', body);
      process.exit(1);
    }
  });
});

req.on('error', (err) => {
  console.error('Request error:', err.message);
  process.exit(1);
});

req.write(payload);
req.end();
