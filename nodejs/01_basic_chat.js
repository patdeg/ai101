/**
 * ============================================================================
 * Example 1: Basic Chat - Your First AI API Call
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to make an AI API call using only Node.js built-in modules
 * - No external dependencies (axios, fetch, etc.) needed!
 * - Understanding the complete HTTP request/response flow
 * - Parsing and displaying AI responses
 *
 * WHAT YOU'LL LEARN:
 * - Building HTTPS POST requests from scratch
 * - Working with JSON payloads
 * - Handling streaming responses (data arrives in chunks)
 * - Event-driven programming in Node.js
 * - Reading environment variables for API keys
 *
 * PREREQUISITES:
 * - Node.js installed (v14 or higher)
 * - GROQ_API_KEY environment variable set
 *   Run: export GROQ_API_KEY="your-key-here"
 *
 * EXPECTED OUTPUT:
 * - Full JSON response from the API
 * - The AI's answer to your question
 * - Token usage statistics (prompt, completion, total)
 *
 * RUN THIS:
 * node 01_basic_chat.js
 * ============================================================================
 */

// Step 1: Import the built-in HTTPS module (no npm install needed!)
const https = require('https');

// Step 2: Prepare the request payload (the data we'll send to the AI)
// JSON.stringify() converts JavaScript object to JSON string format
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",  // Which AI model to use
  messages: [
    {
      role: "user",                                      // Who is speaking (user, system, or assistant)
      content: "What is the capital of Switzerland?"     // The actual question/prompt
    }
  ],
  temperature: 0.7,   // Creativity level (0.0 = focused, 2.0 = creative)
  max_tokens: 100     // Maximum length of response
});

// Step 3: Configure the HTTP request options
// This tells Node.js WHERE and HOW to send the request
const options = {
  hostname: 'api.groq.com',                    // The API server
  port: 443,                                    // HTTPS port (443 = secure, 80 = unsecure)
  path: '/openai/v1/chat/completions',         // The specific API endpoint
  method: 'POST',                               // HTTP method (POST = sending data)
  headers: {
    'Content-Type': 'application/json',                        // We're sending JSON
    'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,    // Your API key from environment
    'Content-Length': Buffer.byteLength(data)                  // Size of data in bytes (required for POST)
  }
};

// Step 4: Create and send the HTTPS request
// https.request() creates the request but doesn't send it yet
// The callback function (res) => {...} runs when we START receiving a response
const req = https.request(options, (res) => {
  // Step 5: Initialize variable to accumulate response data
  let responseData = '';

  // Step 6: Set up event handler for incoming data chunks
  // Data arrives in pieces (chunks), not all at once
  // This event fires multiple times as data streams in
  res.on('data', (chunk) => {
    responseData += chunk;  // Append each chunk to build complete response
  });

  // Step 7: Set up event handler for when ALL data has been received
  // This event fires once, after the last chunk arrives
  res.on('end', () => {
    try {
      // Step 8: Parse the JSON string into a JavaScript object
      const parsed = JSON.parse(responseData);

      // Step 9: Display the full API response (for learning purposes)
      console.log('Full Response:');
      console.log(JSON.stringify(parsed, null, 2));  // null, 2 = pretty print with 2-space indent

      // Step 10: Extract and display just the AI's answer
      console.log('\nAI Answer:');
      console.log(parsed.choices[0].message.content);

      // Step 11: Show token usage statistics
      console.log('\nToken Usage:');
      console.log(`Prompt: ${parsed.usage.prompt_tokens}`);
      console.log(`Response: ${parsed.usage.completion_tokens}`);
      console.log(`Total: ${parsed.usage.total_tokens}`);

    } catch (error) {
      // Step 12: Handle JSON parsing errors
      console.error('Error parsing response:', error.message);
      console.log('Raw response:', responseData);
    }
  });
});

// Step 13: Set up error handler for request failures
// This catches network errors, DNS failures, etc.
req.on('error', (error) => {
  console.error('Request failed:', error.message);

  if (error.message.includes('ENOTFOUND')) {
    console.log('Check your internet connection');
  }
});

// Step 14: Write the payload data to the request body
req.write(data);

// Step 15: Finish the request and send it to the server
// Without this, the request will never be sent!
req.end();

// Explanation of Node.js concepts:
//
// require('https')
//   Imports the built-in HTTPS module (no npm install needed)
//
// JSON.stringify()
//   Converts JavaScript object to JSON string
//   Second parameter (null, 2) = pretty print with 2 spaces
//
// process.env.GROQ_API_KEY
//   Reads environment variable (same as $GROQ_API_KEY in bash)
//
// Buffer.byteLength(data)
//   Calculates exact size of data in bytes
//   Required for the Content-Length header
//
// https.request()
//   Creates an HTTPS request object
//   Returns immediately (doesn't wait for response)
//
// res.on('data', ...)
//   Event fired when data arrives
//   Data comes in "chunks" (pieces), not all at once
//
// res.on('end', ...)
//   Event fired when ALL data has arrived
//   Now we can process the complete response
//
// req.write(data)
//   Sends our JSON data to the server
//
// req.end()
//   Tells server we're done sending data
//   Without this, request will hang forever!
//
// Event-driven programming:
//   Node.js doesn't wait for responses
//   Instead, you provide callback functions
//   These run when events happen (data arrives, request ends, etc.)
