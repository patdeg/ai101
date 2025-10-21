/**
 * ============================================================================
 * Example 7: Whisper - Audio Transcription with Whisper Large-v3-Turbo
 * ============================================================================
 *
 * WHAT THIS DEMONSTRATES:
 * - How to transcribe audio files using Whisper large-v3-turbo
 * - Uploading multipart form data to the API
 * - Processing speech-to-text with fast, cost-effective model
 * - Working with file uploads in Node.js
 *
 * WHAT YOU'LL LEARN:
 * - How to use the Whisper API for audio transcription
 * - Working with multipart/form-data requests in Node.js
 * - Handling audio file uploads with HTTPS module
 * - Processing binary file data
 * - Calculating transcription costs
 *
 * PREREQUISITES:
 * - Node.js installed
 * - GROQ_API_KEY environment variable set
 * - Audio file gettysburg.mp3 in the repository root
 *
 * EXPECTED OUTPUT:
 * - Complete transcription of the Gettysburg Address
 * - JSON response with transcription text
 * - Duration and cost information
 *
 * RUN THIS:
 * node 07_whisper.js
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

// Read the audio file
const audioData = fs.readFileSync(audioFilePath);
const fileName = path.basename(audioFilePath);

console.log('='.repeat(60));
console.log('Transcribing Audio with Whisper');
console.log('='.repeat(60));
console.log(`Model: whisper-large-v3-turbo`);
console.log(`File: ${audioFilePath}`);
console.log(`File size: ${audioData.length} bytes`);
console.log('');

// Step 3: Create multipart form data
// Generate a boundary string for multipart data
const boundary = '----WebKitFormBoundary' + Math.random().toString(36).substring(2);

// Build the multipart body
let body = '';

// Part 1: The audio file
body += `--${boundary}\r\n`;
body += `Content-Disposition: form-data; name="file"; filename="${fileName}"\r\n`;
body += `Content-Type: audio/mpeg\r\n`;
body += '\r\n';

// Convert body to Buffer and append audio data
const header = Buffer.from(body, 'utf8');
const footer = Buffer.from(
  `\r\n--${boundary}\r\n` +
  `Content-Disposition: form-data; name="model"\r\n` +
  `\r\n` +
  `whisper-large-v3-turbo\r\n` +
  `--${boundary}\r\n` +
  `Content-Disposition: form-data; name="response_format"\r\n` +
  `\r\n` +
  `verbose_json\r\n` +
  `--${boundary}--\r\n`,
  'utf8'
);

// Combine all parts
const requestBody = Buffer.concat([header, audioData, footer]);

// Step 4: Configure the HTTPS request
const options = {
  hostname: 'api.groq.com',
  port: 443,
  path: '/openai/v1/audio/transcriptions',
  method: 'POST',
  headers: {
    'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,
    'Content-Type': `multipart/form-data; boundary=${boundary}`,
    'Content-Length': requestBody.length
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

      // Calculate cost based on audio duration
      const duration = parsed.duration || 0;
      const cost = (duration / 3600) * 0.04;  // $0.04 per hour

      console.log('Performance Metrics:');
      console.log('='.repeat(60));
      console.log(`API Latency:    ${latency}ms`);
      console.log(`Audio Duration: ${duration}s`);
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

// Model: whisper-large-v3-turbo
//   - Fast, cost-effective speech recognition from OpenAI
//   - Supports 99+ languages with high accuracy
//   - Automatic language detection
//   - Punctuation and formatting included
//   - Handles various audio formats (mp3, wav, m4a, etc.)
//   - Pricing: $0.04 per hour of audio
//
// Supported audio formats:
//   - MP3, MP4, MPEG, MPGA, M4A, WAV, WEBM
//   - Maximum file size: 25 MB
//   - Recommended: 16kHz or higher sample rate
//
// Pricing examples (at $0.04/hour):
//   - 1 minute:  $0.000667 (~$0.0007)
//   - 5 minutes: $0.003333 (~$0.003)
//   - 30 minutes: $0.02
//   - 1 hour:     $0.04
//   - 10 hours:   $0.40
//
// API Response format:
//   {
//     "text": "The transcribed text will appear here..."
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
//       // Read audio file
//       const audioData = fs.readFileSync(audioPath);
//       const fileName = path.basename(audioPath);
//
//       // Create multipart body
//       const boundary = '----WebKitFormBoundary' + Math.random().toString(36).substring(2);
//       const header = Buffer.from(
//         `--${boundary}\r\n` +
//         `Content-Disposition: form-data; name="file"; filename="${fileName}"\r\n` +
//         `Content-Type: audio/mpeg\r\n\r\n`,
//         'utf8'
//       );
//       const footer = Buffer.from(
//         `\r\n--${boundary}\r\n` +
//         `Content-Disposition: form-data; name="model"\r\n\r\n` +
//         `whisper-large-v3\r\n` +
//         `--${boundary}--\r\n`,
//         'utf8'
//       );
//       const body = Buffer.concat([header, audioData, footer]);
//
//       // Configure request
//       const options = {
//         hostname: 'api.groq.com',
//         port: 443,
//         path: '/openai/v1/audio/transcriptions',
//         method: 'POST',
//         headers: {
//           'Authorization': `Bearer ${process.env.GROQ_API_KEY}`,
//           'Content-Type': `multipart/form-data; boundary=${boundary}`,
//           'Content-Length': body.length
//         }
//       };
//
//       // Send request
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
// Advanced options (add to form data):
//
//   // Specify language (optional - auto-detected by default)
//   footer = Buffer.from(
//     `\r\n--${boundary}\r\n` +
//     `Content-Disposition: form-data; name="model"\r\n\r\n` +
//     `whisper-large-v3\r\n` +
//     `--${boundary}\r\n` +
//     `Content-Disposition: form-data; name="language"\r\n\r\n` +
//     `en\r\n` +  // ISO-639-1 code (en, es, fr, de, etc.)
//     `--${boundary}--\r\n`,
//     'utf8'
//   );
//
//   // Response format (optional - json is default)
//   // Options: json, text, srt, vtt
//   footer = Buffer.from(
//     `\r\n--${boundary}\r\n` +
//     `Content-Disposition: form-data; name="model"\r\n\r\n` +
//     `whisper-large-v3\r\n` +
//     `--${boundary}\r\n` +
//     `Content-Disposition: form-data; name="response_format"\r\n\r\n` +
//     `json\r\n` +
//     `--${boundary}--\r\n`,
//     'utf8'
//   );
//
//   // Temperature for randomness (optional - 0.0 to 1.0)
//   // 0.0 = deterministic, 1.0 = creative
//   footer = Buffer.from(
//     `\r\n--${boundary}\r\n` +
//     `Content-Disposition: form-data; name="model"\r\n\r\n` +
//     `whisper-large-v3\r\n` +
//     `--${boundary}\r\n` +
//     `Content-Disposition: form-data; name="temperature"\r\n\r\n` +
//     `0.0\r\n` +
//     `--${boundary}--\r\n`,
//     'utf8'
//   );
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
// Processing long audio files:
//
//   // For files > 25MB, you'll need to split them
//   // This requires additional libraries like ffmpeg
//   const { exec } = require('child_process');
//
//   function splitAudio(inputFile, chunkDuration = 600) {
//     // Split audio into 10-minute chunks using ffmpeg
//     return new Promise((resolve, reject) => {
//       const cmd = `ffmpeg -i ${inputFile} -f segment -segment_time ${chunkDuration} -c copy chunk_%03d.mp3`;
//       exec(cmd, (error, stdout, stderr) => {
//         if (error) reject(error);
//         else resolve();
//       });
//     });
//   }
//
//   async function transcribeLongAudio(audioPath) {
//     // Split into chunks
//     await splitAudio(audioPath);
//
//     // Get list of chunks
//     const chunks = fs.readdirSync('.')
//       .filter(f => f.startsWith('chunk_'))
//       .sort();
//
//     // Transcribe each chunk
//     const transcriptions = [];
//     for (const chunk of chunks) {
//       const result = await transcribeAudio(chunk);
//       transcriptions.push(result.text);
//       fs.unlinkSync(chunk);  // Clean up
//     }
//
//     return transcriptions.join(' ');
//   }
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
// Response formats:
//
//   JSON (default):
//     {"text": "Transcription here"}
//
//   Text (plain text):
//     Transcription here
//
//   SRT (SubRip subtitles):
//     1
//     00:00:00,000 --> 00:00:05,000
//     Transcription here
//
//   VTT (WebVTT subtitles):
//     WEBVTT
//
//     00:00:00.000 --> 00:00:05.000
//     Transcription here
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
//   - Use appropriate quality (don't over-sample)
//   - Cache transcriptions to avoid re-processing
