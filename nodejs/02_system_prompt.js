// Example 2: System + User Prompt
// Control AI behavior with system instructions

const https = require('https');

const data = JSON.stringify({
  model: "meta-llama/llama-4-scout-17b-16e-instruct",
  messages: [
    {
      role: "system",
      content: "You are a concise expert in world history. Answer questions in exactly 2 sentences, no more."
    },
    {
      role: "user",
      content: "Why did the Roman Empire fall?"
    }
  ],
  temperature: 0.5,
  max_tokens: 150
});

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

const req = https.request(options, (res) => {
  let responseData = '';

  res.on('data', (chunk) => {
    responseData += chunk;
  });

  res.on('end', () => {
    const parsed = JSON.parse(responseData);

    console.log('System Prompt Used:');
    console.log(JSON.parse(data).messages[0].content);

    console.log('\nUser Question:');
    console.log(JSON.parse(data).messages[1].content);

    console.log('\nAI Response (following system rules):');
    console.log(parsed.choices[0].message.content);

    console.log('\nToken Usage:', parsed.usage.total_tokens);
  });
});

req.on('error', (error) => {
  console.error('Error:', error.message);
});

req.write(data);
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
