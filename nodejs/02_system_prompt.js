/**
 * ============================================================================
 * Example 2: System Prompts - Controlling AI Behavior
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to use system prompts to control AI behavior and personality
 * - The difference between system messages and user messages
 * - How message roles affect AI responses
 * - Using temperature to control output consistency
 *
 * WHAT YOU'LL LEARN:
 * - Creating multi-message conversations
 * - System prompt best practices
 * - The order and importance of message roles
 * - How to constrain AI output format and style
 *
 * PREREQUISITES:
 * - Completed 01_basic_chat.js
 * - GROQ_API_KEY environment variable set
 *
 * EXPECTED OUTPUT:
 * - The system prompt you defined
 * - The user's question
 * - AI's response following the system rules (exactly 2 sentences)
 * - Token usage count
 *
 * RUN THIS:
 * node 02_system_prompt.js
 * ============================================================================
 */

// Step 1: Import the HTTPS module
const https = require('https');

// Step 2: Create the request payload with BOTH system and user messages
// Notice the messages array now has TWO objects, each with a different role
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",
  messages: [
    {
      role: "system",  // SYSTEM message = instructions/rules for the AI
      content: "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
    },
    {
      role: "user",    // USER message = the actual question from the user
      content: "Why did the Roman Empire fall?"
    }
  ],
  temperature: 0.5,   // Lower temperature = more consistent responses
  max_tokens: 150
});

// Step 3: Configure the HTTPS request options (same as example 1)
const options = {
  hostname: 'api.groq.com',
  port: 443,
  path: '/openai/v1/chat/completions',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,
    'Content-Length': Buffer.byteLength(data)
  }
};

// Step 4: Create the HTTPS request
const req = https.request(options, (res) => {
  // Step 5: Accumulate response data
  let responseData = '';

  // Step 6: Handle incoming data chunks
  res.on('data', (chunk) => {
    responseData += chunk;
  });

  // Step 7: Process the complete response
  res.on('end', () => {
    // Step 8: Parse the JSON response
    const parsed = JSON.parse(responseData);

    // Step 9: Display the system prompt that was used
    console.log('System Prompt Used:');
    console.log(JSON.parse(data).messages[0].content);

    // Step 10: Display the user's question
    console.log('\nUser Question:');
    console.log(JSON.parse(data).messages[1].content);

    // Step 11: Display the AI's response (which should follow the system rules)
    console.log('\nAI Response (following system rules):');
    console.log(parsed.choices[0].message.content);

    // Step 12: Show token usage
    console.log('\nToken Usage:', parsed.usage.total_tokens);
  });
});

// Step 13: Handle any request errors
req.on('error', (error) => {
  console.error('Error:', error.message);
});

// Step 14: Send the request data
req.write(data);

// Step 15: Complete and send the request
req.end();

// Key concepts:
//
// Message order matters:
//   1. System message (the "rules")
//   2. User message (the "question")
//   3. Assistant message (previous AI response - for conversation)
//   4. More user/assistant pairs for multi-turn chat
//
// System prompt examples:
//   "You are a Python expert. Provide code examples."
//   "You are a friendly teacher. Explain like I'm 10."
//   "Always respond in JSON format."
//   "Be concise. Maximum 3 sentences."
//
// Temperature comparison:
//   0.0  = Same answer every time (deterministic)
//   0.5  = Fairly consistent (good for facts)
//   0.7  = Balanced creativity
//   1.0  = Default
//   1.5+ = Very creative/random
//
// Try modifying:
//   Change temperature to 0.0 and run twice
//   Change it to 1.8 and run twice
//   See the difference!
