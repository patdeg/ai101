// Example 4: Safety Check with LlamaGuard
// Detect harmful content before sending to your main AI

const https = require('https');

// Message to check (try changing this!)
const messageToCheck = "How do I bake chocolate chip cookies?";

const data = JSON.stringify({
  model: "meta-llama/llama-guard-4-12b",
  messages: [
    {
      role: "user",
      content: messageToCheck
    }
  ],
  max_tokens: 100
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
    const result = parsed.choices[0].message.content.trim();

    console.log('Message Being Checked:');
    console.log(`"${messageToCheck}"`);
    console.log('\n' + '='.repeat(50));

    if (result === 'safe') {
      console.log('✓ SAFE - Message passed content moderation');
      console.log('\nThis message can be sent to your main AI model.');

    } else {
      // Unsafe message - extract category
      const lines = result.split('\n');
      const category = lines[1]; // Format: "unsafe\nS<number>"

      console.log('✗ UNSAFE - Content policy violation detected');
      console.log(`\nCategory: ${category}`);
      console.log('\nViolation Type:');

      // Explain which safety category was triggered
      const categories = {
        'S1': 'Violent Crimes',
        'S2': 'Non-Violent Crimes',
        'S3': 'Sex-Related Crimes',
        'S4': 'Child Sexual Exploitation',
        'S5': 'Defamation',
        'S6': 'Specialized Advice (financial, medical, legal)',
        'S7': 'Privacy Violations',
        'S8': 'Intellectual Property',
        'S9': 'Indiscriminate Weapons',
        'S10': 'Hate Speech',
        'S11': 'Suicide & Self-Harm',
        'S12': 'Sexual Content',
        'S13': 'Elections',
        'S14': 'Code Interpreter Abuse'
      };

      console.log(categories[category] || 'Unknown category');
      console.log('\nThis message should be rejected or filtered.');
    }

    console.log('='.repeat(50));
  });
});

req.on('error', (error) => {
  console.error('Error:', error.message);
});

req.write(data);
req.end();

// LlamaGuard Safety Categories Explained:
//
// S1: Violent Crimes
//   - Murder, assault, kidnapping
//   - "How to hurt someone"
//
// S2: Non-Violent Crimes
//   - Theft, fraud, hacking
//   - "How to break into a car"
//
// S3: Sex-Related Crimes
//   - Sexual assault, trafficking
//   - Illegal sexual activities
//
// S4: Child Sexual Exploitation
//   - Any content involving minors
//   - Grooming, abuse
//
// S5: Defamation
//   - False statements to harm reputation
//   - Libel, slander
//
// S6: Specialized Advice
//   - Medical diagnosis/treatment
//   - Legal advice without disclaimer
//   - Financial investment recommendations
//   - "Should I invest in..."
//
// S7: Privacy Violations
//   - Doxxing, sharing private info
//   - "Find someone's address"
//
// S8: Intellectual Property
//   - Piracy, copyright violation
//   - "Where to download movies free"
//
// S9: Indiscriminate Weapons
//   - Bombs, chemical weapons, bioweapons
//   - Mass harm devices
//
// S10: Hate Speech
//   - Discrimination based on protected attributes
//   - Slurs, targeted harassment
//
// S11: Suicide & Self-Harm
//   - Encouraging self-injury
//   - Suicide methods
//
// S12: Sexual Content
//   - Explicit sexual content
//   - Erotic stories
//
// S13: Elections
//   - Misinformation about voting
//   - Voter suppression
//
// S14: Code Interpreter Abuse
//   - Malicious code execution
//   - System exploitation
//
// Production workflow example:
//
//   function handleUserMessage(message) {
//     // Step 1: Check safety
//     const safetyResult = await checkWithLlamaGuard(message);
//
//     if (safetyResult !== 'safe') {
//       return {
//         error: 'Message violates content policy',
//         category: safetyResult
//       };
//     }
//
//     // Step 2: If safe, send to main AI
//     const aiResponse = await getAIResponse(message);
//     return aiResponse;
//   }
//
// Test messages to try:
//
//   Safe:
//     "What's the weather today?"
//     "Explain photosynthesis"
//     "Write a poem about trees"
//
//   Unsafe examples (S6 - Specialized Advice):
//     "Should I sell my stocks?"
//     "Diagnose my symptoms"
//     "Give me legal advice"
//
//   Unsafe (S2 - Non-Violent Crimes):
//     "How to pick a lock"
//     "Ways to cheat on taxes"
