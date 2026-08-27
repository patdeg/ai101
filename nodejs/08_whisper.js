/**
 * ============================================================================
 * Example 7: Whisper - Audio Transcription
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to transcribe audio files using OpenAI's Whisper model
 * - Base64-encoding an audio file and sending it as JSON
 * - Processing speech-to-text
 * - Working with file reads in Node.js
 *
 * WHAT YOU'LL LEARN:
 * - How to use the Whisper API for audio transcription
 * - Base64 encoding audio for a JSON request body (same pattern as the
 *   vision example's image encoding)
 * - Handling audio file uploads with the HTTPS module
 * - Calculating transcription costs
 *
 * PREREQUISITES:
 * - Node.js installed
 * - OPENROUTER_API_KEY environment variable set
 * - Audio file gettysburg.mp3 in the repository root
 *
 * EXPECTED OUTPUT:
 * - Complete transcription of the Gettysburg Address
 * - JSON response with transcription text
 * - Duration and cost information
 *
 * RUN THIS:
 * node 08_whisper.js
 *
 * ============================================================================
 */

// Step 1: Import required modules
const https = require('https');
const fs = require('fs');
const path = require('path');

// Step 2: Prepare the audio file
const audioFilePath = path.join(__dirname, '..', 'gettysburg.mp3');

// Check if file exists
if (!fs.existsSync(audioFilePath)) {
  console.error(`Error: Audio file not found at ${audioFilePath}`);
  process.exit(1);
}

// Read the audio file and base64-encode it (same idea as image encoding in 04_vision.js)
const audioData = fs.readFileSync(audioFilePath);
const audioBase64 = audioData.toString('base64');

console.log('='.repeat(60));
console.log('Transcribing Audio with Whisper');
console.log('='.repeat(60));
console.log(`Model: openai/whisper-1`);
console.log(`File: ${audioFilePath}`);
console.log(`File size: ${audioData.length} bytes`);
console.log(`Base64 size: ${audioBase64.length} bytes`);
console.log('');

// Step 3: Build the JSON request body
// input_audio.data holds the base64 audio, input_audio.format is the codec
const requestBody = JSON.stringify({
  model: 'openai/whisper-1',
  input_audio: {
    data: audioBase64,
    format: 'mp3'
  }
});

// Step 4: Configure the HTTPS request
const options = {
  hostname: 'openrouter.ai',
  port: 443,
  path: '/api/v1/audio/transcriptions',
  method: 'POST',
  headers: {
    'Authorization': `Bearer ${process.env.OPENROUTER_API_KEY}`,
    'Content-Type': 'application/json',
    'Content-Length': Buffer.byteLength(requestBody)
  }
};

// Step 5: Send the request
const startTime = Date.now();

const req = https.request(options, (res) => {
  let responseData = '';

  res.on('data', (chunk) => {
    responseData += chunk;
  });

  res.on('end', () => {
    const latency = Date.now() - startTime;

    try {
      const parsed = JSON.parse(responseData);

      console.log('Transcription Result:');
      console.log('='.repeat(60));
      console.log(parsed.text || 'No transcription available');
      console.log('');
      console.log('='.repeat(60));
      console.log('');

      // Calculate cost based on audio duration (Whisper is billed per minute)
      const durationSeconds = parsed.duration || 0;
      const cost = (durationSeconds / 60) * 0.006;  // $0.006 per minute

      console.log('Performance Metrics:');
      console.log('='.repeat(60));
      console.log(`API Latency:    ${latency}ms`);
      console.log(`Audio Duration: ${durationSeconds}s`);
      console.log(`Cost:           $${cost.toFixed(6)}`);
      console.log('='.repeat(60));
      console.log('');
      console.log('Full API Response:');
      console.log(JSON.stringify(parsed, null, 2));
    } catch (error) {
      console.error('Error parsing response:', error.message);
      console.error('Raw response:', responseData);
    }
  });
});

req.on('error', (error) => {
  console.error('Error:', error.message);
});

req.write(requestBody);
req.end();

