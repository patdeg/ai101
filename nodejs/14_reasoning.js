#!/usr/bin/env node
/*
Example 13: Reasoning with openai/gpt-oss-20b

Demonstrates:
- Reasoning models that show step-by-step thinking
- Three reasoning formats: raw, parsed, hidden
- Three reasoning effort levels: low, medium, high
- Cache optimization for cost savings

What you'll learn:
- How reasoning models differ from chat models
- When to use different reasoning formats
- How to optimize message order for cache hits
- Token usage and cost tracking

Note: Uses Node.js built-in modules only (https, process)
*/

const https = require('https');

// Check for API key
if (!process.env.DEMETERICS_API_KEY) {
  console.error('Error: DEMETERICS_API_KEY environment variable not set');
  console.error('Get your Managed LLM Key from: https://demeterics.com');
  process.exit(1);
}

// Function to call Demeterics Groq proxy with reasoning model
function callReasoning(messages, format = null, effort = 'medium') {
  const requestData = {
    model: 'openai/gpt-oss-20b',
    messages: messages,
    temperature: 0.6,
    max_completion_tokens: 1024,
    reasoning_effort: effort
  };

  if (format) {
    requestData.reasoning_format = format;
  }

  const data = JSON.stringify(requestData);

  const options = {
    hostname: 'api.demeterics.com',
    port: 443,
    path: '/groq/v1/chat/completions',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${process.env.DEMETERICS_API_KEY}`,
      'Content-Length': Buffer.byteLength(data)
    }
  };

  return new Promise((resolve, reject) => {
    const req = https.request(options, (res) => {
      let responseData = '';

      res.on('data', (chunk) => {
        responseData += chunk;
      });

      res.on('end', () => {
        try {
          resolve(JSON.parse(responseData));
        } catch (error) {
          reject(new Error('Failed to parse response'));
        }
      });
    });

    req.on('error', (error) => {
      reject(error);
    });

    req.write(data);
    req.end();
  });
}

async function main() {
  console.log('========================================');
  console.log('Reasoning with openai/gpt-oss-20b');
  console.log('========================================\n');

  const query = "How many 'r' letters are in the word 'strawberry'? Think through this step-by-step.";
  const messages = [{ role: 'user', content: query }];

  console.log(`Query: ${query}\n`);

  // Demo 1: Raw format (default)
  console.log('========================================');
  console.log('Demo 1: Raw Format (reasoning in <think> tags)');
  console.log('========================================\n');

  const rawResponse = await callReasoning(messages);
  console.log('Response (raw format):');
  console.log(rawResponse.choices[0].message.content);
  console.log('');

  console.log('Token Usage (Demo 1):');
  console.log(`  Prompt tokens: ${rawResponse.usage.prompt_tokens}`);
  console.log(`  Completion tokens: ${rawResponse.usage.completion_tokens}`);
  console.log(`  Total tokens: ${rawResponse.usage.total_tokens}\n`);

  // Demo 2: Parsed format
  console.log('========================================');
  console.log('Demo 2: Parsed Format (separate reasoning field)');
  console.log('========================================\n');

  const parsedResponse = await callReasoning(messages, 'parsed');

  if (parsedResponse.choices[0].message.reasoning) {
    console.log('Reasoning process:');
    console.log(parsedResponse.choices[0].message.reasoning);
    console.log('');
  }

  console.log('Final answer:');
  console.log(parsedResponse.choices[0].message.content);
  console.log('');

  // Demo 3: Hidden format
  console.log('========================================');
  console.log('Demo 3: Hidden Format (only final answer)');
  console.log('========================================\n');

  const hiddenResponse = await callReasoning(messages, 'hidden');
  console.log('Response (hidden format):');
  console.log(hiddenResponse.choices[0].message.content);
  console.log('');

  // Cost calculation
  console.log('========================================');
  console.log('Cost Analysis');
  console.log('========================================\n');

  const inputCostPer1M = 0.10;
  const cachedInputCostPer1M = 0.05;
  const outputCostPer1M = 0.50;

  const promptTokens = rawResponse.usage.prompt_tokens;
  const completionTokens = rawResponse.usage.completion_tokens;

  const inputCost = (promptTokens * inputCostPer1M) / 1000000;
  const outputCost = (completionTokens * outputCostPer1M) / 1000000;
  const totalCost = inputCost + outputCost;

  console.log('Pricing (openai/gpt-oss-20b):');
  console.log('  Input tokens: $0.10 per 1M');
  console.log('  Cached input tokens: $0.05 per 1M (50% discount!)');
  console.log('  Output tokens: $0.50 per 1M\n');

  console.log('This request:');
  console.log(`  Input cost: $${inputCost.toFixed(6)} (${promptTokens} tokens)`);
  console.log(`  Output cost: $${outputCost.toFixed(6)} (${completionTokens} tokens)`);
  console.log(`  Total cost: $${totalCost.toFixed(6)}\n`);

  // Cache optimization explanation
  console.log('========================================');
  console.log('Cache Optimization Strategy');
  console.log('========================================\n');

  console.log('What is cached input?');
  console.log('Groq caches parts of your prompt to speed up responses and reduce costs.');
  console.log('Cached tokens are 50% cheaper ($0.05/1M vs $0.10/1M for this model).\n');

  console.log('How to maximize cache hits:');
  console.log('Order messages from most constant to most variable:\n');
  console.log('  1. System prompts (always the same across requests)');
  console.log('  2. Context/documents (same during a conversation)');
  console.log('  3. User questions (changes with each request)\n');

  console.log('Example: agentresume.ai recruiter chatbot\n');
  console.log('Optimal message order:');
  console.log(`[
  {
    "role": "system",
    "content": "You are a recruiter assistant. Analyze resumes..."
  },                                                    ← Always cached
  {
    "role": "user",
    "content": "Candidate Resume:\\nJohn Doe\\nSkills: Python, Go..."
  },                                                    ← Cached per conversation
  {
    "role": "user",
    "content": "Does this candidate have Python experience?"
  }                                                     ← Not cached (changes each Q)
]\n`);

  console.log('Cache benefit calculation:');
  console.log('If 1000 tokens are always cached (system + resume):');
  console.log('  Normal cost: 1000 × $0.10 / 1M = $0.0001');
  console.log('  Cached cost: 1000 × $0.05 / 1M = $0.00005');
  console.log('  Savings: 50% per request!\n');

  console.log('========================================');
  console.log('Reasoning Effort Comparison');
  console.log('========================================\n');

  console.log('Reasoning effort levels:');
  console.log('  low    - Quick analysis, fewer reasoning tokens');
  console.log('  medium - Balanced reasoning (default)');
  console.log('  high   - Deep analysis, more reasoning tokens\n');
  console.log("Use 'low' for simple questions, 'high' for complex reasoning tasks.\n");

  console.log('========================================');
  console.log('Summary');
  console.log('========================================\n');
  console.log('✓ Demonstrated three reasoning formats');
  console.log('✓ Showed token usage and costs');
  console.log('✓ Explained cache optimization strategy');
  console.log('✓ Model excels at step-by-step logical reasoning\n');
}

main().catch(error => {
  console.error('Error:', error.message);
  process.exit(1);
});
