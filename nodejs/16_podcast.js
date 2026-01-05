////////////////////////////////////////////////////////////////////////////////
// Example 16: Multi-Speaker Podcast Generation
////////////////////////////////////////////////////////////////////////////////
//
// This example demonstrates how to generate a podcast with multiple speakers
// using Demeterics multi-speaker TTS API in a SINGLE API call.
//
// What you'll learn:
// - Multi-speaker audio generation with distinct voices
// - NPR/NotebookLM "Deep Dive" podcast style
// - Voice selection from 30 available options
// - Script formatting with "Speaker: text" pattern
//
// Prerequisites:
// - DEMETERICS_API_KEY environment variable
//
// Usage:
//   export DEMETERICS_API_KEY="dmt_your-api-key"
//   node 16_podcast.js
//
////////////////////////////////////////////////////////////////////////////////

const https = require('https');
const fs = require('fs');

// Step 1: Check for API key
const apiKey = process.env.DEMETERICS_API_KEY;
if (!apiKey) {
  console.error('Error: DEMETERICS_API_KEY not set');
  console.error('Get your key from: https://demeterics.com');
  process.exit(1);
}

// Step 2: Define the podcast script
// Format: "SpeakerName: dialogue text"
// Speaker names must match exactly in speakers array
const podcastScript = `Alex: Hey, welcome back to The Deep Dive! I am Alex.
Sam: And I am Sam. Today we are diving into something that every kid learns in school, but honestly, gets way more complicated when you dig into it.
Alex: Columbus. Christopher Columbus. 1492. In fourteen hundred ninety two, Columbus sailed the ocean blue.
Sam: Right! But here is the thing. That rhyme does not tell you much, does it?
Alex: Not at all! So get this. Columbus was not even trying to discover America. He thought he was finding a shortcut to Asia.
Sam: To India, specifically. Which is why he called the native people Indians.
Alex: Exactly! A massive geography fail that stuck around for five hundred years.
Sam: So let us set the scene. It is 1492. Columbus is Italian, from Genoa, but he is sailing for Spain.
Alex: Because Portugal said no! He pitched this idea everywhere. England said no. France said no. Portugal said hard no.
Sam: But Queen Isabella and King Ferdinand of Spain said, you know what, sure, let us do it.
Alex: And here is what is wild. Columbus was wrong about basically everything. He thought the Earth was way smaller than it actually is.
Sam: Most educated people knew the Earth was round. That is a myth that he proved it. They knew. They just thought his math was bad.
Alex: Because it was! If America was not there, he and his crew would have starved in the middle of the ocean.
Sam: So he gets lucky. Three ships. The Nina, the Pinta, and the Santa Maria. About two months at sea.
Alex: And on October 12th, 1492, they land in the Bahamas. Not mainland America. The Bahamas.
Sam: An island he named San Salvador. And the people already living there? The Taino people. They had been there for centuries.
Alex: So this whole discovery narrative is complicated, to say the least.
Sam: Very complicated. It is really the story of a European arriving somewhere that was not empty. And that changes everything.
Alex: That is the deep dive for today. Thanks for listening, everyone!
Sam: See you next time!`;

// Step 3: Configure voices for each speaker
// Available voices (30 total):
//   Puck (upbeat), Kore (firm), Charon (informative), Zephyr (bright),
//   Fenrir (excitable), Leda (youthful), Aoede (breezy), Sulafat (warm),
//   Achird (friendly), and 21 others.
const speakers = [
  { name: 'Alex', voice: 'Charon' },  // Informative, clear - explains concepts
  { name: 'Sam', voice: 'Puck' }      // Upbeat - energetic, enthusiastic
];

// Step 4: Build the request body (Demeterics format)
const requestBody = JSON.stringify({
  provider: 'gemini',
  input: podcastScript,
  speakers: speakers
});

// Step 5: Configure the API request
const options = {
  hostname: 'api.demeterics.com',
  port: 443,
  path: '/tts/v1/generate',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Authorization': `Bearer ${apiKey}`,
    'Content-Length': Buffer.byteLength(requestBody)
  }
};

console.log('========================================');
console.log('Multi-Speaker Podcast Generation');
console.log('========================================');
console.log('');
console.log('Speakers:');
speakers.forEach(s => console.log(`  ${s.name}: ${s.voice}`));
console.log('');
console.log(`Script length: ${podcastScript.length} characters`);
console.log('');
console.log('Generating podcast audio...');

// Step 6: Make the API request
const req = https.request(options, (res) => {
  const chunks = [];

  res.on('data', chunk => {
    chunks.push(chunk);
  });

  res.on('end', () => {
    // Check for errors
    if (res.statusCode !== 200) {
      console.error(`API Error: HTTP ${res.statusCode}`);
      console.error(Buffer.concat(chunks).toString());
      process.exit(1);
    }

    // Step 7: Save the audio file directly (Demeterics returns audio bytes)
    const audioBuffer = Buffer.concat(chunks);
    const outputFile = 'columbus_podcast.wav';
    fs.writeFileSync(outputFile, audioBuffer);

    console.log('');
    console.log('Success!');
    console.log('');
    console.log('Output:');
    console.log(`  File: ${outputFile}`);
    console.log(`  Size: ${audioBuffer.length} bytes (${Math.round(audioBuffer.length / 1024)} KB)`);
    console.log('');
    console.log('To play:');
    console.log(`  mpv ${outputFile}`);
  });
});

req.on('error', (error) => {
  console.error('Request error:', error.message);
  process.exit(1);
});

// Step 8: Send the request
req.write(requestBody);
req.end();
