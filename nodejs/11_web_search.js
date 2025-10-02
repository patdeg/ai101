#!/usr/bin/env node
/**
 * Example 11: Web Search with groq/compound-mini
 *
 * What this demonstrates:
 *  - Calling a web-search-enabled model (groq/compound-mini)
 *  - OpenAI-compatible chat API usage with built-in tools
 *  - Inspecting optional reasoning / executed tools
 *
 * What you'll learn:
 *  - Structuring chat requests in Node.js (built-in https)
 *  - Parsing JSON and safely accessing optional fields
 *
 * Prerequisites:
 *  - GROQ_API_KEY environment variable set
 *  - Node.js v14+
 *
 * Expected output:
  *  - Final answer content
  *  - Optional reasoning and executed tool info (if search was used)
  *
  * Exercises: exercises/11_web_search.md
  */

const https = require('https');

if (!process.env.GROQ_API_KEY) {
  console.error('Error: GROQ_API_KEY not set');
  console.error('Run: export GROQ_API_KEY="gsk_your_api_key_here"');
  process.exit(1);
}

// Step 1: Build payload
const payload = JSON.stringify({
  model: 'groq/compound-mini',
  messages: [
    { role: 'system', content: 'You are a helpful research assistant. Provide concise answers with source links. Use search when needed.' },
    { role: 'user', content: 'What were the top 3 AI model releases last week? Include links and 1-sentence summaries.' }
  ],
  temperature: 0.3,
  max_tokens: 600
});

// Step 2: Configure HTTPS request
const options = {
  hostname: 'api.groq.com',
  path: '/openai/v1/chat/completions',
  method: 'POST',
  headers: {
    'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,
    'Content-Type': 'application/json',
    'Content-Length': Buffer.byteLength(payload)
  }
};

console.log('========================================');
console.log('Web Search: groq/compound-mini');
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
      // 1) Change model to 'groq/compound' for a larger model.
      // 2) Ask a boolean-style query like: '(OpenAI OR Meta) AND release notes last 7 days'.
      // 3) For include/exclude domains or country hints, consult docs for supported fields
      //    in your deployment (often a web-search config object). Only include fields
      //    confirmed by the docs to avoid request errors.

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