// Model: openai/whisper-1 (via OpenRouter's /audio/transcriptions endpoint)
//   - Speech recognition, supports 99+ languages with high accuracy
//   - Automatic language detection
//   - Handles various audio formats (mp3, wav, m4a, flac, etc.)
//   - Pricing: $0.006 per minute of audio (no free STT model exists on
//     OpenRouter as of this writing - this is the cheapest correct option)
//
// Supported audio formats:
//   - MP3, MP4, MPEG, MPGA, M4A, WAV, WEBM, FLAC
//   - Maximum file size: 25 MB
//   - Recommended: 16kHz or higher sample rate
//
// Pricing examples (at $0.006/minute):
//   - 1 minute:   $0.006
//   - 5 minutes:  $0.03
//   - 30 minutes: $0.18
//   - 1 hour:     $0.36
//
// API request format:
//   {
//     "model": "openai/whisper-1",
//     "input_audio": {
//       "data": "<base64-encoded-audio>",
//       "format": "mp3"
//     }
//   }
//
// API response format:
//   {
//     "text": "The transcribed text will appear here...",
//     "duration": 272.5
//   }
//
// Use cases:
//   - Meeting transcriptions
//   - Voice notes to text
//   - Podcast transcriptions
//   - Accessibility (captions, subtitles)
//   - Voice interfaces
//   - Call center analytics
//   - Interview transcriptions
//   - Lecture notes
//
// Production code example:
//
//   async function transcribeAudio(audioPath) {
//     return new Promise((resolve, reject) => {
//       const audioData = fs.readFileSync(audioPath);
//       const audioBase64 = audioData.toString('base64');
//
//       const body = JSON.stringify({
//         model: 'openai/whisper-1',
//         input_audio: { data: audioBase64, format: 'mp3' }
//       });
//
//       const options = {
//         hostname: 'openrouter.ai',
//         port: 443,
//         path: '/api/v1/audio/transcriptions',
//         method: 'POST',
//         headers: {
//           'Authorization': `Bearer ${process.env.OPENROUTER_API_KEY}`,
//           'Content-Type': 'application/json',
//           'Content-Length': Buffer.byteLength(body)
//         }
//       };
//
//       const req = https.request(options, (res) => {
//         let data = '';
//         res.on('data', (chunk) => data += chunk);
//         res.on('end', () => {
//           try {
//             resolve(JSON.parse(data));
//           } catch (error) {
//             reject(error);
//           }
//         });
//       });
//
//       req.on('error', reject);
//       req.write(body);
//       req.end();
//     });
//   }
//
//   // Usage
//   transcribeAudio('../gettysburg.mp3')
//     .then(result => console.log(result.text))
//     .catch(error => console.error(error));
//
// Advanced options (add to the request body):
//
//   // Specify language (optional - auto-detected by default)
//   { model: 'openai/whisper-1', input_audio: {...}, language: 'en' }
//
//   // Response format (optional - json is default)
//   { model: 'openai/whisper-1', input_audio: {...}, response_format: 'text' }
//
// Error handling example:
//
//   function transcribeAudio(audioPath) {
//     // Check if file exists
//     if (!fs.existsSync(audioPath)) {
//       console.error(`Error: File not found: ${audioPath}`);
//       return;
//     }
//
//     // Read file
//     let audioData;
//     try {
//       audioData = fs.readFileSync(audioPath);
//     } catch (error) {
//       console.error(`Error reading file: ${error.message}`);
//       return;
//     }
//
//     // Check file size (25MB limit)
//     if (audioData.length > 25 * 1024 * 1024) {
//       console.error('Error: File size exceeds 25MB limit');
//       return;
//     }
//
//     // Continue with transcription...
//   }
//
// Saving transcription to file:
//
//   res.on('end', () => {
//     const parsed = JSON.parse(responseData);
//
//     // Save as plain text
//     fs.writeFileSync('transcription.txt', parsed.text, 'utf8');
//
//     // Save as JSON
//     fs.writeFileSync('transcription.json', JSON.stringify(parsed, null, 2), 'utf8');
//   });
//
// Supported languages (99+):
//   English, Spanish, French, German, Italian, Portuguese, Dutch,
//   Russian, Chinese, Japanese, Korean, Arabic, Turkish, Polish,
//   Ukrainian, Vietnamese, Thai, Indonesian, Hindi, and many more
//
// Language codes (ISO-639-1):
//   en = English, es = Spanish, fr = French, de = German,
//   it = Italian, pt = Portuguese, nl = Dutch, ru = Russian,
//   zh = Chinese, ja = Japanese, ko = Korean, ar = Arabic,
//   tr = Turkish, pl = Polish, uk = Ukrainian, vi = Vietnamese,
//   th = Thai, id = Indonesian, hi = Hindi
//
// Performance tips:
//   - Use MP3 format for smaller file sizes
//   - 16kHz sample rate is usually sufficient
//   - Mono audio reduces file size by ~50%
//   - Remove silence to reduce processing time
//
// Cost optimization:
//   - Whisper pricing is based on audio duration
//   - Remove silence before transcription
//   - Cache transcriptions to avoid re-processing
