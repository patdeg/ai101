#!/usr/bin/env node
/**
 * Example 12: Code Execution with openai/gpt-oss-20b
 *
 * What this demonstrates:
 *  - Using a model that can execute Python to solve tasks
 *  - Inspecting optional reasoning and executed tool details
 *
 * What you'll learn:
 *  - How to design prompts that trigger code execution
 *  - How to view executed code and outputs (if provided)
 *
 * Prerequisites:
  *  - GROQ_API_KEY environment variable set
  *  - Node.js v14+
  *
  * Exercises: exercises/12_code_execution.md
  */

const https = require('https');

if (!process.env.GROQ_API_KEY) {
  console.error('Error: GROQ_API_KEY not set');
  console.error('Run: export GROQ_API_KEY="gsk_your_api_key_here"');
  process.exit(1);
}

// Step 1: Build payload
const payload = JSON.stringify({
  model: 'openai/gpt-oss-20b',
  messages: [
    { role: 'system', content: 'You can execute Python to verify results. If code raises an exception, show it and provide a corrected version.' },
    { role: 'user', content: '1) Compute the 2000th prime number using Python. 2) Execute Python: print(1/0) to demonstrate the exception, then show how to catch it without crashing.' }
  ],
  tools: [ { type: 'code_interpreter' } ],
  tool_choice: 'required',
  temperature: 0.2,
  max_tokens: 900
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
console.log('Code Execution: openai/gpt-oss-20b');
console.log('========================================\n');

// Step 3: Send request
const req = https.request(options, (res) => {
  let body = '';
  res.on('data', (chunk) => (body += chunk));
  res.on('end', () => {
    try {
      const json = JSON.parse(body);
      const msg = json?.choices?.[0]?.message || {};

      console.log('Final Answer');
      console.log('----------------------------------------');
      console.log(msg.content || '(no content)');
      console.log('');

      console.log('Reasoning & Tool Calls (if any)');
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
      // 1) Switch to 'openai/gpt-oss-120b' (if available) and compare.
      // 2) Ask to run code that raises exceptions (e.g., invalid JSON) and show try/except.
      // 3) Ask for the exact code executed and outputs.

      console.log('Exercises: exercises/12_code_execution.md');

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
