#!/usr/bin/env node
/*
Example 14: Text-to-Speech with OpenAI

Demonstrates:
- Converting text to spoken audio with OpenAI's TTS
- 11 different English voice options
- Instructions parameter for voice customization
- Speed control (0.25x to 4.0x)
- Multiple audio formats
- Multi-language support

What you'll learn:
- How to use OpenAI's affordable TTS model
- When to use different voices and instructions
- Cost implications of TTS ($0.60/1M input tokens!)
- How to save audio files from API responses

Note: Uses Node.js built-in modules only (https, fs, process)
*/

const https = require('https');
const fs = require('fs');

// Check for API key
if (!process.env.OPENAI_API_KEY) {
  console.error('Error: OPENAI_API_KEY environment variable not set');
  console.error('Get your key from: https://platform.openai.com');
  process.exit(1);
}

// Function to call OpenAI TTS API
function textToSpeech(text, voice, options = {}) {
  const requestData = {
    model: 'gpt-4o-mini-tts',
    input: text,
    voice: voice,
    ...options
  };

  const data = JSON.stringify(requestData);

  const requestOptions = {
    hostname: 'api.openai.com',
    port: 443,
    path: '/v1/audio/speech',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${process.env.OPENAI_API_KEY}`,
      'Content-Length': Buffer.byteLength(data)
    }
  };

  return new Promise((resolve, reject) => {
    const req = https.request(requestOptions, (res) => {
      const chunks = [];

      res.on('data', (chunk) => {
        chunks.push(chunk);
      });

      res.on('end', () => {
        const audioBuffer = Buffer.concat(chunks);
        resolve(audioBuffer);
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
  console.log('Text-to-Speech with OpenAI');
  console.log('========================================\n');

  // Demo 1: Basic TTS
  console.log('========================================');
  console.log('Demo 1: Basic Text-to-Speech');
  console.log('========================================\n');

  const text = "Hello! This is an example of text-to-speech synthesis using OpenAI's affordable TTS model.";
  const voice = "alloy";
  const outputFile = "demo1_alloy.mp3";

  console.log(`Text: ${text}`);
  console.log(`Voice: ${voice}\n`);

  const audioBuffer = await textToSpeech(text, voice);
  fs.writeFileSync(outputFile, audioBuffer);

  console.log(`✓ Audio created: ${outputFile} (${audioBuffer.length} bytes)\n`);

  // Demo 2: Voice Comparison
  console.log('========================================');
  console.log('Demo 2: Comparing All 11 Voices');
  console.log('========================================\n');

  const comparisonText = "Welcome to OpenAI's text-to-speech demonstration.";
  const voices = {
    'alloy': 'Neutral, balanced',
    'ash': 'Clear, articulate',
    'ballad': 'Smooth, expressive',
    'coral': 'Warm, friendly',
    'echo': 'Clear, professional',
    'fable': 'Narrative, storytelling',
    'onyx': 'Deep, authoritative',
    'nova': 'Energetic, youthful',
    'sage': 'Calm, measured',
    'shimmer': 'Bright, cheerful',
    'verse': 'Poetic, expressive'
  };

  console.log('Generating samples with all 11 voices...\n');

  for (const [voiceName, description] of Object.entries(voices)) {
    const voiceOutputFile = `voice_${voiceName}.mp3`;
    console.log(`Voice: ${voiceName} (${description})`);

    const voiceBuffer = await textToSpeech(comparisonText, voiceName);
    fs.writeFileSync(voiceOutputFile, voiceBuffer);

    console.log(`  ✓ Created: ${voiceOutputFile} (${voiceBuffer.length} bytes)\n`);
  }

  // Demo 3: Instructions Parameter
  console.log('========================================');
  console.log('Demo 3: Voice Customization with Instructions');
  console.log('========================================\n');

  const examples = [
    {
      name: 'enthusiastic',
      text: 'The quarterly results exceeded expectations.',
      voice: 'nova',
      instructions: 'Speak with enthusiasm and excitement, emphasizing positive words.'
    },
    {
      name: 'professional',
      text: 'The quarterly results exceeded expectations.',
      voice: 'onyx',
      instructions: 'Speak in a serious, professional tone suitable for a boardroom presentation.'
    },
    {
      name: 'storytelling',
      text: 'Once upon a time, in a land far away, there lived a curious explorer.',
      voice: 'fable',
      instructions: 'Speak as if narrating a fairy tale, with dramatic pauses and expressive intonation.'
    }
  ];

  for (const example of examples) {
    const instructionFile = `instruction_${example.name}.mp3`;
    console.log(`${example.name.charAt(0).toUpperCase() + example.name.slice(1)} delivery:`);
    console.log(`  Instructions: "${example.instructions}"`);

    const instructionBuffer = await textToSpeech(example.text, example.voice, {
      instructions: example.instructions
    });

    fs.writeFileSync(instructionFile, instructionBuffer);
    console.log(`  ✓ Created: ${instructionFile}\n`);
  }

  // Demo 4: Speed Control
  console.log('========================================');
  console.log('Demo 4: Speed Variations');
  console.log('========================================\n');

  const speedText = "This sentence demonstrates different playback speeds.";
  const speeds = [0.5, 1.0, 1.5, 2.0];

  for (const speed of speeds) {
    const speedFile = `speed_${speed}x.mp3`;
    console.log(`Speed: ${speed}x`);

    const speedBuffer = await textToSpeech(speedText, 'alloy', { speed });
    fs.writeFileSync(speedFile, speedBuffer);

    console.log(`  ✓ Created: ${speedFile}\n`);
  }

  // Demo 5: Multi-Language
  console.log('========================================');
  console.log('Demo 5: Multi-Language Support');
  console.log('========================================\n');

  const languages = [
    { lang: 'Spanish', text: 'Hola, bienvenido a la demostración de síntesis de voz.', voice: 'coral' },
    { lang: 'French', text: 'Bonjour, bienvenue à la démonstration de synthèse vocale.', voice: 'ballad' },
    { lang: 'German', text: 'Hallo, willkommen zur Sprachsynthese-Demonstration.', voice: 'echo' }
  ];

  for (const { lang, text: langText, voice: langVoice } of languages) {
    const langFile = `lang_${lang.toLowerCase()}.mp3`;
    console.log(`${lang}:`);

    const langBuffer = await textToSpeech(langText, langVoice);
    fs.writeFileSync(langFile, langBuffer);

    console.log(`  ✓ Created: ${langFile}\n`);
  }

  // Cost Analysis
  console.log('========================================');
  console.log('Cost Analysis');
  console.log('========================================\n');

  const inputCostPer1M = 0.60;
  const outputCostPer1M = 12.00;
  const textLength = text.length;
  const estimatedInputTokens = textLength;
  const inputCost = (estimatedInputTokens * inputCostPer1M) / 1000000;

  console.log('Pricing (gpt-4o-mini-tts):');
  console.log('  Input:  $0.60 per 1M tokens');
  console.log('  Output: $12.00 per 1M tokens (audio duration-based)\n');

  console.log('This demo:');
  console.log(`  Text length: ${textLength} characters`);
  console.log(`  Estimated input tokens: ${estimatedInputTokens}`);
  console.log(`  Input cost: $${inputCost.toFixed(8)}`);
  console.log('  Note: Output cost varies by audio duration\n');

  console.log('Cost examples:');
  console.log('  100 chars:   ~$0.00006 input');
  console.log('  1,000 chars: ~$0.0006 input');
  console.log('  10,000 chars: ~$0.006 input\n');

  console.log('Much more affordable than alternatives!\n');

  // Summary
  console.log('========================================');
  console.log('Summary');
  console.log('========================================\n');
  console.log('✓ Generated audio with all 11 voices');
  console.log('✓ Demonstrated instructions parameter');
  console.log('✓ Showed speed control (0.5x to 2.0x)');
  console.log('✓ Tested multi-language support');
  console.log('✓ Affordable pricing: $0.60/1M input tokens\n');

  console.log('Voice selection guide:');
  console.log('  Professional: echo, onyx, sage');
  console.log('  Friendly: coral, nova, shimmer');
  console.log('  Storytelling: fable, ballad, verse');
  console.log('  Neutral: alloy, ash\n');
}

main().catch(error => {
  console.error('Error:', error.message);
  process.exit(1);
});
