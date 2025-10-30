/**
 * ============================================================================
 * Example 1: Basic Chat with Demeterics - Instrumented AI Proxy
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to use Demeterics observability proxy
 * - Automatic instrumentation of AI API calls
 * - Analytics and monitoring for production AI
 * - Using Groq models through Demeterics proxy
 *
 * WHAT YOU'LL LEARN:
 * - Routing API calls through an observability layer
 * - Getting analytics without code changes
 * - Understanding proxy patterns in AI deployments
 * - Production monitoring best practices
 *
 * DEMETERICS PROVIDES:
 * - Token usage tracking across all providers
 * - Performance metrics (latency, queue times)
 * - Cost analysis and optimization
 * - Error tracking and debugging
 * - Compliance audit trails
 * - A/B testing capabilities
 *
 * PREREQUISITES:
 * - Node.js installed (v14 or higher)
 * - DEMETERICS_API_KEY environment variable set
 *   Run: export DEMETERICS_API_KEY="..."
 *
 * EXPECTED OUTPUT:
 * - Full JSON response (same as direct Groq)
 * - The AI's answer to your question
 * - Token usage (tracked by Demeterics)
 * - Check Demeterics dashboard for analytics!
 *
 * RUN THIS:
 * node 01_basic_chat_DEMETERICS.js
 * ============================================================================
 */

// Step 1: Import the built-in HTTPS module
const https = require('https');

// Step 2: Prepare the request payload
// Same format as Groq (OpenAI-compatible)
const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",  // Groq model via proxy
  messages: [
    {
      role: "user",
      content: "What is the capital of Switzerland?"
    }
  ],
  temperature: 0.7,   // Creativity level
  max_tokens: 100     // Maximum response length
});

// Step 3: Configure the Demeterics proxy request
// Note: Different hostname, same API path as Groq
const options = {
  hostname: 'demeterics.uc.r.appspot.com',     // Demeterics proxy server
  port: 443,
  path: '/api/groq/v1/chat/completions',       // Groq endpoint via proxy
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${process.env.DEMETERICS_API_KEY}`,  // Demeterics key
    'Content-Length': Buffer.byteLength(data)
  }
};

// Optional: If you have both Demeterics AND Groq keys, you can use combined format:
// 'Authorization': `Bearer ${process.env.DEMETERICS_API_KEY};${process.env.GROQ_API_KEY}`
// This enables advanced features like fallback and provider switching

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
      console.log('Full Response (via Demeterics):');
      console.log(JSON.stringify(parsed, null, 2));

      // Extract the AI's answer
      if (parsed.choices && parsed.choices[0]) {
        console.log('\nAI Answer:');
        console.log(parsed.choices[0].message.content);
      }

      // Show token usage (tracked by Demeterics)
      if (parsed.usage) {
        console.log('\nToken Usage (monitored):');
        console.log(`Prompt: ${parsed.usage.prompt_tokens}`);
        console.log(`Completion: ${parsed.usage.completion_tokens}`);
        console.log(`Total: ${parsed.usage.total_tokens}`);
        console.log('\n📊 Check Demeterics dashboard for detailed analytics!');
      }

      // Demeterics may add metadata
      if (parsed.demeterics_metadata) {
        console.log('\nDemeterics Metadata:');
        console.log(JSON.stringify(parsed.demeterics_metadata, null, 2));
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
  if (!process.env.DEMETERICS_API_KEY) {
    console.log('Make sure DEMETERICS_API_KEY is set');
  }
});

// Send the request
req.write(data);
req.end();

// Demeterics Benefits:
// - Zero code changes for observability
// - Works with existing API integrations
// - Universal analytics across all providers
// - Production debugging and monitoring
// - Cost optimization insights
// - Compliance and audit trails
//
// Architecture:
// Your App → Demeterics Proxy → AI Provider (Groq/OpenAI/etc)
//              ↓
//         Analytics DB
//
// The proxy is transparent - same API format, same responses
// But now with complete observability!