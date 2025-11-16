#!/usr/bin/env node
/**
 * Example 10: Tool Use (Function Calling) - AI Agents with Groq + Tavily
 *
 * Demonstrates:
 * - Groq function calling with llama-4-scout
 * - Tavily Search and Extract as tools
 * - Multi-step agent workflow
 *
 * Note: This uses built-in Node.js modules only (no external dependencies)
 * For production, consider using the official Groq SDK: npm install groq-sdk
 */

const https = require('https');

// Check for API keys
if (!process.env.DEMETERICS_API_KEY || !process.env.TAVILY_API_KEY) {
  console.error('Error: Both DEMETERICS_API_KEY and TAVILY_API_KEY must be set');
  console.error('Get your Demeterics Managed LLM Key: https://demeterics.com');
  console.error('Get a Tavily key: https://tavily.com');
  process.exit(1);
}

// Tool functions
async function tavilySearch(query) {
  console.log(`[Tool] Executing tavily_search with query: ${query}`);

  return new Promise((resolve, reject) => {
    const data = JSON.stringify({
      query,
      max_results: 5,
      include_answer: true
    });

    const options = {
      hostname: 'api.tavily.com',
      path: '/search',
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${process.env.TAVILY_API_KEY}`,
        'Content-Length': Buffer.byteLength(data)
      }
    };

    const req = https.request(options, (res) => {
      let body = '';
      res.on('data', chunk => body += chunk);
      res.on('end', () => resolve(JSON.parse(body)));
    });

    req.on('error', reject);
    req.write(data);
    req.end();
  });
}

async function tavilyExtract(url) {
  console.log(`[Tool] Executing tavily_extract for URL: ${url}`);

  return new Promise((resolve, reject) => {
    const data = JSON.stringify({
      urls: [url],
      extract_depth: 'basic'
    });

    const options = {
      hostname: 'api.tavily.com',
      path: '/extract',
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${process.env.TAVILY_API_KEY}`,
        'Content-Length': Buffer.byteLength(data)
      }
    };

    const req = https.request(options, (res) => {
      let body = '';
      res.on('data', chunk => body += chunk);
      res.on('end', () => resolve(JSON.parse(body)));
    });

    req.on('error', reject);
    req.write(data);
    req.end();
  });
}

// Execute tool by name
async function executeTool(functionName, args) {
  switch (functionName) {
    case 'tavily_search':
      return await tavilySearch(args.query);
    case 'tavily_extract':
      return await tavilyExtract(args.url);
    default:
      throw new Error(`Unknown function: ${functionName}`);
  }
}

// Call Demeterics Groq proxy
async function callGroq(messages, tools) {
  return new Promise((resolve, reject) => {
    const data = JSON.stringify({
      model: 'meta-llama/llama-4-scout-17b-16e-instruct',
      messages,
      tools,
      tool_choice: 'auto'
    });

    const options = {
      hostname: 'api.demeterics.com',
      path: '/groq/v1/chat/completions',
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${process.env.DEMETERICS_API_KEY}`,
        'Content-Length': Buffer.byteLength(data)
      }
    };

    const req = https.request(options, (res) => {
      let body = '';
      res.on('data', chunk => body += chunk);
      res.on('end', () => resolve(JSON.parse(body)));
    });

    req.on('error', reject);
    req.write(data);
    req.end();
  });
}

// Main function
async function main() {
  console.log('========================================');
  console.log('Groq Tool Use - AI Agent Demo');
  console.log('========================================\n');

  const userQuery = 'Search for the latest developments in quantum computing and provide a summary';

  console.log(`User Query: ${userQuery}\n`);
  console.log('Step 1: Sending query to AI with tool definitions...\n');

  // Tool definitions
  const tools = [
    {
      type: 'function',
      function: {
        name: 'tavily_search',
        description: 'Search the web for current information using Tavily API',
        parameters: {
          type: 'object',
          properties: {
            query: {
              type: 'string',
              description: 'The search query'
            }
          },
          required: ['query']
        }
      }
    },
    {
      type: 'function',
      function: {
        name: 'tavily_extract',
        description: 'Extract content from a specific URL',
        parameters: {
          type: 'object',
          properties: {
            url: {
              type: 'string',
              description: 'The URL to extract content from'
            }
          },
          required: ['url']
        }
      }
    }
  ];

  // Step 1: Initial request
  const initialResponse = await callGroq([
    { role: 'user', content: userQuery }
  ], tools);

  // Check for tool calls
  const toolCalls = initialResponse.choices[0].message.tool_calls;

  if (!toolCalls || toolCalls.length === 0) {
    console.log('AI answered without using tools:\n');
    console.log(initialResponse.choices[0].message.content);
    return;
  }

  // Step 2: Execute tool calls
  console.log('========================================');
  console.log('AI decided to use tools!');
  console.log('========================================\n');
  console.log(`Number of tool calls: ${toolCalls.length}\n`);

  const toolMessages = [];

  for (const toolCall of toolCalls) {
    console.log(`Function: ${toolCall.function.name}`);
    console.log(`Arguments: ${toolCall.function.arguments}\n`);

    const args = JSON.parse(toolCall.function.arguments);
    const result = await executeTool(toolCall.function.name, args);

    toolMessages.push({
      role: 'tool',
      tool_call_id: toolCall.id,
      content: JSON.stringify(result)
    });

    console.log('✓ Tool executed successfully\n');
  }

  // Step 3: Send results back to AI
  console.log('Step 2: Sending tool results back to AI for final answer...\n');

  const finalResponse = await callGroq([
    { role: 'user', content: userQuery },
    initialResponse.choices[0].message,
    ...toolMessages
  ], tools);

  // Display final response
  console.log('========================================');
  console.log('Final AI Response');
  console.log('========================================\n');
  console.log(finalResponse.choices[0].message.content);
  console.log('');

  // Display token usage
  console.log('========================================');
  console.log('Token Usage');
  console.log('========================================\n');
  const usage = finalResponse.usage;
  console.log(`Prompt tokens: ${usage.prompt_tokens}`);
  console.log(`Completion tokens: ${usage.completion_tokens}`);
  console.log(`Total tokens: ${usage.total_tokens}`);

  const cost = (usage.prompt_tokens * 0.11 + usage.completion_tokens * 0.34) / 1000000;
  console.log(`Cost: $${cost.toFixed(6)}\n`);

  console.log('========================================');
  console.log('Summary');
  console.log('========================================\n');
  console.log('✓ AI agent successfully used tools');
  console.log(`✓ Executed ${toolCalls.length} tool call(s)`);
  console.log('✓ Retrieved real-time information');
  console.log('✓ Generated informed response\n');
}

main().catch(console.error);
