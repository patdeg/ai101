#!/usr/bin/env node
/*
Example 15: Text-to-Speech with Fish Audio (via OpenRouter)

Demonstrates:
- Converting text to spoken audio using OpenRouter's /audio/speech endpoint
- Fish Audio S2.1 Pro Free - a genuinely free TTS model, no card required
- Picking a voice by reference ID from Fish Audio's voice library

What you'll learn:
- How to call a TTS model through OpenRouter's unified API
- Optional voice selection via a Fish Audio reference ID
- How to save audio files from API responses

Note: Uses Node.js built-in modules only (https, fs, process)
*/

const https = require('https');
const fs = require('fs');

// Check for API key
if (!process.env.OPENROUTER_API_KEY) {
  console.error('Error: OPENROUTER_API_KEY environment variable not set');
  console.error('Get your key from: https://openrouter.ai/keys');
  process.exit(1);
}

// Function to call OpenRouter's /audio/speech endpoint (Fish Audio S2.1 Pro Free)
function textToSpeech(text, referenceId) {
  const requestData = {
    model: 'fish-audio/s2.1-pro-free:free',
    input: text,
    response_format: 'mp3'
  };
  // Without a referenceId, Fish Audio picks a default voice - good enough for
  // a first TTS demo. Pass one to pick a specific voice (see Example 16 for
  // the multi-speaker version of this).
  if (referenceId) {
    requestData.providerOptions = { fishAudio: { referenceId } };
  }

  const data = JSON.stringify(requestData);

  const requestOptions = {
    hostname: 'openrouter.ai',
    port: 443,
    path: '/api/v1/audio/speech',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${process.env.OPENROUTER_API_KEY}`,
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
        if (res.statusCode !== 200) {
          reject(new Error(`API Error: HTTP ${res.statusCode} - ${audioBuffer.toString()}`));
          return;
        }
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
  console.log('Text-to-Speech with Fish Audio (OpenRouter)');
  console.log('========================================\n');

  // Demo 1: Basic TTS with the default voice
  console.log('========================================');
  console.log('Demo 1: Basic Text-to-Speech');
  console.log('========================================\n');

  const text = "Hello! This is an example of text-to-speech synthesis using Fish Audio's free TTS model.";
  const outputFile = 'demo1_default_voice.mp3';

  console.log(`Text: ${text}\n`);

  const audioBuffer = await textToSpeech(text);
  fs.writeFileSync(outputFile, audioBuffer);

  console.log(`✓ Audio created: ${outputFile} (${audioBuffer.length} bytes)\n`);

  // Demo 2: A specific voice, if the learner has picked one
  console.log('========================================');
  console.log('Demo 2: A Voice Picked From Fish Audio');
  console.log('========================================\n');

  const referenceId = process.env.FISH_VOICE_1;
  if (referenceId) {
    const comparisonText = "This is the same text, spoken with a voice picked from Fish Audio's library.";
    const voiceFile = 'demo2_chosen_voice.mp3';

    const voiceBuffer = await textToSpeech(comparisonText, referenceId);
    fs.writeFileSync(voiceFile, voiceBuffer);

    console.log(`✓ Chosen voice: ${voiceFile} (${voiceBuffer.length} bytes)\n`);
  } else {
    console.log('Tip: set FISH_VOICE_1 to a voice ID from https://fish.audio to');
    console.log('hear a specific voice instead of the default.\n');
  }

  // Cost Analysis
  console.log('========================================');
  console.log('Cost Analysis');
  console.log('========================================\n');

  console.log('Pricing (fish-audio/s2.1-pro-free:free):');
  console.log('  $0 - free tier, rate-limited, best-effort availability');
  console.log('  (meant for learning/prototyping, not production)\n');

  // Summary
  console.log('========================================');
  console.log('Summary');
  console.log('========================================\n');
  console.log('✓ Generated audio with the default voice');
  console.log('✓ Showed how to pick a specific voice by reference ID');
  console.log('✓ Zero cost on the free tier\n');
  console.log('Next: Example 16 (podcast) uses this same model for a');
  console.log('two-speaker conversation in a single API call.\n');
}

main().catch(error => {
  console.error('Error:', error.message);
  process.exit(1);
});
