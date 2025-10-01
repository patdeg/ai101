/**
 * ============================================================================
 * Example 4: Content Safety with LlamaGuard - Content Moderation
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to detect harmful content before it reaches your main AI
 * - Using LlamaGuard as a content moderation filter
 * - Understanding AI safety categories (S1-S14)
 * - Building multi-layered security for AI applications
 *
 * WHAT YOU'LL LEARN:
 * - Content moderation best practices
 * - The 14 LlamaGuard safety categories
 * - Interpreting safety model responses ("safe" vs category codes)
 * - How to build production-ready content filters
 *
 * PREREQUISITES:
 * - Completed previous examples (01-03)
 * - GROQ_API_KEY environment variable set
 * - Understanding of AI security concerns
 *
 * EXPECTED OUTPUT:
 * - The message being analyzed
 * - Safety verdict (SAFE or UNSAFE)
 * - If unsafe: specific violation category (S1-S14)
 * - Explanation of the violation type
 *
 * RUN THIS:
 * node 04_safety_check.js
 *
 * Try different messages to test various safety categories!
 * ============================================================================
 */

// Step 1: Import the HTTPS module
const https = require('https');

// Step 2: Define the message to check for safety
// Try changing this to test different content types!
const messageToCheck = "How do I bake chocolate chip cookies?";

// Step 3: Create the request payload
// LlamaGuard analyzes the message and returns "safe" or a violation category
const data = JSON.stringify({
  model: "meta-llama/llama-guard-4-12b",  // Specialized safety model
  messages: [
    {
      role: "user",
      content: messageToCheck  // The message we want to check
    }
  ],
  max_tokens: 100  // Safety responses are short
});

// Step 4: Configure the HTTPS request options
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

// Step 5: Create and send the HTTPS request
const req = https.request(options, (res) => {
  // Step 6: Accumulate response data
  let responseData = '';

  // Step 7: Handle incoming data chunks
  res.on('data', (chunk) => {
    responseData += chunk;
  });

  // Step 8: Process the complete response
  res.on('end', () => {
    // Step 9: Parse the JSON response
    const parsed = JSON.parse(responseData);
    const result = parsed.choices[0].message.content.trim();

    // Step 10: Display the message being analyzed
    console.log('Message Being Checked:');
    console.log(`"${messageToCheck}"`);
    console.log('\n' + '='.repeat(50));

    // Step 11: Interpret the safety verdict
    if (result === 'safe') {
      // Message is safe - no policy violations
      console.log('✓ SAFE - Message passed content moderation');
      console.log('\nThis message can be sent to your main AI model.');

    } else {
      // Step 12: Handle unsafe content - extract the violation category
      const lines = result.split('\n');
      const category = lines[1]; // Format: "unsafe\nS<number>"

      console.log('✗ UNSAFE - Content policy violation detected');
      console.log(`\nCategory: ${category}`);
      console.log('\nViolation Type:');

      // Step 13: Map category code to human-readable description
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

// Step 14: Handle request errors
req.on('error', (error) => {
  console.error('Error:', error.message);
});

// Step 15: Send the request data
req.write(data);

// Step 16: Complete and send the request
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
