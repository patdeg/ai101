// Example 1: Basic Chat
// This sends a simple question to the AI model using Node.js built-in https module.
// No external dependencies needed!

const https = require('https');

// The request data (payload) that we'll send to the API
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",
  messages: [
    {
      role: "user",
      content: "What is the capital of Switzerland?"
    }
  ],
  temperature: 0.7,
  max_tokens: 100
});

// Configure the HTTP request
const options = {
  hostname: 'api.groq.com',
  port: 443,                                    // HTTPS port
  path: '/openai/v1/chat/completions',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,
    'Content-Length': Buffer.byteLength(data)  // Required for POST
  }
};

// Make the request
const req = https.request(options, (res) => {
  let responseData = '';

  // Receive data in chunks
  res.on('data', (chunk) => {
    responseData += chunk;
  });

  // When all data received
  res.on('end', () => {
    try {
      const parsed = JSON.parse(responseData);

      // Pretty print the full response
      console.log('Full Response:');
      console.log(JSON.stringify(parsed, null, 2));

      // Extract just the answer
      console.log('\nAI Answer:');
      console.log(parsed.choices[0].message.content);

      // Show token usage
      console.log('\nToken Usage:');
      console.log(`Prompt: ${parsed.usage.prompt_tokens}`);
      console.log(`Response: ${parsed.usage.completion_tokens}`);
      console.log(`Total: ${parsed.usage.total_tokens}`);

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
});

// Send the request with our data
req.write(data);
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
